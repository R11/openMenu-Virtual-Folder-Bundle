#include "dcnow_net_init.h"
#include <stdio.h>

#ifdef _arch_dreamcast
#include <kos.h>
#include <kos/net.h>
#include <ppp/ppp.h>
#include <dc/modem/modem.h>
#include <arch/timer.h>
#endif

int dcnow_net_early_init(void) {
#ifdef _arch_dreamcast
    printf("DC Now: Early network initialization...\n");

    /* Check if BBA is already active (like ClassiCube does) */
    if (net_default_dev) {
        printf("DC Now: Network already initialized (BBA detected)\n");
        printf("DC Now: Device: %s\n", net_default_dev->name);
        return 0;  /* BBA already active, we're done */
    }

    /* No BBA detected - try modem initialization (DreamPi dial-up) */
    printf("DC Now: No BBA detected, attempting DreamPi modem dial-up...\n");

    /* Initialize modem hardware FIRST (like ClassiCube does) */
    printf("DC Now: Initializing modem hardware...\n");
    if (!modem_init()) {
        printf("DC Now: Modem hardware initialization failed\n");
        printf("DC Now: DC Now feature will be unavailable\n");
        return -1;
    }
    printf("DC Now: Modem hardware initialized\n");

    /* Initialize PPP subsystem */
    printf("DC Now: Initializing PPP subsystem...\n");
    if (ppp_init() < 0) {
        printf("DC Now: Failed to initialize PPP subsystem\n");
        return -2;
    }

    /* Dial modem (using DreamPi dummy number like ClassiCube) */
    /* ClassiCube uses "111111111111", DreamPi typically uses "555" */
    printf("DC Now: Dialing DreamPi (this can take ~20 seconds)...\n");
    int err = ppp_modem_init("555", 1, NULL);
    if (err) {
        printf("DC Now: Failed to dial modem (error %d)\n", err);
        ppp_shutdown();
        return -3;
    }

    /* Set login credentials (ClassiCube uses "dream"/"dreamcast") */
    printf("DC Now: Setting PPP login credentials...\n");
    if (ppp_set_login("dream", "dreamcast") < 0) {
        printf("DC Now: Failed to set login credentials\n");
        ppp_shutdown();
        return -4;
    }

    /* Establish PPP connection */
    printf("DC Now: Connecting PPP (this can take ~20 seconds)...\n");
    err = ppp_connect();
    if (err) {
        printf("DC Now: Failed to connect PPP (error %d)\n", err);
        ppp_shutdown();
        return -5;
    }

    printf("DC Now: PPP connection initiated, waiting for link up...\n");

    /* Wait for PPP connection to be established (max 40 seconds for safety) */
    int wait_count = 0;
    int max_wait = 400; /* 40 seconds at 100ms intervals */

    while (wait_count < max_wait) {
        /* Check if link is up */
        if (net_default_dev && (net_default_dev->if_flags & NETIF_FLAG_LINK_UP)) {
            printf("DC Now: PPP connection established!\n");

            /* Display connection info */
            if (net_default_dev->ip_addr[0] != 0) {
                printf("DC Now: IP Address: %d.%d.%d.%d\n",
                       net_default_dev->ip_addr[0],
                       net_default_dev->ip_addr[1],
                       net_default_dev->ip_addr[2],
                       net_default_dev->ip_addr[3]);
            }

            return 0;  /* Success! */
        }

        /* Wait 100ms before checking again */
        timer_spin_sleep(100);
        wait_count++;

        /* Print progress every 5 seconds */
        if (wait_count % 50 == 0) {
            printf("DC Now: Still waiting for connection (%d/%d seconds)...\n",
                   wait_count / 10, max_wait / 10);
        }
    }

    /* Timeout - connection failed */
    printf("DC Now: PPP connection timeout after 40 seconds\n");
    printf("DC Now: Please check:\n");
    printf("DC Now:   - DreamPi is running and configured\n");
    printf("DC Now:   - Phone line cable connected between DC and DreamPi\n");
    printf("DC Now:   - DreamPi has internet connectivity\n");
    printf("DC Now:   - DreamPi settings are correct\n");

    ppp_shutdown();
    return -6;  /* Connection timeout */

#else
    /* Non-Dreamcast - no network */
    return -1;
#endif
}
