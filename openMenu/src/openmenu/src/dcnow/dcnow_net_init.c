#include "dcnow_net_init.h"
#include "dcnow_vmu.h"
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
    printf("DC Now STATUS: %s\n", message);

    /* Log to RAM disk for debugging without serial cable */
    /* /ram/ is writable, unlike /cd/ which is read-only */
    FILE* logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
    if (logfile) {
        fprintf(logfile, "STATUS: %s\n", message);
        fclose(logfile);
    } else {
        printf("DC Now: WARNING - Failed to open log file\n");
    }

    if (status_callback) {
        printf("DC Now: Calling status callback...\n");
        /* Call callback which will draw the message */
        /* Callback is responsible for full scene rendering */
        status_callback(message);
        printf("DC Now: Status callback returned\n");
        /* Give user time to see the message */
        timer_spin_sleep(500);  /* 500ms delay so messages are visible */
    } else {
        printf("DC Now: WARNING - No status callback set!\n");
        logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
        if (logfile) {
            fprintf(logfile, "ERROR: No status callback!\n");
            fclose(logfile);
        }
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

    /* Initialize modem hardware */
    if (!modem_init()) {
        update_status("Modem init failed!");
        return -1;
    }

    /* Force 14400 baud for faster connection */
    /* MODEM_MODE_REMOTE = 0, MODEM_SPEED_V8_14400 = 0x86 */
    update_status("Setting modem speed to 14400...");
    modem_set_mode(0 /* MODEM_MODE_REMOTE */, 0x86 /* MODEM_SPEED_V8_14400 */);

    /* Initialize PPP subsystem */
    if (ppp_init() < 0) {
        update_status("PPP init failed!");
        return -2;
    }

    /* Dial modem (using DreamPi dummy number) */
    update_status("Dialing...");
    int err = ppp_modem_init("111-1111", 1, NULL);
    if (err) {
        update_status("Dial failed!");
        ppp_shutdown();
        return -3;
    }

    /* Set login credentials */
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

    /* ppp_connect() is BLOCKING - returns when connection is established */
    update_status("Connected!");
    printf("DC Now: ppp_connect() succeeded\n");

    return 0;

#else
    /* Non-Dreamcast - no network */
    return -1;
#endif
}

void dcnow_net_disconnect(void) {
#ifdef _arch_dreamcast
    printf("DC Now: Disconnecting network...\n");

    /* Restore VMU to OpenMenu logo when disconnecting */
    dcnow_vmu_restore_logo();

    /* Log to RAM disk for debugging */
    FILE* logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
    if (logfile) {
        fprintf(logfile, "Disconnecting network...\n");
        fclose(logfile);
    }

    /* Check if we have a network device */
    if (!net_default_dev) {
        printf("DC Now: No network device to disconnect\n");
        return;
    }

    /* Check if it's a PPP connection (modem) */
    if (strncmp(net_default_dev->name, "ppp", 3) == 0) {
        printf("DC Now: Shutting down PPP connection...\n");
        ppp_shutdown();

        /* Give PPP time to fully shut down */
        timer_spin_sleep(800);

        printf("DC Now: Shutting down modem hardware...\n");
        modem_shutdown();

        /* CRITICAL: Give modem hardware EXTRA time to fully reset */
        /* Without sufficient delay, subsequent modem_init() calls may fail */
        /* and arch_exec/reset may leave hardware in bad state causing black screen */
        timer_spin_sleep(2500);

        printf("DC Now: Modem and PPP disconnected\n");

        logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
        if (logfile) {
            fprintf(logfile, "PPP and modem disconnected successfully\n");
            fclose(logfile);
        }

        /* Reset network state to NULL so future init knows to reinitialize */
        net_default_dev = NULL;
        printf("DC Now: Network state reset to NULL\n");
    } else {
        /* BBA doesn't need special disconnect handling */
        printf("DC Now: Network device is not modem (BBA), no disconnect needed\n");
    }
#else
    /* Non-Dreamcast - nothing to do */
#endif
}
