#include "dcnow_json.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Helper functions */

static const char* skip_whitespace(const char* p) {
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

static const char* parse_string(const char* p, char* out, int max_len) {
    if (*p != '"') return NULL;
    p++;  /* Skip opening quote */

    int i = 0;
    while (*p && *p != '"' && i < max_len - 1) {
        if (*p == '\\') {
            p++;  /* Skip escape char */
            if (!*p) return NULL;
            /* Simple escape handling */
            switch (*p) {
                case 'n': out[i++] = '\n'; break;
                case 't': out[i++] = '\t'; break;
                case 'r': out[i++] = '\r'; break;
                case '"': out[i++] = '"'; break;
                case '\\': out[i++] = '\\'; break;
                default: out[i++] = *p; break;
            }
        } else {
            out[i++] = *p;
        }
        p++;
    }

    out[i] = '\0';

    if (*p != '"') return NULL;
    p++;  /* Skip closing quote */

    return p;
}

static const char* parse_number(const char* p, int* out) {
    int sign = 1;
    int value = 0;

    if (*p == '-') {
        sign = -1;
        p++;
    }

    if (!isdigit((unsigned char)*p)) return NULL;

    while (isdigit((unsigned char)*p)) {
        value = value * 10 + (*p - '0');
        p++;
    }

    *out = value * sign;
    return p;
}

static const char* find_key(const char* p, const char* key) {
    const char* search = p;
    int key_len = strlen(key);

    while (*search) {
        search = skip_whitespace(search);

        if (*search == '"') {
            const char* key_start = search + 1;
            const char* key_end = strchr(key_start, '"');

            if (key_end) {
                int found_len = key_end - key_start;
                if (found_len == key_len && strncmp(key_start, key, key_len) == 0) {
                    /* Found the key, skip to the colon */
                    search = key_end + 1;
                    search = skip_whitespace(search);
                    if (*search == ':') {
                        return skip_whitespace(search + 1);
                    }
                }
            }
        }

        search++;
    }

    return NULL;
}

bool dcnow_json_parse(const char* json_str, json_dcnow_t* result) {
    if (!json_str || !result) {
        return false;
    }

    memset(result, 0, sizeof(json_dcnow_t));

    const char* p = skip_whitespace(json_str);

    /* Expect opening brace */
    if (*p != '{') {
        return false;
    }
    p++;

    /* Parse total_players */
    const char* total_val = find_key(p, "total_players");
    if (total_val) {
        const char* next = parse_number(total_val, &result->total_players);
        if (!next) {
            result->total_players = 0;
        }
    }

    /* Find games array */
    const char* games_val = find_key(p, "games");
    if (!games_val || *games_val != '[') {
        /* No games array, but that's okay - could be empty */
        result->valid = true;
        return true;
    }

    games_val++;  /* Skip opening bracket */
    games_val = skip_whitespace(games_val);

    /* Parse each game object */
    int game_idx = 0;
    while (*games_val && *games_val != ']' && game_idx < JSON_MAX_GAMES) {
        games_val = skip_whitespace(games_val);

        if (*games_val != '{') {
            break;  /* Not an object */
        }
        games_val++;  /* Skip opening brace */

        /* Parse game object */
        json_game_t* game = &result->games[game_idx];

        /* Find "name" field */
        const char* name_val = find_key(games_val, "name");
        if (name_val) {
            const char* next = parse_string(name_val, game->name, JSON_MAX_NAME_LEN);
            if (!next) {
                game->name[0] = '\0';
            }
        }

        /* Find "players" field */
        const char* players_val = find_key(games_val, "players");
        if (players_val) {
            const char* next = parse_number(players_val, &game->players);
            if (!next) {
                game->players = 0;
            }
        }

        /* Skip to end of object */
        int brace_count = 1;
        while (*games_val && brace_count > 0) {
            if (*games_val == '{') brace_count++;
            else if (*games_val == '}') brace_count--;
            games_val++;
        }

        game_idx++;

        /* Skip comma if present */
        games_val = skip_whitespace(games_val);
        if (*games_val == ',') {
            games_val++;
        }
    }

    result->game_count = game_idx;
    result->valid = true;

    return true;
}
