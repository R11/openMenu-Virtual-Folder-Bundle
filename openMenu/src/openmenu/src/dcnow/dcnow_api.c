#include "dcnow_api.h"
#include "dcnow_json.h"
#include <string.h>
#include <stdio.h>

#ifdef _arch_dreamcast
#include <kos.h>
#include <kos/net.h>
#include <kos/thread.h>
#include <arch/timer.h>
#include <dc/modem/modem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

/* Cached data from last successful fetch */
static dcnow_data_t cached_data = {0};
static bool cache_valid = false;
static bool network_initialized = false;

int dcnow_init(void) {
#ifdef _arch_dreamcast
    /* Initialize the cache */
    memset(&cached_data, 0, sizeof(cached_data));
    cache_valid = false;

    if (network_initialized) {
        return 0;  /* Already initialized */
    }

    /* Initialize KOS network subsystem */
    /* This auto-detects BBA or modem and initializes appropriately */
    printf("DC Now: Initializing network...\n");

    if (net_init() < 0) {
        printf("DC Now: Network initialization failed\n");
        return -1;
    }

    /* Wait a moment for network to stabilize */
    thd_sleep(500);  /* 500ms */

    /* Check if we have a valid network device */
    /* net_default_dev should be set by net_init() */
    if (!net_default_dev) {
        printf("DC Now: No network device found\n");
        printf("DC Now: Make sure BBA is connected or modem is configured\n");
        return -2;
    }

    printf("DC Now: Network device: %s\n", net_default_dev->name);

    /* For modem/DreamPi: Check if PPP is configured */
    if (strncmp(net_default_dev->name, "ppp", 3) == 0) {
        printf("DC Now: Modem/DreamPi detected\n");
        printf("DC Now: Note: Modem must be configured via modem_init() before openMenu starts\n");
        /* The modem should already be initialized and connected by this point */
        /* If not, we'll get connection errors when trying to use sockets */
    }
    /* For BBA: Should be auto-configured by net_init() */
    else if (strncmp(net_default_dev->name, "bba", 3) == 0) {
        printf("DC Now: Broadband Adapter detected\n");
    }

    /* Wait for link to be established */
    int retry = 0;
    while (retry < 20) {  /* 10 seconds max */
        if (net_default_dev->if_flags & NETIF_FLAG_LINK_UP) {
            break;
        }
        thd_sleep(500);
        retry++;
    }

    if (!(net_default_dev->if_flags & NETIF_FLAG_LINK_UP)) {
        printf("DC Now: Network link not established\n");
        return -3;
    }

    printf("DC Now: Network link up\n");
    network_initialized = true;
    return 0;
#else
    /* Non-Dreamcast platforms - just initialize cache */
    memset(&cached_data, 0, sizeof(cached_data));
    cache_valid = false;
    return 0;
#endif
}

void dcnow_shutdown(void) {
    cache_valid = false;
    /* Note: We don't call net_shutdown() as other parts of the system may be using the network */
}

#ifdef _arch_dreamcast
static int http_get_request(const char* hostname, const char* path, char* response_buf, int buf_size, uint32_t timeout_ms) {
    int sock = -1;
    struct hostent *host;
    struct sockaddr_in server_addr;
    int result = -1;
    char request_buf[512];
    int total_received = 0;
    uint64_t start_time;
    uint64_t timeout_ticks;

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("DC Now: Socket creation failed (errno: %d)\n", errno);
        return -2;  /* Socket creation failed */
    }

    /* Resolve hostname */
    printf("DC Now: Resolving %s...\n", hostname);
    host = gethostbyname(hostname);
    if (!host) {
        printf("DC Now: DNS lookup failed for %s\n", hostname);
        close(sock);
        return -3;  /* DNS resolution failed */
    }

    printf("DC Now: Resolved to %s\n", inet_ntoa(*(struct in_addr*)host->h_addr));

    /* Setup server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    /* Connect to server */
    printf("DC Now: Connecting...\n");
    start_time = timer_ms_gettime64();
    timeout_ticks = timeout_ms;

    int connect_result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (connect_result < 0) {
        /* For blocking sockets, connect should succeed or fail immediately on Dreamcast */
        printf("DC Now: Connection failed (errno: %d)\n", errno);
        close(sock);
        return -4;
    }

    printf("DC Now: Connected\n");

    /* Build HTTP GET request */
    snprintf(request_buf, sizeof(request_buf),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: openMenu-Dreamcast/1.1-ateam\r\n"
             "Accept: application/json\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, hostname);

    /* Send request */
    printf("DC Now: Sending request...\n");
    int sent = send(sock, request_buf, strlen(request_buf), 0);
    if (sent <= 0) {
        printf("DC Now: Send failed (errno: %d)\n", errno);
        close(sock);
        return -5;  /* Send failed */
    }

    printf("DC Now: Request sent, waiting for response...\n");

    /* Receive response */
    start_time = timer_ms_gettime64();
    total_received = 0;

    while (total_received < buf_size - 1) {
        if (timer_ms_gettime64() - start_time > timeout_ticks) {
            printf("DC Now: Receive timeout\n");
            break;  /* Timeout - but we may have received some data */
        }

        int received = recv(sock, response_buf + total_received,
                           buf_size - total_received - 1, 0);

        if (received > 0) {
            total_received += received;
            start_time = timer_ms_gettime64();  /* Reset timeout on successful receive */
        } else if (received == 0) {
            /* Connection closed by server - this is normal */
            printf("DC Now: Server closed connection\n");
            break;
        } else {
            /* Error receiving */
            if (total_received == 0) {
                printf("DC Now: Receive failed (errno: %d)\n", errno);
                close(sock);
                return -6;  /* Receive failed */
            }
            break;  /* We got some data, so continue */
        }

        thd_pass();  /* Yield to other threads */
    }

    response_buf[total_received] = '\0';
    close(sock);

    printf("DC Now: Received %d bytes\n", total_received);

    return (total_received > 0) ? total_received : -6;
}
#endif

int dcnow_fetch_data(dcnow_data_t *data, uint32_t timeout_ms) {
    if (!data) {
        return -1;
    }

    /* Clear the data structure */
    memset(data, 0, sizeof(dcnow_data_t));

#ifdef _arch_dreamcast
    /* Check if network is initialized */
    if (!network_initialized) {
        strcpy(data->error_message, "Network not initialized");
        data->data_valid = false;
        return -11;
    }

    char response[8192];
    int result;

    printf("DC Now: Fetching data from dreamcast.online/now...\n");

    /* Perform HTTP GET request */
    result = http_get_request("dreamcast.online", "/now", response, sizeof(response), timeout_ms);

    if (result < 0) {
        /* Network error */
        switch (result) {
            case -2: strcpy(data->error_message, "Socket creation failed"); break;
            case -3: strcpy(data->error_message, "DNS lookup failed"); break;
            case -4: strcpy(data->error_message, "Connection failed"); break;
            case -5: strcpy(data->error_message, "Failed to send request"); break;
            case -6: strcpy(data->error_message, "Failed to receive data"); break;
            default: strcpy(data->error_message, "Network error"); break;
        }
        printf("DC Now: Error - %s\n", data->error_message);
        data->data_valid = false;
        return result;
    }

    /* Find the JSON body (skip HTTP headers) */
    char *json_start = strstr(response, "\r\n\r\n");
    if (!json_start) {
        strcpy(data->error_message, "Invalid HTTP response");
        data->data_valid = false;
        printf("DC Now: Invalid HTTP response\n");
        return -7;
    }
    json_start += 4;  /* Skip the \r\n\r\n */

    /* Check for HTTP error status */
    if (strncmp(response, "HTTP/1.", 7) == 0) {
        /* Extract status code */
        char *status_line = response;
        char *status_code_start = strchr(status_line, ' ');
        if (status_code_start) {
            status_code_start++;
            int status_code = atoi(status_code_start);
            if (status_code != 200) {
                snprintf(data->error_message, sizeof(data->error_message),
                        "HTTP error %d", status_code);
                data->data_valid = false;
                printf("DC Now: HTTP error %d\n", status_code);
                return -8;
            }
        }
    }

    printf("DC Now: Parsing JSON...\n");

    /* Parse JSON */
    json_dcnow_t json_result;
    if (!dcnow_json_parse(json_start, &json_result)) {
        strcpy(data->error_message, "JSON parse error");
        data->data_valid = false;
        printf("DC Now: JSON parse failed\n");
        return -9;
    }

    if (!json_result.valid) {
        strcpy(data->error_message, "Invalid JSON data");
        data->data_valid = false;
        printf("DC Now: Invalid JSON data\n");
        return -10;
    }

    printf("DC Now: Successfully parsed %d games, %d total players\n",
           json_result.game_count, json_result.total_players);

    /* Copy parsed data to result structure */
    data->total_players = json_result.total_players;
    data->game_count = json_result.game_count;

    for (int i = 0; i < json_result.game_count && i < MAX_DCNOW_GAMES; i++) {
        strncpy(data->games[i].game_name, json_result.games[i].name, MAX_GAME_NAME_LEN - 1);
        data->games[i].game_name[MAX_GAME_NAME_LEN - 1] = '\0';
        data->games[i].player_count = json_result.games[i].players;
        data->games[i].is_active = (json_result.games[i].players > 0);
        printf("DC Now:   %s - %d players\n",
               data->games[i].game_name, data->games[i].player_count);
    }

    data->data_valid = true;
    data->last_update_time = (uint32_t)timer_ms_gettime64();

    /* Cache the data */
    memcpy(&cached_data, data, sizeof(dcnow_data_t));
    cache_valid = true;

    printf("DC Now: Data fetch complete\n");
    return 0;

#else
    /* Non-Dreamcast platforms - return stub data or error */
    #ifdef DCNOW_USE_STUB_DATA
    /* Populate with stub data for testing on non-DC platforms */
    strcpy(data->games[0].game_name, "Phantasy Star Online");
    data->games[0].player_count = 12;
    data->games[0].is_active = true;

    strcpy(data->games[1].game_name, "Quake III Arena");
    data->games[1].player_count = 4;
    data->games[1].is_active = true;

    strcpy(data->games[2].game_name, "Toy Racer");
    data->games[2].player_count = 2;
    data->games[2].is_active = true;

    strcpy(data->games[3].game_name, "4x4 Evolution");
    data->games[3].player_count = 0;
    data->games[3].is_active = false;

    strcpy(data->games[4].game_name, "Starlancer");
    data->games[4].player_count = 1;
    data->games[4].is_active = true;

    data->game_count = 5;
    data->total_players = 19;
    data->data_valid = true;
    data->last_update_time = 0;

    /* Cache the stub data */
    memcpy(&cached_data, data, sizeof(dcnow_data_t));
    cache_valid = true;

    return 0;
    #else
    strcpy(data->error_message, "Network not available");
    data->data_valid = false;
    return -100;
    #endif
#endif
}

bool dcnow_get_cached_data(dcnow_data_t *data) {
    if (!data || !cache_valid) {
        return false;
    }

    memcpy(data, &cached_data, sizeof(dcnow_data_t));
    return true;
}

void dcnow_clear_cache(void) {
    memset(&cached_data, 0, sizeof(cached_data));
    cache_valid = false;
}
