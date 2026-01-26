#ifndef DCNOW_NET_INIT_H
#define DCNOW_NET_INIT_H

/**
 * Initialize network for DreamPi or BBA with automatic modem dialing
 *
 * This function:
 * - Auto-detects BBA (Broadband Adapter) or modem hardware
 * - For BBA: Configures and returns immediately
 * - For DreamPi/modem: Automatically dials and establishes PPP connection
 *   - Initializes modem hardware
 *   - Dials 555-5555 (DreamPi default number)
 *   - Authenticates with dreamcast/dreamcast credentials
 *   - Waits up to 30 seconds for connection
 *
 * This should be called early in main() before any network operations
 *
 * @return 0 on success, negative on error:
 *         -1: No network hardware detected
 *         -2: PPP detected but not connected (legacy message)
 *         -3: Modem hardware init failed
 *         -4: Modem mode setting failed
 *         -5: PPP init failed
 *         -6: Modem dial failed
 *         -7: PPP connection timeout (30 seconds)
 */
int dcnow_net_early_init(void);

#endif /* DCNOW_NET_INIT_H */
