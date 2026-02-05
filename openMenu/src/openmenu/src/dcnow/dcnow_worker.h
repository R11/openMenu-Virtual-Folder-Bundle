#ifndef DCNOW_WORKER_H
#define DCNOW_WORKER_H

#include <stdbool.h>
#include "dcnow_api.h"

/**
 * Worker thread states for non-blocking network operations
 */
typedef enum {
    DCNOW_WORKER_IDLE,          /* No operation in progress */
    DCNOW_WORKER_CONNECTING,    /* PPP/modem connection in progress */
    DCNOW_WORKER_FETCHING,      /* HTTP data fetch in progress */
    DCNOW_WORKER_DONE,          /* Operation completed successfully */
    DCNOW_WORKER_ERROR          /* Operation failed */
} dcnow_worker_state_t;

/**
 * Worker thread context - shared between main and worker threads
 * All volatile fields are accessed from both threads
 */
typedef struct {
    volatile dcnow_worker_state_t state;
    volatile char status_message[128];  /* Current status for UI display */
    dcnow_data_t result_data;           /* Fetch result (only valid when state == DONE) */
    volatile int error_code;            /* Error code if state == ERROR */
    volatile bool cancel_requested;     /* Set by main thread to request cancellation */
} dcnow_worker_context_t;

/**
 * Initialize the worker thread system
 * Must be called once before using any worker functions
 */
void dcnow_worker_init(void);

/**
 * Shutdown the worker thread system
 * Cancels any pending operation and cleans up resources
 */
void dcnow_worker_shutdown(void);

/**
 * Start async network connection in worker thread
 *
 * This initiates PPP/modem connection without blocking the main thread.
 * Poll with dcnow_worker_poll() to check progress.
 *
 * @param ctx - Context structure to receive status updates and results
 * @return 0 on success (worker started), negative on error:
 *         -1: Worker already busy
 *         -2: Thread creation failed
 */
int dcnow_worker_start_connect(dcnow_worker_context_t* ctx);

/**
 * Start async data fetch in worker thread
 *
 * This fetches data from dreamcast.online without blocking the main thread.
 * Network must already be connected (call dcnow_worker_start_connect first).
 * Poll with dcnow_worker_poll() to check progress.
 *
 * @param ctx - Context structure to receive status updates and results
 * @param timeout_ms - HTTP timeout in milliseconds
 * @return 0 on success (worker started), negative on error:
 *         -1: Worker already busy
 *         -2: Thread creation failed
 */
int dcnow_worker_start_fetch(dcnow_worker_context_t* ctx, uint32_t timeout_ms);

/**
 * Poll worker thread status (call from main loop each frame)
 *
 * This is non-blocking and returns immediately.
 *
 * @param ctx - Context structure
 * @return Current worker state
 */
dcnow_worker_state_t dcnow_worker_poll(dcnow_worker_context_t* ctx);

/**
 * Get current status message for UI display
 *
 * @param ctx - Context structure
 * @return Pointer to status message string (valid until next poll)
 */
const char* dcnow_worker_get_status(dcnow_worker_context_t* ctx);

/**
 * Request cancellation of current operation
 *
 * The worker will check this flag and abort when possible.
 * Note: Some blocking operations (like ppp_connect) cannot be interrupted.
 *
 * @param ctx - Context structure
 */
void dcnow_worker_cancel(dcnow_worker_context_t* ctx);

/**
 * Check if worker is currently busy
 *
 * @return true if a worker operation is in progress
 */
bool dcnow_worker_is_busy(void);

#endif /* DCNOW_WORKER_H */
