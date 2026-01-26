#include "dcnow_api.h"
#include "dcnow_json.h"
#include <string.h>
#include <stdio.h>

#ifdef _arch_dreamcast
#include <kos/net.h>
#include <kos/thread.h>
#include <arch/timer.h>
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

    /* Initialize KOS network subsystem if not already initialized */
    if (!network_initialized) {
        /* Note: net_init() should have been called at startup */
        /* We'll just mark it as ready */
        network_initialized = true;
    }

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
        return -2;  /* Socket creation failed */
    }

    /* Set socket to non-blocking for timeout support */
    int flags = 1;
    setsockopt(sock, SOL_SOCKET, SO_NONBLOCK, &flags, sizeof(flags));

    /* Resolve hostname */
    host = gethostbyname(hostname);
    if (!host) {
        close(sock);
        return -3;  /* DNS resolution failed */
    }

    /* Setup server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    /* Connect with timeout */
    start_time = timer_ms_gettime64();
    timeout_ticks = timeout_ms;

    int connect_result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    /* Wait for connection with timeout */
    while (connect_result < 0) {
        if (timer_ms_gettime64() - start_time > timeout_ticks) {
            close(sock);
            return -4;  /* Connection timeout */
        }

        /* Check if connection is established */
        fd_set write_fds;
        struct timeval tv;
        FD_ZERO(&write_fds);
        FD_SET(sock, &write_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;  /* 100ms */

        int sel_result = select(sock + 1, NULL, &write_fds, NULL, &tv);
        if (sel_result > 0 && FD_ISSET(sock, &write_fds)) {
            /* Connection established */
            break;
        } else if (sel_result < 0) {
            close(sock);
            return -4;  /* Connection failed */
        }

        thd_pass();  /* Yield to other threads */
    }

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
    int sent = send(sock, request_buf, strlen(request_buf), 0);
    if (sent <= 0) {
        close(sock);
        return -5;  /* Send failed */
    }

    /* Receive response with timeout */
    start_time = timer_ms_gettime64();
    total_received = 0;

    while (total_received < buf_size - 1) {
        if (timer_ms_gettime64() - start_time > timeout_ticks) {
            break;  /* Timeout - but we may have received some data */
        }

        fd_set read_fds;
        struct timeval tv;
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;  /* 100ms */

        int sel_result = select(sock + 1, &read_fds, NULL, NULL, &tv);
        if (sel_result > 0 && FD_ISSET(sock, &read_fds)) {
            int received = recv(sock, response_buf + total_received,
                               buf_size - total_received - 1, 0);

            if (received > 0) {
                total_received += received;
                start_time = timer_ms_gettime64();  /* Reset timeout on successful receive */
            } else if (received == 0) {
                /* Connection closed by server - this is normal after receiving all data */
                break;
            } else {
                /* Error receiving */
                if (total_received == 0) {
                    close(sock);
                    return -6;  /* Receive failed */
                }
                break;  /* We got some data, so continue */
            }
        }

        thd_pass();  /* Yield to other threads */
    }

    response_buf[total_received] = '\0';
    close(sock);

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
    char response[8192];
    int result;

    /* Perform HTTP GET request */
    result = http_get_request("dreamcast.online", "/now", response, sizeof(response), timeout_ms);

    if (result < 0) {
        /* Network error */
        switch (result) {
            case -2: strcpy(data->error_message, "Socket creation failed"); break;
            case -3: strcpy(data->error_message, "DNS lookup failed"); break;
            case -4: strcpy(data->error_message, "Connection failed/timeout"); break;
            case -5: strcpy(data->error_message, "Failed to send request"); break;
            case -6: strcpy(data->error_message, "Failed to receive data"); break;
            default: strcpy(data->error_message, "Network error"); break;
        }
        data->data_valid = false;
        return result;
    }

    /* Find the JSON body (skip HTTP headers) */
    char *json_start = strstr(response, "\r\n\r\n");
    if (!json_start) {
        strcpy(data->error_message, "Invalid HTTP response");
        data->data_valid = false;
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
                return -8;
            }
        }
    }

    /* Parse JSON */
    json_dcnow_t json_result;
    if (!dcnow_json_parse(json_start, &json_result)) {
        strcpy(data->error_message, "JSON parse error");
        data->data_valid = false;
        return -9;
    }

    if (!json_result.valid) {
        strcpy(data->error_message, "Invalid JSON data");
        data->data_valid = false;
        return -10;
    }

    /* Copy parsed data to result structure */
    data->total_players = json_result.total_players;
    data->game_count = json_result.game_count;

    for (int i = 0; i < json_result.game_count && i < MAX_DCNOW_GAMES; i++) {
        strncpy(data->games[i].game_name, json_result.games[i].name, MAX_GAME_NAME_LEN - 1);
        data->games[i].game_name[MAX_GAME_NAME_LEN - 1] = '\0';
        data->games[i].player_count = json_result.games[i].players;
        data->games[i].is_active = (json_result.games[i].players > 0);
    }

    data->data_valid = true;
    data->last_update_time = (uint32_t)timer_ms_gettime64();

    /* Cache the data */
    memcpy(&cached_data, data, sizeof(dcnow_data_t));
    cache_valid = true;

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
