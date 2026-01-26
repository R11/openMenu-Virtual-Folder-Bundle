#ifndef DCNOW_API_H
#define DCNOW_API_H

#include <stdint.h>
#include <stdbool.h>

/* Maximum number of games that can be displayed */
#define MAX_DCNOW_GAMES 32

/* Maximum length for game names */
#define MAX_GAME_NAME_LEN 64

/**
 * Structure representing a single game's active player data
 */
typedef struct {
    char game_name[MAX_GAME_NAME_LEN];
    int player_count;
    bool is_active;
} dcnow_game_info_t;

/**
 * Structure representing the complete DC Now data from dreamcast.online/now
 */
typedef struct {
    dcnow_game_info_t games[MAX_DCNOW_GAMES];
    int game_count;
    int total_players;
    bool data_valid;
    char error_message[128];
    uint32_t last_update_time;
} dcnow_data_t;

/**
 * Initialize the DC Now network subsystem
 * This should be called once at startup
 *
 * @return 0 on success, negative error code on failure
 *
 * TODO: Implement actual network initialization:
 *       - Initialize lwIP network stack
 *       - Configure DreamPi dial-up connection
 *       - Set up DNS resolution
 */
int dcnow_init(void);

/**
 * Shutdown the DC Now network subsystem
 *
 * TODO: Implement network cleanup:
 *       - Close any open connections
 *       - Free network resources
 */
void dcnow_shutdown(void);

/**
 * Fetch the current active player data from dreamcast.online/now
 *
 * This function connects via DreamPi (dial-up modem emulation) to fetch
 * JSON data from dreamcast.online/now and parse it into the dcnow_data_t structure.
 *
 * @param data Pointer to dcnow_data_t structure to fill with results
 * @param timeout_ms Timeout in milliseconds for the network operation
 * @return 0 on success, negative error code on failure
 *
 * TODO: Implement actual network fetch:
 *       - Establish HTTP connection to dreamcast.online
 *       - Send GET request to /now endpoint
 *       - Parse JSON response (consider using cJSON or similar)
 *       - Populate dcnow_data_t structure with parsed data
 *       - Handle network errors gracefully
 *
 * Example JSON format expected from dreamcast.online/now:
 * {
 *   "games": [
 *     {"name": "Phantasy Star Online", "players": 5},
 *     {"name": "Quake III Arena", "players": 2}
 *   ],
 *   "total_players": 7
 * }
 */
int dcnow_fetch_data(dcnow_data_t *data, uint32_t timeout_ms);

/**
 * Get a cached copy of the most recent DC Now data
 * This can be used to avoid repeated network calls
 *
 * @param data Pointer to dcnow_data_t structure to fill with cached results
 * @return true if cached data is available and valid, false otherwise
 */
bool dcnow_get_cached_data(dcnow_data_t *data);

/**
 * Clear the cached data
 */
void dcnow_clear_cache(void);

#endif /* DCNOW_API_H */
