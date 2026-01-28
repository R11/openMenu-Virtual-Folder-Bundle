#include "dcnow_json.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

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

    /* Parse online_count (total players) */
    const char* online_val = find_key(p, "online_count");
    if (online_val) {
        const char* next = parse_number(online_val, &result->total_players);
        if (!next) {
            result->total_players = 0;
        }
    }

    /* Find users array */
    const char* users_val = find_key(p, "users");
    if (!users_val || *users_val != '[') {
        /* No users array - return empty but valid */
        result->valid = true;
        return true;
    }

    users_val++;  /* Skip opening bracket */
    users_val = skip_whitespace(users_val);

    /* Parse each user and aggregate games */
    int user_count = 0;
    int users_with_games = 0;
    int users_without_games = 0;

    while (*users_val && *users_val != ']') {
        users_val = skip_whitespace(users_val);

        if (*users_val != '{') {
            break;  /* Not an object */
        }
        users_val++;  /* Skip opening brace */
        user_count++;

        /* Find current_game_display field */
        const char* game_val = find_key(users_val, "current_game_display");
        bool has_game = false;

        if (game_val && *game_val == '"') {
            char game_name[JSON_MAX_NAME_LEN];
            const char* next = parse_string(game_val, game_name, JSON_MAX_NAME_LEN);

            if (next && game_name[0] != '\0') {
                /* User has a game */
                users_with_games++;
                has_game = true;

                /* Find existing game or add new one */
                int found_idx = -1;
                for (int i = 0; i < result->game_count; i++) {
                    if (strcmp(result->games[i].name, game_name) == 0) {
                        found_idx = i;
                        break;
                    }
                }

                if (found_idx >= 0) {
                    /* Increment existing game count */
                    result->games[found_idx].players++;
                } else if (result->game_count < JSON_MAX_GAMES) {
                    /* Add new game */
                    strncpy(result->games[result->game_count].name, game_name, JSON_MAX_NAME_LEN - 1);
                    result->games[result->game_count].name[JSON_MAX_NAME_LEN - 1] = '\0';
                    result->games[result->game_count].players = 1;
                    result->game_count++;
                }
            }
        }

        if (!has_game) {
            /* User is idle/not in a game - count them separately */
            users_without_games++;
            printf("DC Now: User %d is idle/not in game\n", user_count);
        }

        /* Skip to end of user object */
        int brace_count = 1;
        while (*users_val && brace_count > 0) {
            if (*users_val == '{') brace_count++;
            else if (*users_val == '}') brace_count--;
            users_val++;
        }

        /* Skip comma if present */
        users_val = skip_whitespace(users_val);
        if (*users_val == ',') {
            users_val++;
        }
    }

    printf("DC Now: Parsed %d users total - %d with games, %d without games\n",
           user_count, users_with_games, users_without_games);
    printf("DC Now: Total players from API: %d\n", result->total_players);

    /* Add idle users as a separate entry if any */
    if (users_without_games > 0 && result->game_count < JSON_MAX_GAMES) {
        strncpy(result->games[result->game_count].name, "Idle/Not in game", JSON_MAX_NAME_LEN - 1);
        result->games[result->game_count].name[JSON_MAX_NAME_LEN - 1] = '\0';
        result->games[result->game_count].players = users_without_games;
        result->game_count++;
    }

    result->valid = true;
    return true;
}
