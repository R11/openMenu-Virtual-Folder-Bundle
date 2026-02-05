# Proposals for Non-Blocking Network Operations

## Executive Summary

This document outlines several approaches to make network connections non-blocking in openMenu's DC NOW feature. The current implementation uses a deferred execution pattern (flags + render callbacks) which shows "Connecting..." messages before blocking calls, but the actual blocking still freezes the UI during:

1. **PPP Connection** (`ppp_connect()`) - 20-40 seconds blocking
2. **DNS Resolution** (`gethostbyname()`) - blocking
3. **TCP Connect** (`connect()`) - blocking
4. **HTTP Send/Receive** (`send()`/`recv()`) - blocking with `thd_pass()` yielding

---

## Current Implementation Analysis

### Blocking Call Locations

| File | Line | Function | What Blocks | Duration |
|------|------|----------|-------------|----------|
| `dcnow_net_init.c` | 67 | `modem_init()` | Modem hardware init | ~1s |
| `dcnow_net_init.c` | 85 | `ppp_modem_init()` | Dialing modem | ~5-10s |
| `dcnow_net_init.c` | 101 | `ppp_connect()` | PPP handshake | ~10-20s |
| `dcnow_api.c` | 172 | `gethostbyname()` | DNS resolution | ~1-5s |
| `dcnow_api.c` | 192 | `connect()` | TCP handshake | ~1-3s |
| `dcnow_api.c` | 234 | `recv()` loop | HTTP response | ~1-5s |

### Current "Deferred Execution" Pattern

The current code in `ui_menu_credits.c` uses a flag-based pattern:

```c
// ui_menu_credits.c:2016-2017 - User presses A button
dcnow_is_connecting = true;
dcnow_needs_connect = true;  // Set flag, return immediately

// ui_menu_credits.c:2216-2222 - Next draw call checks flag
if (dcnow_needs_connect && dcnow_shown_connecting) {
    dcnow_needs_connect = false;
    dcnow_set_status_callback(dcnow_connection_status_callback);
    int net_result = dcnow_net_early_init();  // <-- BLOCKS HERE for 20-40 seconds
    // ...
}
```

**Limitation**: This pattern ensures the "Connecting..." message is shown BEFORE blocking, but the UI still **freezes completely** during the blocking calls. The status callback (`dcnow_connection_status_callback`) renders frames during the block, but:
- Input is not processed
- Animations don't update smoothly
- User cannot cancel the operation

### What "Non-Blocking" Would Achieve

| Aspect | Current | With Non-Blocking |
|--------|---------|-------------------|
| UI during connect | Frozen (status callback renders occasionally) | Fully responsive, 60fps |
| Cancel operation | Not possible | User can press B to cancel |
| Progress display | Status messages only | Animated spinner, percentage |
| Auto-refresh | Blocks UI for ~3s | Background, user unaware |

---

## Proposal 1: Worker Thread Approach

**Concept**: Use KOS's `thd_create()` to run network operations in a dedicated worker thread while the main thread continues rendering.

### Architecture

```
┌─────────────────┐         ┌─────────────────┐
│   Main Thread   │         │  Worker Thread  │
│  (UI Rendering) │         │  (Network I/O)  │
├─────────────────┤         ├─────────────────┤
│ - Render loop   │◄───────►│ - PPP connect   │
│ - Input handling│  flags  │ - DNS resolve   │
│ - Animation     │ /mutex  │ - HTTP request  │
│ - thd_pass()    │         │ - JSON parse    │
└─────────────────┘         └─────────────────┘
```

### Implementation

**New file: `dcnow_worker.h`**
```c
#ifndef DCNOW_WORKER_H
#define DCNOW_WORKER_H

#include <stdbool.h>
#include "dcnow_api.h"

typedef enum {
    DCNOW_WORKER_IDLE,
    DCNOW_WORKER_CONNECTING,
    DCNOW_WORKER_FETCHING,
    DCNOW_WORKER_DONE,
    DCNOW_WORKER_ERROR
} dcnow_worker_state_t;

typedef struct {
    dcnow_worker_state_t state;
    char status_message[128];
    dcnow_data_t result_data;
    int error_code;
    bool cancel_requested;
} dcnow_worker_context_t;

/* Start async connection in worker thread */
int dcnow_worker_start_connect(dcnow_worker_context_t* ctx);

/* Start async data fetch in worker thread */
int dcnow_worker_start_fetch(dcnow_worker_context_t* ctx);

/* Poll worker status (call from main loop) */
dcnow_worker_state_t dcnow_worker_poll(dcnow_worker_context_t* ctx);

/* Get current status message for UI display */
const char* dcnow_worker_get_status(dcnow_worker_context_t* ctx);

/* Request cancellation of current operation */
void dcnow_worker_cancel(dcnow_worker_context_t* ctx);

#endif
```

**New file: `dcnow_worker.c`**
```c
#include "dcnow_worker.h"
#include "dcnow_net_init.h"
#include <kos/thread.h>
#include <kos/mutex.h>

static kthread_t* worker_thread = NULL;
static mutex_t worker_mutex = MUTEX_INITIALIZER;
static volatile dcnow_worker_context_t* active_ctx = NULL;

/* Status callback that updates shared context */
static void worker_status_callback(const char* message) {
    mutex_lock(&worker_mutex);
    if (active_ctx) {
        strncpy((char*)active_ctx->status_message, message, 127);
    }
    mutex_unlock(&worker_mutex);
}

/* Worker thread entry point for connection */
static void* connect_worker_func(void* arg) {
    dcnow_worker_context_t* ctx = (dcnow_worker_context_t*)arg;

    mutex_lock(&worker_mutex);
    ctx->state = DCNOW_WORKER_CONNECTING;
    active_ctx = ctx;
    mutex_unlock(&worker_mutex);

    dcnow_set_status_callback(worker_status_callback);
    int result = dcnow_net_early_init();
    dcnow_set_status_callback(NULL);

    mutex_lock(&worker_mutex);
    if (result < 0) {
        ctx->state = DCNOW_WORKER_ERROR;
        ctx->error_code = result;
    } else {
        ctx->state = DCNOW_WORKER_DONE;
        ctx->error_code = 0;
    }
    active_ctx = NULL;
    mutex_unlock(&worker_mutex);

    return NULL;
}

/* Worker thread entry point for fetch */
static void* fetch_worker_func(void* arg) {
    dcnow_worker_context_t* ctx = (dcnow_worker_context_t*)arg;

    mutex_lock(&worker_mutex);
    ctx->state = DCNOW_WORKER_FETCHING;
    strncpy((char*)ctx->status_message, "Fetching data...", 127);
    mutex_unlock(&worker_mutex);

    int result = dcnow_fetch_data(&ctx->result_data, 10000);

    mutex_lock(&worker_mutex);
    if (result < 0) {
        ctx->state = DCNOW_WORKER_ERROR;
        ctx->error_code = result;
    } else {
        ctx->state = DCNOW_WORKER_DONE;
        ctx->error_code = 0;
    }
    mutex_unlock(&worker_mutex);

    return NULL;
}

int dcnow_worker_start_connect(dcnow_worker_context_t* ctx) {
    if (worker_thread != NULL) {
        return -1;  /* Already busy */
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->state = DCNOW_WORKER_CONNECTING;

    worker_thread = thd_create(0, connect_worker_func, ctx);
    if (!worker_thread) {
        return -2;
    }

    thd_detach(worker_thread);
    return 0;
}

int dcnow_worker_start_fetch(dcnow_worker_context_t* ctx) {
    if (worker_thread != NULL) {
        return -1;
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->state = DCNOW_WORKER_FETCHING;

    worker_thread = thd_create(0, fetch_worker_func, ctx);
    if (!worker_thread) {
        return -2;
    }

    thd_detach(worker_thread);
    return 0;
}

dcnow_worker_state_t dcnow_worker_poll(dcnow_worker_context_t* ctx) {
    dcnow_worker_state_t state;

    mutex_lock(&worker_mutex);
    state = ctx->state;
    if (state == DCNOW_WORKER_DONE || state == DCNOW_WORKER_ERROR) {
        worker_thread = NULL;  /* Thread finished, allow new operations */
    }
    mutex_unlock(&worker_mutex);

    return state;
}

const char* dcnow_worker_get_status(dcnow_worker_context_t* ctx) {
    return ctx->status_message;
}

void dcnow_worker_cancel(dcnow_worker_context_t* ctx) {
    mutex_lock(&worker_mutex);
    ctx->cancel_requested = true;
    mutex_unlock(&worker_mutex);
}
```

### UI Integration Changes (`ui_menu_credits.c`)

```c
/* Replace blocking deferred execution with worker polling */
static dcnow_worker_context_t worker_ctx;

void draw_dcnow_tr(void) {
    /* Poll worker thread status */
    if (dcnow_is_connecting) {
        dcnow_worker_state_t state = dcnow_worker_poll(&worker_ctx);
        strncpy(connection_status, dcnow_worker_get_status(&worker_ctx), 127);

        if (state == DCNOW_WORKER_DONE) {
            dcnow_is_connecting = false;
            dcnow_net_initialized = true;
            /* Auto-trigger fetch */
            dcnow_worker_start_fetch(&worker_ctx);
            dcnow_is_loading = true;
        } else if (state == DCNOW_WORKER_ERROR) {
            dcnow_is_connecting = false;
            snprintf(dcnow_data.error_message, sizeof(dcnow_data.error_message),
                    "Connection failed (error %d)", worker_ctx.error_code);
        }
        /* else: still connecting, UI keeps animating */
    }

    if (dcnow_is_loading) {
        dcnow_worker_state_t state = dcnow_worker_poll(&worker_ctx);

        if (state == DCNOW_WORKER_DONE) {
            dcnow_is_loading = false;
            memcpy(&dcnow_data, &worker_ctx.result_data, sizeof(dcnow_data));
            dcnow_data_fetched = true;
        } else if (state == DCNOW_WORKER_ERROR) {
            dcnow_is_loading = false;
            dcnow_data.data_valid = false;
        }
    }

    /* Continue with normal popup rendering... */
}
```

### Pros
- UI remains fully responsive during all network operations
- Clean separation of concerns
- Relatively simple to implement with KOS's threading support
- Can add progress animations (dots, spinner, etc.)

### Cons
- Thread synchronization overhead (mutexes)
- Potential stack size concerns on Dreamcast (limited RAM)
- Must be careful about callback usage across threads
- Current PPP status callback renders directly to PVR - needs refactoring

### Complexity: Medium

---

## Proposal 2: Non-Blocking Sockets with State Machine

**Concept**: Convert socket operations to non-blocking mode using `fcntl(O_NONBLOCK)` and implement a state machine that can be polled each frame.

### Prerequisites (Need Verification)

KOS *may* support these POSIX APIs - needs testing:
```c
#include <fcntl.h>
#include <sys/select.h>

/* Set non-blocking */
int flags = fcntl(sock, F_GETFL, 0);
fcntl(sock, F_SETFL, flags | O_NONBLOCK);

/* Check readiness with select() */
fd_set read_fds, write_fds;
struct timeval tv = {0, 0};  /* Non-blocking poll */
select(sock + 1, &read_fds, &write_fds, NULL, &tv);
```

### State Machine Design

```c
typedef enum {
    HTTP_STATE_IDLE,
    HTTP_STATE_RESOLVING_DNS,
    HTTP_STATE_CONNECTING,
    HTTP_STATE_SENDING,
    HTTP_STATE_RECEIVING_HEADERS,
    HTTP_STATE_RECEIVING_BODY,
    HTTP_STATE_COMPLETE,
    HTTP_STATE_ERROR
} http_state_t;

typedef struct {
    http_state_t state;
    int socket;
    char* hostname;
    char* path;
    char* response_buffer;
    int response_len;
    int expected_len;
    uint64_t timeout_start;
    uint32_t timeout_ms;
    char error_message[128];

    /* Partial data for non-blocking operations */
    int send_offset;
    char request_buffer[512];
    int request_len;
} http_request_t;

/* Call once per frame - returns true when complete or error */
bool http_request_tick(http_request_t* req);
```

### Implementation

**New file: `dcnow_async_http.c`**
```c
#include "dcnow_async_http.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netdb.h>

/* Note: DNS resolution (gethostbyname) is typically blocking
   For true async DNS, would need a thread or custom resolver */

bool http_request_tick(http_request_t* req) {
    fd_set read_fds, write_fds;
    struct timeval tv = {0, 0};  /* Non-blocking poll */

    /* Check timeout */
    if (timer_ms_gettime64() - req->timeout_start > req->timeout_ms) {
        req->state = HTTP_STATE_ERROR;
        strcpy(req->error_message, "Timeout");
        if (req->socket >= 0) close(req->socket);
        return true;
    }

    switch (req->state) {
        case HTTP_STATE_IDLE:
            /* Start DNS resolution - unfortunately blocking */
            /* See Proposal 3 for thread-based DNS */
            {
                struct hostent* host = gethostbyname(req->hostname);
                if (!host) {
                    req->state = HTTP_STATE_ERROR;
                    strcpy(req->error_message, "DNS failed");
                    return true;
                }

                req->socket = socket(AF_INET, SOCK_STREAM, 0);
                if (req->socket < 0) {
                    req->state = HTTP_STATE_ERROR;
                    strcpy(req->error_message, "Socket failed");
                    return true;
                }

                /* Set non-blocking */
                int flags = fcntl(req->socket, F_GETFL, 0);
                fcntl(req->socket, F_SETFL, flags | O_NONBLOCK);

                /* Start connect */
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(80);
                memcpy(&addr.sin_addr, host->h_addr, host->h_length);

                int ret = connect(req->socket, (struct sockaddr*)&addr, sizeof(addr));
                if (ret < 0 && errno != EINPROGRESS) {
                    req->state = HTTP_STATE_ERROR;
                    snprintf(req->error_message, 128, "Connect failed: %d", errno);
                    close(req->socket);
                    return true;
                }

                req->state = HTTP_STATE_CONNECTING;
            }
            break;

        case HTTP_STATE_CONNECTING:
            /* Poll for connection completion */
            FD_ZERO(&write_fds);
            FD_SET(req->socket, &write_fds);

            if (select(req->socket + 1, NULL, &write_fds, NULL, &tv) > 0) {
                /* Check if connect succeeded */
                int error = 0;
                socklen_t len = sizeof(error);
                getsockopt(req->socket, SOL_SOCKET, SO_ERROR, &error, &len);

                if (error == 0) {
                    /* Connected! Build and start sending request */
                    req->request_len = snprintf(req->request_buffer, 512,
                        "GET %s HTTP/1.1\r\n"
                        "Host: %s\r\n"
                        "User-Agent: openMenu/1.1\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        req->path, req->hostname);
                    req->send_offset = 0;
                    req->state = HTTP_STATE_SENDING;
                } else {
                    req->state = HTTP_STATE_ERROR;
                    snprintf(req->error_message, 128, "Connect error: %d", error);
                    close(req->socket);
                    return true;
                }
            }
            break;

        case HTTP_STATE_SENDING:
            /* Poll for write readiness */
            FD_ZERO(&write_fds);
            FD_SET(req->socket, &write_fds);

            if (select(req->socket + 1, NULL, &write_fds, NULL, &tv) > 0) {
                int remaining = req->request_len - req->send_offset;
                int sent = send(req->socket,
                               req->request_buffer + req->send_offset,
                               remaining, 0);

                if (sent > 0) {
                    req->send_offset += sent;
                    if (req->send_offset >= req->request_len) {
                        req->state = HTTP_STATE_RECEIVING_HEADERS;
                        req->response_len = 0;
                    }
                } else if (sent < 0 && errno != EWOULDBLOCK) {
                    req->state = HTTP_STATE_ERROR;
                    strcpy(req->error_message, "Send failed");
                    close(req->socket);
                    return true;
                }
            }
            break;

        case HTTP_STATE_RECEIVING_HEADERS:
        case HTTP_STATE_RECEIVING_BODY:
            /* Poll for read readiness */
            FD_ZERO(&read_fds);
            FD_SET(req->socket, &read_fds);

            if (select(req->socket + 1, &read_fds, NULL, NULL, &tv) > 0) {
                int received = recv(req->socket,
                                   req->response_buffer + req->response_len,
                                   8192 - req->response_len - 1, 0);

                if (received > 0) {
                    req->response_len += received;
                    req->response_buffer[req->response_len] = '\0';
                    req->timeout_start = timer_ms_gettime64();  /* Reset timeout */

                    /* Check for end of headers / content-length */
                    if (req->state == HTTP_STATE_RECEIVING_HEADERS) {
                        if (strstr(req->response_buffer, "\r\n\r\n")) {
                            req->state = HTTP_STATE_RECEIVING_BODY;
                        }
                    }
                } else if (received == 0) {
                    /* Connection closed - we're done */
                    req->state = HTTP_STATE_COMPLETE;
                    close(req->socket);
                    return true;
                } else if (errno != EWOULDBLOCK) {
                    req->state = HTTP_STATE_ERROR;
                    strcpy(req->error_message, "Recv failed");
                    close(req->socket);
                    return true;
                }
            }
            break;

        case HTTP_STATE_COMPLETE:
        case HTTP_STATE_ERROR:
            return true;  /* Already finished */
    }

    return false;  /* Still in progress */
}
```

### Pros
- Fine-grained control over each network operation
- No thread synchronization needed
- Can show precise progress (connecting, sending, receiving X%)
- Lower memory overhead than threads

### Cons
- `gethostbyname()` is still blocking (needs thread or custom DNS resolver)
- More complex implementation
- Requires KOS to support `fcntl()`, `select()` (needs verification)
- **Cannot help with PPP modem connection** - that's hardware-level and always blocking

### Complexity: Medium-High

---

## Proposal 3: Hybrid Approach (Recommended)

**Concept**: Combine worker thread for PPP/DNS (inherently blocking operations) with non-blocking sockets for HTTP I/O. This leverages the best of both approaches.

### Architecture

```
┌─────────────────────────────────────────────────────────┐
│                      Main Thread                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐  │
│  │ UI Render   │  │ Input       │  │ HTTP State      │  │
│  │ Loop        │  │ Handler     │  │ Machine Tick    │  │
│  └─────────────┘  └─────────────┘  └─────────────────┘  │
│         ▲                                    ▲          │
│         │ polls                              │ polls    │
│         ▼                                    ▼          │
│  ┌─────────────────────┐          ┌─────────────────┐  │
│  │  Worker Thread      │          │ Non-blocking    │  │
│  │  (PPP + DNS only)   │          │ Socket Ops      │  │
│  └─────────────────────┘          └─────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### Phase 1: Worker Thread (PPP Connection + DNS)
```c
typedef struct {
    volatile int state;        /* 0=idle, 1=connecting, 2=resolving, 3=done, <0=error */
    volatile char status[128];
    volatile uint32_t resolved_ip;  /* DNS result */
    char hostname[128];
} dcnow_net_worker_t;

static void* net_worker_func(void* arg) {
    dcnow_net_worker_t* w = (dcnow_net_worker_t*)arg;

    /* Phase 1: PPP Connection */
    w->state = 1;
    dcnow_set_status_callback(worker_status_cb);
    int result = dcnow_net_early_init();
    dcnow_set_status_callback(NULL);

    if (result < 0) {
        w->state = -1;
        return NULL;
    }

    /* Phase 2: DNS Resolution */
    w->state = 2;
    strcpy((char*)w->status, "Resolving hostname...");

    struct hostent* host = gethostbyname(w->hostname);
    if (!host) {
        w->state = -2;
        return NULL;
    }

    w->resolved_ip = *(uint32_t*)host->h_addr;
    w->state = 3;  /* Done */
    return NULL;
}
```

### Phase 2: Non-blocking HTTP (After worker completes)
```c
/* Once worker is done, we have IP - use non-blocking socket from main thread */
int dcnow_async_http_start(uint32_t ip, const char* path) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    /* Set non-blocking */
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    /* Start non-blocking connect */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = ip;

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    /* Will return -1 with errno=EINPROGRESS, that's expected */

    return sock;  /* Main loop will poll with select() */
}
```

### Pros
- Best of both worlds: threading for truly blocking ops, state machine for I/O
- UI stays responsive throughout
- PPP status callbacks work naturally in worker thread
- Lower complexity than pure non-blocking approach
- HTTP I/O gets fine-grained progress tracking

### Cons
- Still requires thread for initial connection
- More components to manage
- Requires KOS to support non-blocking sockets (needs verification)

### Complexity: Medium

---

## Proposal 4: Promise-like Wrapper (Promisification)

**Concept**: Create a "promise-like" abstraction layer that wraps async operations, similar to JavaScript Promises but using C callbacks.

### API Design

```c
/* Promise-like types */
typedef struct dcnow_promise dcnow_promise_t;

typedef void (*dcnow_resolve_fn)(void* result, void* user_data);
typedef void (*dcnow_reject_fn)(int error_code, const char* message, void* user_data);

/* Create a promise for network connection */
dcnow_promise_t* dcnow_connect_async(void);

/* Create a promise for data fetch */
dcnow_promise_t* dcnow_fetch_async(uint32_t timeout_ms);

/* Chain handlers (like .then() and .catch()) */
void dcnow_promise_then(dcnow_promise_t* p, dcnow_resolve_fn on_resolve, void* user_data);
void dcnow_promise_catch(dcnow_promise_t* p, dcnow_reject_fn on_reject, void* user_data);

/* Must be called each frame to drive promises */
void dcnow_promises_tick(void);

/* Cancel a pending promise */
void dcnow_promise_cancel(dcnow_promise_t* p);
```

### Usage Example

```c
/* In handle_input_dcnow when A is pressed */
if (!dcnow_net_initialized) {
    dcnow_promise_t* connect_promise = dcnow_connect_async();

    dcnow_promise_then(connect_promise, on_connected, NULL);
    dcnow_promise_catch(connect_promise, on_connect_failed, NULL);

    dcnow_is_connecting = true;
}

static void on_connected(void* result, void* user_data) {
    dcnow_net_initialized = true;
    dcnow_is_connecting = false;

    /* Chain: fetch data after connected */
    dcnow_promise_t* fetch_promise = dcnow_fetch_async(5000);
    dcnow_promise_then(fetch_promise, on_data_received, NULL);
    dcnow_promise_catch(fetch_promise, on_fetch_failed, NULL);

    dcnow_is_loading = true;
}

static void on_data_received(void* result, void* user_data) {
    dcnow_data_t* data = (dcnow_data_t*)result;
    memcpy(&dcnow_data, data, sizeof(dcnow_data));
    dcnow_data_fetched = true;
    dcnow_is_loading = false;
}
```

### Internal Implementation

The promise system would internally use either:
- Worker threads (Proposal 1)
- Non-blocking sockets + state machine (Proposal 2)
- Hybrid approach (Proposal 3)

### Pros
- Clean, modern API that's easy to reason about
- Chaining operations is natural
- Hides complexity of threading/non-blocking behind abstraction
- Easy to add new async operations later

### Cons
- Adds abstraction layer overhead
- Still needs underlying async mechanism (thread or non-blocking)
- Memory management for promises needs care
- Unfamiliar pattern for embedded C developers

### Complexity: Medium-High

---

## Proposal 5: Enhanced Cooperative Multitasking

**Concept**: Improve the current approach by breaking blocking operations into smaller chunks with more frequent `thd_pass()` yields.

### Modifications to `dcnow_net_init.c`

```c
/* Modified status callback that yields after each status update */
static void update_status_yielding(const char* message) {
    printf("DC Now STATUS: %s\n", message);

    if (status_callback) {
        status_callback(message);

        /* Yield to allow other threads/main loop to run */
        for (int i = 0; i < 5; i++) {
            thd_pass();
        }
    }
}

int dcnow_net_early_init(void) {
    /* More frequent status updates = more yield points */
    update_status_yielding("Initializing network...");
    thd_pass();

    if (net_default_dev) {
        update_status_yielding("BBA detected");
        return 0;
    }

    update_status_yielding("Initializing modem...");
    thd_pass();

    if (!modem_init()) {
        return -1;
    }
    thd_pass();

    update_status_yielding("Setting modem speed...");
    modem_set_mode(0, 0x86);
    thd_pass();

    /* etc... with thd_pass() after each step */
}
```

### Modifications to `dcnow_api.c` recv loop

```c
while (total_received < buf_size - 1) {
    /* Yield more frequently */
    thd_pass();

    /* Use smaller receive chunks for more yield opportunities */
    int chunk_size = 512;  /* Instead of full buffer */
    int received = recv(sock, response_buf + total_received,
                       chunk_size, 0);

    if (received > 0) {
        total_received += received;
        start_time = timer_ms_gettime64();

        /* Yield after each successful receive */
        thd_pass();
    }
    /* ... */
}
```

### Pros
- Minimal changes to existing code
- No new dependencies (threads, select, etc.)
- Low risk of introducing bugs
- Already partially implemented

### Cons
- **Still blocks the main thread** - thd_pass() only helps if there are other threads
- PPP connection is fundamentally blocking at hardware level
- UI still freezes, just with status updates
- Not a true solution, more of an incremental improvement

### Complexity: Low

---

## Feasibility Analysis: Lower-Level Network Hooks

### Question: Can we hook into lower-level networking calls?

**Analysis of KOS Network Stack:**

1. **PPP Layer** (`ppp_connect()`, `ppp_modem_init()`)
   - These are high-level KOS functions that wrap modem hardware
   - The modem itself requires synchronous handshaking (AT commands, carrier detection)
   - **Cannot be made non-blocking** without rewriting KOS PPP stack

2. **Socket Layer** (`socket()`, `connect()`, `send()`, `recv()`)
   - KOS implements BSD-style sockets
   - **May support** `fcntl(O_NONBLOCK)` and `select()` (needs verification)
   - If supported, can be promisified with state machine approach

3. **DNS Layer** (`gethostbyname()`)
   - Standard blocking POSIX call
   - **Cannot be made non-blocking** without custom async DNS resolver
   - Best wrapped in worker thread

### Verification Needed

To confirm KOS capabilities, test this code:
```c
#include <fcntl.h>
#include <sys/select.h>

void test_nonblocking_support(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    /* Test fcntl */
    int flags = fcntl(sock, F_GETFL, 0);
    printf("fcntl F_GETFL: %s\n", flags >= 0 ? "SUPPORTED" : "NOT SUPPORTED");

    int ret = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    printf("fcntl O_NONBLOCK: %s\n", ret >= 0 ? "SUPPORTED" : "NOT SUPPORTED");

    /* Test select with zero timeout (poll) */
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    ret = select(sock + 1, &fds, NULL, NULL, &tv);
    printf("select: %s\n", ret >= 0 ? "SUPPORTED" : "NOT SUPPORTED");

    close(sock);
}
```

---

## Recommendation

**Short-term (Quick Win)**: **Proposal 1 (Worker Thread)**
- Easiest to implement with known KOS APIs
- Guaranteed to work with `thd_create()`
- Provides immediate UI responsiveness
- Can be implemented incrementally

**Medium-term (Polish)**: **Proposal 3 (Hybrid)**
- After verifying non-blocking socket support
- Combines worker thread for PPP with state machine for HTTP
- Gives finest-grained progress updates

**Long-term (If needed)**: **Proposal 4 (Promise-like)**
- Only if codebase grows more async operations
- Provides clean abstraction for future features

---

## Implementation Priority

1. **First**: Verify KOS supports `fcntl(O_NONBLOCK)` and `select()`
2. **Second**: Implement worker thread wrapper (Proposal 1)
3. **Third**: Test on real Dreamcast hardware
4. **Fourth**: If non-blocking sockets work, implement hybrid (Proposal 3)
5. **Fifth**: Add promise-like wrapper if API becomes unwieldy

---

## Files to Create/Modify

| File | Action | Purpose |
|------|--------|---------|
| `dcnow_worker.h` | Create | Worker thread API |
| `dcnow_worker.c` | Create | Worker thread implementation |
| `dcnow_async_http.h` | Create | Non-blocking HTTP API |
| `dcnow_async_http.c` | Create | Non-blocking HTTP state machine |
| `ui_menu_credits.c` | Modify | Use async APIs instead of deferred blocking |
| `dcnow_net_init.c` | Modify | Status callback thread-safety |
| `CMakeLists.txt` | Modify | Add new source files |

---

## References

- [KallistiOS Documentation](https://kos-docs.dreamcast.wiki/)
- [KallistiOS GitHub Repository](https://github.com/KallistiOS/KallistiOS)
- [KOS Thread API](http://gamedev.allusion.net/docs/kos-current/thread_8h.html)
- [Non-blocking Sockets Tutorial](https://www.scottklement.com/rpg/socktut/nonblocking.html)
- [Setting Socket Non-blocking](https://jameshfisher.com/2017/04/05/set_socket_nonblocking/)
