#include "dcnow_net_init.h"
#include <stdio.h>
#include <string.h>

#ifdef _arch_dreamcast
#include <kos.h>
#include <kos/net.h>
#include <ppp/ppp.h>
#include <dc/modem/modem.h>
#include <dc/flashrom.h>
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

    /* No BBA detected - try modem initialization using stored ISP config */
    update_status("Reading ISP config...");

    /* Read stored ISP configuration from flashrom (like browser does!) */
    flashrom_ispcfg_t isp_config;
    int config_result = flashrom_get_pw_ispcfg(&isp_config);  /* Try PlanetWeb first */
    if (config_result < 0) {
        config_result = flashrom_get_ispcfg(&isp_config);  /* Fall back to DreamPassport */
    }

    if (config_result < 0) {
        update_status("No ISP config found!");
        printf("DC Now: ERROR - No stored ISP configuration found in flashrom\n");
        printf("DC Now: Please configure ISP settings using Dreamcast browser first\n");
        return -1;
    }

    printf("DC Now: ISP config loaded from flashrom\n");
    printf("DC Now: Phone: %s\n", isp_config.phone1);
    printf("DC Now: PPP Login: %s\n", isp_config.ppp_login);

    update_status("Initializing modem...");

    /* Initialize modem hardware */
    if (!modem_init()) {
        update_status("Modem init failed!");
        return -2;
    }

    /* Initialize PPP subsystem */
    if (ppp_init() < 0) {
        update_status("PPP init failed!");
        return -3;
    }

    /* Dial using stored phone number */
    update_status("Dialing...");
    int err = ppp_modem_init(isp_config.phone1, 1, NULL);
    if (err) {
        update_status("Dial failed!");
        ppp_shutdown();
        return -4;
    }

    /* Use stored PPP credentials */
    if (ppp_set_login(isp_config.ppp_login, isp_config.ppp_passwd) < 0) {
        update_status("Login setup failed!");
        ppp_shutdown();
        return -5;
    }

    /* Establish PPP connection */
    update_status("Connecting...");
    err = ppp_connect();
    if (err) {
        update_status("Connection failed!");
        ppp_shutdown();
        return -6;
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
