#ifndef DCNOW_NET_INIT_H
#define DCNOW_NET_INIT_H

/**
 * Initialize network for DreamPi or BBA with automatic modem dialing
 *
 * Based on ClassiCube's proven implementation approach.
 *
 * This function:
 * - Checks if BBA is already active (net_default_dev exists)
 * - For BBA: Returns immediately (already initialized)
 * - For DreamPi/modem: Automatically dials and establishes PPP connection using:
 *   1. modem_init() - Initialize modem hardware (FIRST!)
 *   2. ppp_init() - Initialize PPP subsystem
 *   3. ppp_modem_init("555", 1, NULL) - Dial DreamPi (~20 seconds)
 *   4. ppp_set_login("dream", "dreamcast") - Set auth credentials
 *   5. ppp_connect() - Establish connection (~20 seconds)
 *   6. Waits up to 40 seconds for link up
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

#endif /* DCNOW_NET_INIT_H */
