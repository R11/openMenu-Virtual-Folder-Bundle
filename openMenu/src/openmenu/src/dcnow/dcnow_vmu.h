#ifndef DCNOW_VMU_H
#define DCNOW_VMU_H

#include "dcnow_api.h"
#include <stdbool.h>

/**
 * Update VMU display with DC Now games list
 * Shows game names and player counts (including idle users) in a scrolling format
 *
 * @param data Pointer to DC Now data to display
 */
void dcnow_vmu_update_display(const dcnow_data_t *data);

/**
 * Restore VMU display to OpenMenu logo (when disconnected from DC Now)
 */
void dcnow_vmu_restore_logo(void);

/**
 * Check if DC Now VMU display is currently active
 *
 * @return true if DC Now display is active, false if showing OpenMenu logo
 */
bool dcnow_vmu_is_active(void);

/**
 * Show a refresh/fetching indicator on the VMU display.
 * Overlays a spinning animation next to the DCNOW title on the current
 * VMU content.  If no game data has been displayed yet, a placeholder
 * screen is rendered first.  Safe to call repeatedly — each call advances
 * the spinner by one frame.
 */
void dcnow_vmu_show_refreshing(void);

#endif /* DCNOW_VMU_H */
