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
#define JSON_MAX_CODE_LEN 16
#define JSON_MAX_PLAYERS_PER_GAME 16
#define JSON_MAX_USERNAME_LEN 32
#define JSON_MAX_LEVEL_LEN 32

/* Player details structure - minimal info only */
typedef struct {
    char level[JSON_MAX_LEVEL_LEN];  /* e.g., "Newbie", "Occasional Gamer", "Enthusiastic Gamer" */
    char country[4];                 /* 2-letter country code like "US", "UK", "JP" */
} json_player_details_t;

/* Parsed game structure */
typedef struct {
    char name[JSON_MAX_NAME_LEN];      /* Display name (e.g., "Phantasy Star Online") */
    char code[JSON_MAX_CODE_LEN];      /* Short code (e.g., "PSO") */
    int players;
    char player_names[JSON_MAX_PLAYERS_PER_GAME][JSON_MAX_USERNAME_LEN];  /* List of usernames */
    json_player_details_t player_details[JSON_MAX_PLAYERS_PER_GAME];  /* Level and country per player */
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
 * Expected format (from /now/api/users.json):
 * {
 *   "users": [
 *     {"username": "player1", "current_game_display": "Phantasy Star Online", ...},
 *     {"username": "player2", "current_game_display": "Quake III", ...}
 *   ],
 *   "total_count": 7,
 *   "online_count": 7
 * }
 *
 * This parser aggregates users by game and counts players per game.
 *
 * @param json_str Null-terminated JSON string
 * @param result Pointer to result structure to fill
 * @return true on success, false on parse error
 */
bool dcnow_json_parse(const char* json_str, json_dcnow_t* result);

#endif /* DCNOW_JSON_H */
