#ifndef DCNOW_NET_INIT_H
#define DCNOW_NET_INIT_H

/**
 * Status callback for network initialization
 * Called during network init to provide visual feedback
 * @param message - Status message to display (e.g., "Dialing modem...")
 */
typedef void (*dcnow_status_callback_t)(const char* message);

/**
 * Set status callback for visual feedback during network initialization
 * @param callback - Function to call with status updates (NULL to disable)
 */
void dcnow_set_status_callback(dcnow_status_callback_t callback);

/**
 * Initialize network for DreamPi or BBA with automatic connection
 *
 * Based on ClassiCube's proven implementation approach.
 *
 * This function tries connection methods in order:
 * 1. BBA - Checks if Broadband Adapter is already active
 * 2. Serial Coders Cable - Tries serial connection at 115200 baud
 *    - Sends "AT\r\n" and waits for "OK\r\n" (DreamPi 2 detection)
 *    - Sends "ATDT\r\n" dial command
 *    - Waits for "CONNECT 115200\r\n"
 *    - Waits 5 seconds for DreamPi to start pppd
 *    - Establishes PPP over SCIF
 * 3. Modem Dial-up - Falls back to traditional modem connection
 *    - modem_init() - Initialize modem hardware
 *    - ppp_modem_init("111-1111", 1, NULL) - Dial DreamPi
 *    - ppp_set_login("dream", "dreamcast") - Set auth credentials
 *    - ppp_connect() - Establish PPP connection
 *
 * This should be called early in main() before any network operations
 *
 * @return 0 on success, negative on error:
 *         -1: Modem hardware initialization failed
 *         -2: PPP subsystem init failed
 *         -3: ppp_modem_init failed (dial failed)
 *         -4: ppp_set_login failed
 *         -5: ppp_connect failed
 *         -6: PPP connection timeout (40 seconds)
 */
int dcnow_net_early_init(void);

/**
 * Disconnect and reset the network connection (modem/serial/PPP)
 *
 * This function should be called before:
 * - Exiting to BIOS
 * - Launching a game
 * - Launching CodeBreaker
 *
 * It will properly shutdown the PPP connection and modem hardware
 * (if modem was used) with appropriate delays to ensure hardware
 * fully resets. For serial coders cable connections, only PPP is
 * shutdown (no modem hardware involved).
 *
 * After shutdown, net_default_dev is set to NULL so subsequent
 * calls to dcnow_net_early_init() will properly reinitialize.
 *
 * Timing: ~200ms for serial, ~700ms for modem
 */
void dcnow_net_disconnect(void);

#endif /* DCNOW_NET_INIT_H */
