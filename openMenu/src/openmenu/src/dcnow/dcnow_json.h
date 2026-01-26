#ifndef DCNOW_JSON_H
#define DCNOW_JSON_H

/*
 * Minimal JSON parser for DC Now API
 * Only parses the specific format from dreamcast.online/now
 */

#include <stdint.h>
#include <stdbool.h>

/* Maximum values for parsing */
#define JSON_MAX_GAMES 32
#define JSON_MAX_NAME_LEN 64

/* Parsed game structure */
typedef struct {
    char name[JSON_MAX_NAME_LEN];
    int players;
} json_game_t;

/* Parsed JSON data */
typedef struct {
    json_game_t games[JSON_MAX_GAMES];
    int game_count;
    int total_players;
    bool valid;
} json_dcnow_t;

/**
 * Parse DC Now JSON response
 *
 * Expected format:
 * {
 *   "total_players": 19,
 *   "games": [
 *     {"name": "Game 1", "players": 5},
 *     {"name": "Game 2", "players": 3}
 *   ]
 * }
 *
 * @param json_str Null-terminated JSON string
 * @param result Pointer to result structure to fill
 * @return true on success, false on parse error
 */
bool dcnow_json_parse(const char* json_str, json_dcnow_t* result);

#endif /* DCNOW_JSON_H */
