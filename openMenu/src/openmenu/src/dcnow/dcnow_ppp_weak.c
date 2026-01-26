/*
 * Weak PPP symbols for builds without libppp.a
 *
 * This file provides weak (default) implementations of PPP functions
 * so the build can succeed even if libppp.a is not available.
 *
 * If libppp.a is linked, these weak symbols will be overridden by the
 * real implementations. If not linked, these stubs will be used and
 * modem support will gracefully fail.
 */

#ifdef _arch_dreamcast

#include <stdio.h>

/* Weak implementations that return error codes */

__attribute__((weak))
int ppp_init(void) {
    printf("DC Now: PPP library not available (using stub)\n");
    return -1;
}

__attribute__((weak))
int ppp_modem_init(const char *number, int blind, int *conn_rate) {
    (void)number;
    (void)blind;
    (void)conn_rate;
    return -1;
}

__attribute__((weak))
int ppp_set_login(const char *username, const char *password) {
    (void)username;
    (void)password;
    return -1;
}

__attribute__((weak))
int ppp_connect(void) {
    return -1;
}

__attribute__((weak))
int ppp_shutdown(void) {
    return 0;
}

#endif /* _arch_dreamcast */
