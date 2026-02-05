#include "dcnow_net_init.h"
#include "dcnow_vmu.h"
#include <stdio.h>
#include <string.h>

#ifdef _arch_dreamcast
#include <kos.h>
#include <kos/net.h>
#include <ppp/ppp.h>
#include <dc/modem/modem.h>
#include <dc/scif.h>
#include <arch/timer.h>
#include <dc/pvr.h>
#endif

/* Serial coders cable connection state */
static int serial_connection_active = 0;

#ifdef _arch_dreamcast
/* Helper to write a string to SCIF (serial port) */
static void scif_write_string(const char* str) {
    while (*str) {
        scif_write(*str++);
    }
}
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

/**
 * Try to connect via serial coders cable to DreamPi 2
 * Uses SCIF at 115200 baud with AT command handshake
 *
 * Protocol:
 * 1. Send "AT\r\n" and wait for "OK\r\n" (DreamPi detection)
 * 2. Send "ATDT\r\n" (dial command)
 * 3. Wait for "CONNECT 115200\r\n"
 * 4. Sleep 5 seconds for DreamPi to start pppd
 * 5. Initialize PPP over serial
 *
 * @return 0 on success, negative on error
 */
static int try_serial_coders_cable(void) {
#ifdef _arch_dreamcast
    char buf[64];
    int bytes_read;
    uint64 start_time;
    const uint64 TIMEOUT_MS = 2000;  /* 2 second timeout for responses */

    update_status("Checking for serial cable...");

    /* Initialize SCIF hardware - required before setting parameters */
    scif_init();
    scif_set_irq_usage(0);  /* Disable IRQ mode for polling */
    scif_set_parameters(115200, 1);  /* 115200 baud, FIFO enabled */

    /* Flush any pending data */
    timer_spin_sleep(200);
    while (scif_read() != -1) { /* drain buffer */ }

    /* Small delay after flush */
    timer_spin_sleep(100);

    /* Send AT command to check for listening DreamPi */
    update_status("Sending AT command...");
    scif_write_string("AT\r\n");
    scif_flush();  /* Ensure data is transmitted */
    update_status("Waiting for OK response...");

    /* Small delay for DreamPi to process and respond */
    timer_spin_sleep(200);

    /* Wait for OK response with timeout */
    memset(buf, 0, sizeof(buf));
    bytes_read = 0;
    start_time = timer_ms_gettime64();

    while ((timer_ms_gettime64() - start_time) < TIMEOUT_MS) {
        int c = scif_read();
        if (c != -1 && bytes_read < (int)sizeof(buf) - 1) {
            buf[bytes_read++] = (char)c;
            buf[bytes_read] = '\0';

            /* Check for OK response */
            if (strstr(buf, "OK") != NULL) {
                printf("DC Now: Serial - Got OK response from DreamPi\n");
                break;
            }
        }
        timer_spin_sleep(10);  /* Small delay to avoid busy loop */
    }

    if (strstr(buf, "OK") == NULL) {
        /* Show on screen what we got */
        char status_msg[80];
        snprintf(status_msg, sizeof(status_msg), "No OK - got %d bytes: %.20s", bytes_read, buf);
        update_status(status_msg);
        /* Extra delay so user can see the message */
        timer_spin_sleep(2000);
        /* Restore SCIF to default state for KOS debug output */
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);  /* Re-enable IRQ mode */
        timer_spin_sleep(100);
        return -1;  /* No DreamPi detected on serial */
    }

    /* DreamPi detected! Send dial command */
    update_status("DreamPi found! Dialing...");
    timer_spin_sleep(100);  /* Small delay before dial */

    /* Flush buffer before dial */
    while (scif_read() != -1) { /* drain buffer */ }

    scif_write_string("ATDT\r\n");
    scif_flush();  /* Ensure data is transmitted */

    /* Small delay for DreamPi to process */
    timer_spin_sleep(100);

    /* Wait for CONNECT response */
    memset(buf, 0, sizeof(buf));
    bytes_read = 0;
    start_time = timer_ms_gettime64();
    const uint64 CONNECT_TIMEOUT_MS = 5000;  /* 5 second timeout for CONNECT */

    while ((timer_ms_gettime64() - start_time) < CONNECT_TIMEOUT_MS) {
        int c = scif_read();
        if (c != -1 && bytes_read < (int)sizeof(buf) - 1) {
            buf[bytes_read++] = (char)c;
            buf[bytes_read] = '\0';

            /* Check for CONNECT response */
            if (strstr(buf, "CONNECT") != NULL) {
                printf("DC Now: Serial - Got CONNECT response\n");
                break;
            }
        }
        timer_spin_sleep(10);
    }

    if (strstr(buf, "CONNECT") == NULL) {
        /* Show on screen what we got */
        char status_msg[80];
        snprintf(status_msg, sizeof(status_msg), "No CONNECT - got: %.30s", buf);
        update_status(status_msg);
        timer_spin_sleep(2000);
        /* Restore SCIF to default state */
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);
        timer_spin_sleep(100);
        return -2;  /* Dial failed */
    }

    /* Connection established - wait for DreamPi to start pppd */
    update_status("Connected! Waiting for PPP...");
    timer_spin_sleep(6000);  /* 6 seconds to ensure pppd is ready */

    /* Flush any remaining data in buffer before PPP takes over */
    while (scif_read() != -1) { /* drain buffer */ }

    /* Initialize PPP subsystem */
    if (ppp_init() < 0) {
        update_status("PPP init failed!");
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);
        return -3;
    }

    /* Initialize PPP over SCIF (serial) */
    update_status("Starting PPP (serial)...");
    int err = ppp_scif_init(115200);  /* Use 115200 baud for PPP */
    if (err < 0) {
        char status_msg[80];
        snprintf(status_msg, sizeof(status_msg), "ppp_scif_init failed: %d", err);
        update_status(status_msg);
        timer_spin_sleep(2000);
        ppp_shutdown();
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);
        return -4;
    }

    /* Set login credentials */
    if (ppp_set_login("dream", "dreamcast") < 0) {
        update_status("Login setup failed!");
        ppp_shutdown();
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);
        return -5;
    }

    /* Establish PPP connection */
    update_status("Connecting PPP...");
    err = ppp_connect();
    if (err) {
        char status_msg[80];
        snprintf(status_msg, sizeof(status_msg), "ppp_connect failed: %d", err);
        update_status(status_msg);
        timer_spin_sleep(2000);
        ppp_shutdown();
        scif_set_parameters(57600, 1);
        scif_set_irq_usage(1);
        return -6;
    }

    update_status("Connected via serial!");
    serial_connection_active = 1;
    printf("DC Now: Serial coders cable connection established!\n");
    return 0;

#else
    return -1;
#endif
}

int dcnow_net_early_init(void) {
#ifdef _arch_dreamcast
    update_status("Initializing network...");

    /* Check if BBA is already active (like ClassiCube does) */
    if (net_default_dev) {
        update_status("Network ready (BBA detected)");
        return 0;  /* BBA already active, we're done */
    }

    /* Try serial coders cable first (faster than modem dial-up) */
    int serial_result = try_serial_coders_cable();
    if (serial_result == 0) {
        return 0;  /* Serial connection successful */
    }
    printf("DC Now: Serial cable not detected, trying modem...\n");

    /* Give system time to settle after serial detection before modem init */
    timer_spin_sleep(500);

    /* No BBA or serial - try modem initialization (DreamPi dial-up) */
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

    /* Check if it's a PPP connection (modem or serial) */
    if (strncmp(net_default_dev->name, "ppp", 3) == 0) {
        printf("DC Now: Shutting down PPP connection...\n");
        ppp_shutdown();

        /* Give PPP time to shut down */
        timer_spin_sleep(200);

        if (serial_connection_active) {
            /* Serial coders cable - no modem hardware to shutdown */
            printf("DC Now: Serial PPP disconnected\n");
            logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
            if (logfile) {
                fprintf(logfile, "Serial PPP disconnected successfully\n");
                fclose(logfile);
            }
            serial_connection_active = 0;
        } else {
            /* Modem connection - shutdown modem hardware */
            printf("DC Now: Shutting down modem hardware...\n");
            modem_shutdown();

            /* Give modem hardware time to reset */
            timer_spin_sleep(500);

            printf("DC Now: Modem and PPP disconnected\n");
            logfile = fopen("/ram/DCNOW_LOG.TXT", "a");
            if (logfile) {
                fprintf(logfile, "PPP and modem disconnected successfully\n");
                fclose(logfile);
            }
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
