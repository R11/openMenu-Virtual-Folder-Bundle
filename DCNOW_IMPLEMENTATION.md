# DC Now (dreamcast.online/now) Implementation Guide

## Overview

This document describes the implementation of the "DC Now" feature for openMenu, which displays active player counts for Dreamcast Live games by fetching data from `dreamcast.online/now`.

## Feature Description

The DC Now feature adds a popup window that shows:
- Total active players across all Dreamcast Live games
- List of games with their current player counts
- Real-time status (online/offline)

## User Interface

### Accessing DC Now

Press **L + R triggers together** to open the DC Now popup from any UI mode (Folders, Scroll, Grid, or LineDesc).

### Popup Controls

- **Up/Down**: Scroll through the game list
- **A or B**: Close the popup
- **START**: Refresh data from server

## Implementation Status

### ✅ Completed

1. **UI Implementation** - Full popup UI with support for all view modes
2. **API Layer** - Complete stub implementation with clear TODOs
3. **Integration** - Integrated into all UI modes (Folders, Scroll, Grid, LineDesc)
4. **Build System** - CMakeLists.txt updated

### ⚠️ Pending - Network Implementation

The network functionality is **NOT YET IMPLEMENTED**. The code is structured with clear TODOs marking where actual network code needs to be added.

## Testing with Stub Data

To test the UI with dummy data before implementing the actual network connection:

1. Edit `/openMenu/src/openmenu/src/dcnow/dcnow_api.c`
2. Find the `#ifdef DCNOW_USE_STUB_DATA` section in the `dcnow_fetch_data()` function
3. To enable stub data, add this compile definition to CMakeLists.txt:

```cmake
target_compile_definitions(openmenu PRIVATE
    OPENMENU_BUILD_VERSION="${OPENMENU_VERSION}"
    DCNOW_USE_STUB_DATA=1
)
```

With stub data enabled, the popup will display sample game data for UI testing.

## Network Implementation TODOs

### Required Components

To complete the network implementation, you'll need to integrate these components into the openMenu build:

1. **lwIP** - Lightweight IP stack for embedded systems (already available in KallistiOS)
2. **HTTP Client** - To make GET requests to dreamcast.online
3. **JSON Parser** - Recommended: `cJSON` (lightweight, MIT license)
4. **DreamPi Modem Support** - Configure network interface for dial-up emulation

### Step-by-Step Implementation

#### 1. Add Network Libraries

Add lwIP and cJSON to the build system:

```cmake
# In openMenu/src/openmenu/CMakeLists.txt
target_link_libraries(openmenu
    PRIVATE
    crayon_savefile
    easing
    ini
    openmenu_settings
    openmenu_shared
    uthash
    lwip          # Add network stack
    cjson         # Add JSON parser
)
```

#### 2. Initialize Network Stack

In `dcnow_init()` (dcnow_api.c):

```c
#include <kos/net.h>

int dcnow_init(void) {
    /* Initialize KOS network subsystem */
    if (net_init() < 0) {
        return -1;
    }

    /* Configure for DreamPi modem connection */
    // TODO: Add DreamPi-specific configuration
    // This may involve setting up the modem device
    // and configuring dial-up parameters

    /* Initialize lwIP */
    // lwIP should be initialized as part of net_init()

    memset(&cached_data, 0, sizeof(cached_data));
    cache_valid = false;

    return 0;
}
```

#### 3. Implement HTTP GET Request

In `dcnow_fetch_data()` (dcnow_api.c), replace the stub implementation with:

```c
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <cJSON.h>

int dcnow_fetch_data(dcnow_data_t *data, uint32_t timeout_ms) {
    if (!data) {
        return -1;
    }

    memset(data, 0, sizeof(dcnow_data_t));

    /* 1. Create TCP socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        strcpy(data->error_message, "Failed to create socket");
        return -2;
    }

    /* 2. Set socket timeout */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    /* 3. Resolve dreamcast.online */
    struct hostent *host = gethostbyname("dreamcast.online");
    if (!host) {
        strcpy(data->error_message, "DNS lookup failed");
        close(sock);
        return -3;
    }

    /* 4. Connect to server */
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    memcpy(&server.sin_addr, host->h_addr, host->h_length);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        strcpy(data->error_message, "Connection failed");
        close(sock);
        return -4;
    }

    /* 5. Send HTTP GET request */
    const char *request =
        "GET /now HTTP/1.1\r\n"
        "Host: dreamcast.online\r\n"
        "User-Agent: openMenu-Dreamcast/1.1\r\n"
        "Connection: close\r\n"
        "\r\n";

    if (send(sock, request, strlen(request), 0) < 0) {
        strcpy(data->error_message, "Failed to send request");
        close(sock);
        return -5;
    }

    /* 6. Receive response */
    char response[4096];
    int total_received = 0;
    int bytes_received;

    while ((bytes_received = recv(sock, response + total_received,
                                   sizeof(response) - total_received - 1, 0)) > 0) {
        total_received += bytes_received;
        if (total_received >= sizeof(response) - 1) {
            break;  /* Buffer full */
        }
    }

    response[total_received] = '\0';
    close(sock);

    if (total_received == 0) {
        strcpy(data->error_message, "No data received");
        return -6;
    }

    /* 7. Find JSON body (skip HTTP headers) */
    char *json_start = strstr(response, "\r\n\r\n");
    if (!json_start) {
        strcpy(data->error_message, "Invalid HTTP response");
        return -7;
    }
    json_start += 4;  /* Skip the \r\n\r\n */

    /* 8. Parse JSON */
    cJSON *json = cJSON_Parse(json_start);
    if (!json) {
        strcpy(data->error_message, "JSON parse error");
        return -8;
    }

    /* 9. Extract game data */
    cJSON *games_array = cJSON_GetObjectItem(json, "games");
    cJSON *total_players = cJSON_GetObjectItem(json, "total_players");

    if (!games_array || !cJSON_IsArray(games_array)) {
        cJSON_Delete(json);
        strcpy(data->error_message, "Invalid JSON format");
        return -9;
    }

    /* 10. Populate game list */
    int game_idx = 0;
    cJSON *game = NULL;
    cJSON_ArrayForEach(game, games_array) {
        if (game_idx >= MAX_DCNOW_GAMES) {
            break;  /* Hit maximum games limit */
        }

        cJSON *name = cJSON_GetObjectItem(game, "name");
        cJSON *players = cJSON_GetObjectItem(game, "players");

        if (name && cJSON_IsString(name)) {
            strncpy(data->games[game_idx].game_name,
                    name->valuestring,
                    MAX_GAME_NAME_LEN - 1);
            data->games[game_idx].game_name[MAX_GAME_NAME_LEN - 1] = '\0';
        }

        if (players && cJSON_IsNumber(players)) {
            data->games[game_idx].player_count = players->valueint;
            data->games[game_idx].is_active = (players->valueint > 0);
        }

        game_idx++;
    }

    data->game_count = game_idx;
    data->total_players = (total_players && cJSON_IsNumber(total_players))
                          ? total_players->valueint : 0;
    data->data_valid = true;
    data->last_update_time = time(NULL);  /* Or use KOS timer */

    /* 11. Cache the data */
    memcpy(&cached_data, data, sizeof(dcnow_data_t));
    cache_valid = true;

    cJSON_Delete(json);
    return 0;
}
```

#### 4. Implement Cleanup

In `dcnow_shutdown()` (dcnow_api.c):

```c
void dcnow_shutdown(void) {
    /* Shutdown network subsystem */
    net_shutdown();

    cache_valid = false;
}
```

### Expected JSON Format

The implementation expects JSON from `dreamcast.online/now` in this format:

```json
{
  "total_players": 19,
  "games": [
    {
      "name": "Phantasy Star Online",
      "players": 12
    },
    {
      "name": "Quake III Arena",
      "players": 4
    },
    {
      "name": "Toy Racer",
      "players": 2
    }
  ]
}
```

### DreamPi Integration Notes

The DreamPi is a Raspberry Pi-based modem emulator that allows Dreamcast to connect to the internet via dial-up emulation.

**Configuration Requirements:**
1. The Dreamcast must have a BBA (Broadband Adapter) or use the modem port with DreamPi
2. Network initialization may require specific modem AT commands
3. DNS should be configured to use DreamPi's DNS server
4. Consider adding retry logic for connection failures

**Testing:**
- Test with actual DreamPi hardware before deploying
- Handle cases where DreamPi is not available gracefully
- Add user-facing error messages for common connection issues

## Code Structure

### Files Added/Modified

**New Files:**
- `/openMenu/src/openmenu/src/dcnow/dcnow_api.h` - API interface
- `/openMenu/src/openmenu/src/dcnow/dcnow_api.c` - Implementation (with TODOs)
- `/DCNOW_IMPLEMENTATION.md` - This documentation

**Modified Files:**
- `/openMenu/src/openmenu_settings/include/openmenu_settings.h` - Added DRAW_DCNOW_PLAYERS state
- `/openMenu/src/openmenu/src/ui/ui_menu_credits.h` - Added DC Now function declarations
- `/openMenu/src/openmenu/src/ui/ui_menu_credits.c` - Added DC Now popup implementation
- `/openMenu/src/openmenu/src/ui/ui_folders.c` - Integrated DC Now popup
- `/openMenu/src/openmenu/src/ui/ui_scroll.c` - Integrated DC Now popup
- `/openMenu/src/openmenu/src/ui/ui_grid.c` - Integrated DC Now popup
- `/openMenu/src/openmenu/src/ui/ui_line_desc.c` - Integrated DC Now popup
- `/openMenu/src/openmenu/CMakeLists.txt` - Added dcnow_api.c to build

### Function Call Flow

```
User presses L+R → handle_input_ui()
                 → dcnow_setup()
                 → dcnow_fetch_data()
                 → [Network request to dreamcast.online/now]
                 → [Parse JSON response]
                 → draw_dcnow_tr() [Display popup]
                 → handle_input_dcnow() [Handle user input]
```

## Future Enhancements

1. **Auto-refresh** - Periodically update data in background
2. **Game details** - Show server names, regions, or game modes
3. **Favorites** - Let users mark favorite games for notifications
4. **Connection indicator** - Visual feedback during network fetch
5. **Offline caching** - Save last known data to VMU

## Troubleshooting

### Stub Data Not Showing

- Verify `DCNOW_USE_STUB_DATA=1` is defined in CMakeLists.txt
- Rebuild the project completely (`make clean && make`)

### Network Connection Fails

- Check DreamPi connection and configuration
- Verify DNS resolution is working
- Increase timeout values for slow connections
- Check firewall rules on DreamPi

### JSON Parse Errors

- Verify the JSON format from dreamcast.online/now matches expected structure
- Add logging to see the raw JSON response
- Check for character encoding issues

## License

This feature follows the same BSD 3-Clause license as openMenu.

## Credits

- Network architecture designed following KallistiOS best practices
- UI implementation follows existing openMenu popup patterns
- Inspired by the Dreamcast Live community's efforts to keep online gaming alive
