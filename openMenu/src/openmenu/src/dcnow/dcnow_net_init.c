#include "dcnow_net_init.h"
#include <stdio.h>
#include <string.h>

#ifdef _arch_dreamcast
#include <kos.h>
#include <kos/net.h>
#include <ppp/ppp.h>
#include <dc/modem/modem.h>
#include <arch/timer.h>
#include <dc/pvr.h>
#endif

/* Status callback for visual feedback */
static dcnow_status_callback_t status_callback = NULL;

void dcnow_set_status_callback(dcnow_status_callback_t callback) {
    status_callback = callback;
}

static void update_status(const char* message) {
    printf("%s\n", message);
    if (status_callback) {
        status_callback(message);
        /* Give PVR time to render the update */
        pvr_wait_ready();
        pvr_scene_begin();
        status_callback(message);  /* Callback draws the message */
        pvr_scene_finish();
    }
}

int dcnow_net_early_init(void) {
#ifdef _arch_dreamcast
    update_status("Initializing network...");

    /* Check if BBA is already active (like ClassiCube does) */
    if (net_default_dev) {
        update_status("Network ready (BBA detected)");
        return 0;  /* BBA already active, we're done */
    }

    /* No BBA detected - try modem initialization (DreamPi dial-up) */
    update_status("Initializing modem...");

    /* Initialize modem hardware FIRST (like ClassiCube does) */
    if (!modem_init()) {
        update_status("Modem init failed!");
        return -1;
    }

    /* Initialize PPP subsystem */
    if (ppp_init() < 0) {
        update_status("PPP init failed!");
        return -2;
    }

    /* Dial modem (using DreamPi dummy number) */
    update_status("Dialing DreamPi...");
    int err = ppp_modem_init("111-1111", 1, NULL);
    if (err) {
        update_status("Dial failed!");
        ppp_shutdown();
        return -3;
    }

    /* Set login credentials (ClassiCube uses "dream"/"dreamcast") */
    if (ppp_set_login("dream", "dreamcast") < 0) {
        update_status("Login setup failed!");
        ppp_shutdown();
        return -4;
    }

    /* Establish PPP connection */
    update_status("Connecting...");
    err = ppp_connect();
    if (err) {
        update_status("Connection failed!");
        ppp_shutdown();
        return -5;
    }

    update_status("Waiting for link...");

    /* Wait for PPP connection to be established (max 40 seconds for safety) */
    int wait_count = 0;
    int max_wait = 400; /* 40 seconds at 100ms intervals */
    char status_msg[64];

    while (wait_count < max_wait) {
        /* Check if link is up by verifying device exists and has an IP */
        if (net_default_dev && net_default_dev->ip_addr[0] != 0) {
            update_status("Connected!");
            return 0;  /* Success! */
        }

        /* Wait 100ms before checking again */
        timer_spin_sleep(100);
        wait_count++;

        /* Update progress every 2 seconds */
        if (wait_count % 20 == 0) {
            snprintf(status_msg, sizeof(status_msg), "Waiting... (%d sec)", wait_count / 10);
            update_status(status_msg);
        }
    }

    /* Timeout - connection failed */
    update_status("Connection timeout!");
    ppp_shutdown();
    return -6;  /* Connection timeout */

#else
    /* Non-Dreamcast - no network */
    return -1;
#endif
}
