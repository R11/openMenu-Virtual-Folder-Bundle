#ifdef _arch_dreamcast
#include <crayon_savefile/peripheral.h>
#include <dc/maple/vmu.h>
#include <kos/thread.h>
#endif

#include <stdbool.h>
#include <crayon_savefile/savefile.h>

#include "openmenu_savefile.h"
#include "openmenu_settings.h"

/* Images and such */
#if __has_include("openmenu_lcd.h") && __has_include("openmenu_pal.h") && __has_include("openmenu_vmu.h")
#include "openmenu_lcd.h"
#include "openmenu_lcd_save_ok.h"
#include "openmenu_pal.h"
#include "openmenu_vmu.h"

#define OPENMENU_ICON         (openmenu_icon)
#define OPENMENU_LCD          (openmenu_lcd)
#define OPENMENU_LCD_SAVE_OK  (openmenu_lcd_save_ok)
#define OPENMENU_PAL          (openmenu_pal)
#define OPENMENU_ICONS        (1)
#else
#define OPENMENU_ICON         (NULL)
#define OPENMENU_LCD          (NULL)
#define OPENMENU_LCD_SAVE_OK  (NULL)
#define OPENMENU_PAL          (NULL)
#define OPENMENU_ICONS        (0)
#endif

static crayon_savefile_details_t savefile_details;
static bool savefile_was_migrated = false;
static int8_t startup_device_id = -1;  /* Device we loaded settings from at startup */
#ifdef _arch_dreamcast
static uint8_t vmu_screens_bitmap = 0;
#endif

void
savefile_defaults() {
    sf_region[0] = REGION_NTSC_U;
    sf_aspect[0] = ASPECT_NORMAL;
    sf_ui[0] = UI_FOLDERS;
    sf_sort[0] = SORT_DEFAULT;
    sf_filter[0] = FILTER_ALL;
    sf_beep[0] = BEEP_ON;
    sf_multidisc[0] = MULTIDISC_SHOW;
    sf_multidisc_grouping[0] = MULTIDISC_GROUPING_ANYWHERE;
    sf_custom_theme[0] = THEME_OFF;
    sf_custom_theme_num[0] = THEME_0;
    sf_bios_3d[0] = BIOS_3D_OFF;
    sf_scroll_art[0] = SCROLL_ART_ON;
    sf_scroll_index[0] = SCROLL_INDEX_ON;
    sf_folders_art[0] = FOLDERS_ART_ON;
    sf_marquee_speed[0] = MARQUEE_SPEED_MEDIUM;
    sf_disc_details[0] = DISC_DETAILS_SHOW;
    sf_folders_item_details[0] = FOLDERS_ITEM_DETAILS_ON;
    sf_clock[0] = CLOCK_12HOUR;
    sf_vm2_send_all[0] = VM2_SEND_ALL;
    sf_boot_mode[0] = BOOT_MODE_FULL;
    sf_dcnow_vmu[0] = DCNOW_VMU_ON;
}

//THIS IS USED BY THE CRAYON SAVEFILE DESERIALISER WHEN LOADING A SAVE FROM AN OLDER VERSION
//THERE IS NO NEED TO CALL THIS MANUALLY
int8_t
update_savefile(void** loaded_variables, crayon_savefile_version_t loaded_version,
                crayon_savefile_version_t latest_version) {
    /* Track if any migration occurred */
    if (loaded_version < latest_version) {
        savefile_was_migrated = true;
    }

    if (loaded_version < SFV_BIOS_3D) {
        sf_bios_3d[0] = BIOS_3D_OFF;
    }
    if (loaded_version < SFV_SCROLL_ART) {
        sf_scroll_art[0] = SCROLL_ART_ON;
    }
    if (loaded_version < SFV_SCROLL_INDEX) {
        sf_scroll_index[0] = SCROLL_INDEX_ON;
    }
    if (loaded_version < SFV_FOLDERS_ART) {
        sf_folders_art[0] = FOLDERS_ART_ON;
    }
    if (loaded_version < SFV_MARQUEE_SPEED) {
        sf_marquee_speed[0] = MARQUEE_SPEED_MEDIUM;
    }
    if (loaded_version < SFV_DISC_DETAILS) {
        sf_disc_details[0] = DISC_DETAILS_SHOW;
    }
    if (loaded_version < SFV_FOLDERS_ITEM_DETAILS) {
        sf_folders_item_details[0] = FOLDERS_ITEM_DETAILS_ON;
    }
    if (loaded_version < SFV_CLOCK) {
        sf_clock[0] = CLOCK_12HOUR;
    }
    if (loaded_version < SFV_MULTIDISC_GROUPING) {
        sf_multidisc_grouping[0] = MULTIDISC_GROUPING_ANYWHERE;
    }
    if (loaded_version < SFV_VM2_SEND_ALL) {
        sf_vm2_send_all[0] = VM2_SEND_ALL;
    }
    if (loaded_version < SFV_BOOT_MODE) {
        sf_boot_mode[0] = BOOT_MODE_FULL;
    }
    if (loaded_version < SFV_DCNOW_VMU) {
        sf_dcnow_vmu[0] = DCNOW_VMU_ON;
    }
    return 0;
}

uint8_t
setup_savefile(crayon_savefile_details_t* details) {
    uint8_t error;

#if defined(_arch_pc)
    crayon_savefile_set_base_path("saves/");
#else
    crayon_savefile_set_base_path(NULL); //Dreamcast ignores the parameter anyways
    // (Assumes "/vmu/") so it's still fine to do the method above for all platforms
#endif
    error =
        crayon_savefile_init_savefile_details(details, "OPENMENU.SYS", SFV_CURRENT, savefile_defaults, update_savefile);

    error += crayon_savefile_set_app_id(details, "openMenu");
    error += crayon_savefile_set_short_desc(details, "openMenu Config");
    error += crayon_savefile_set_long_desc(details, "openMenu Preferences");

    if (error) {
        return 1;
    }

#if defined(_arch_dreamcast) && OPENMENU_ICONS
    vmu_screens_bitmap = crayon_peripheral_dreamcast_get_screens();

    crayon_peripheral_vmu_display_icon(vmu_screens_bitmap, OPENMENU_LCD);

    savefile_details.icon_anim_count = OPENMENU_ICONS;
    savefile_details.icon_anim_speed = 1;
    savefile_details.icon_data = OPENMENU_ICON;
    savefile_details.icon_palette = (unsigned short*)OPENMENU_PAL;
#endif

    crayon_savefile_add_variable(details, &sf_region, sf_region_type, sf_region_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_aspect, sf_aspect_type, sf_aspect_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_ui, sf_ui_type, sf_ui_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_sort, sf_sort_type, sf_sort_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_filter, sf_filter_type, sf_filter_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_beep, sf_beep_type, sf_beep_length, SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_multidisc, sf_multidisc_type, sf_multidisc_length, SFV_INITIAL,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_custom_theme, sf_custom_theme_type, sf_custom_theme_length, SFV_INITIAL,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_custom_theme_num, sf_custom_theme_num_type, sf_custom_theme_num_length,
                                 SFV_INITIAL, VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_bios_3d, sf_bios_3d_type, sf_bios_3d_length, SFV_BIOS_3D,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_scroll_art, sf_scroll_art_type, sf_scroll_art_length, SFV_SCROLL_ART,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_scroll_index, sf_scroll_index_type, sf_scroll_index_length, SFV_SCROLL_INDEX,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_folders_art, sf_folders_art_type, sf_folders_art_length, SFV_FOLDERS_ART,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_marquee_speed, sf_marquee_speed_type, sf_marquee_speed_length, SFV_MARQUEE_SPEED,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_disc_details, sf_disc_details_type, sf_disc_details_length, SFV_DISC_DETAILS,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_folders_item_details, sf_folders_item_details_type, sf_folders_item_details_length, SFV_FOLDERS_ITEM_DETAILS,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_clock, sf_clock_type, sf_clock_length, SFV_CLOCK,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_multidisc_grouping, sf_multidisc_grouping_type, sf_multidisc_grouping_length, SFV_MULTIDISC_GROUPING,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_vm2_send_all, sf_vm2_send_all_type, sf_vm2_send_all_length, SFV_VM2_SEND_ALL,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_boot_mode, sf_boot_mode_type, sf_boot_mode_length, SFV_BOOT_MODE,
                                 VAR_STILL_PRESENT);
    crayon_savefile_add_variable(details, &sf_dcnow_vmu, sf_dcnow_vmu_type, sf_dcnow_vmu_length, SFV_DCNOW_VMU,
                                 VAR_STILL_PRESENT);

    if (crayon_savefile_solidify(details)) {
        return 1;
    }

    return 0;
}

int8_t
find_first_valid_savefile_device(crayon_savefile_details_t* details) {
    int8_t err = -1;
    for (int8_t i = 0; i < CRAYON_SF_NUM_SAVE_DEVICES; ++i) {
        err = crayon_savefile_set_device(details, i);
        if (!err) {
            break;
        }
    }
    return err;
}

void
savefile_init() {
    uint8_t setup_res = setup_savefile(&savefile_details);
    int8_t device_res = find_first_valid_savefile_device(&savefile_details);

    if (!setup_res && !device_res) {
        savefile_was_migrated = false;
        crayon_savefile_load_savedata(&savefile_details);
        settings_sanitize();

        /* Remember which device we loaded from at startup */
        startup_device_id = savefile_details.save_device_id;

        /* Only auto-save if migration from older version occurred */
        if (savefile_was_migrated) {
            crayon_savefile_save_savedata(&savefile_details);
            savefile_was_migrated = false;
        }
    }
}

void
savefile_close() {
    crayon_savefile_free_details(&savefile_details);
    crayon_savefile_free_base_path();
}

int8_t
vmu_beep(int8_t save_device_id, uint32_t beep) {
    if (sf_beep[0] != BEEP_ON) {
        return 0;
    }

#ifdef _arch_dreamcast
    maple_device_t* vmu;

    vec2_s8_t port_and_slot = crayon_peripheral_dreamcast_get_port_and_slot(save_device_id);

    // Invalid controller/port
    if (port_and_slot.x < 0) {
        return -1;
    }

    // Make sure there's a device in the port/slot
    if (!((vmu = maple_enum_dev(port_and_slot.x, port_and_slot.y)))) {
        return -1;
    }

    // Check the device is valid and it has a certain function
    if (!vmu->valid) {
        return -1;
    }

    vmu_beep_raw(vmu, beep);
#endif

    return 0;
}

#if defined(_arch_dreamcast) && OPENMENU_ICONS
/* Thread function to restore VMU icon after delay */
static void*
vmu_icon_restore_thread(void* param) {
    (void)param;
    thd_sleep(2000);  /* 2 seconds */
    crayon_peripheral_vmu_display_icon(vmu_screens_bitmap, OPENMENU_LCD);
    return NULL;
}
#endif

int8_t
savefile_save() {
    settings_sanitize();
    vmu_beep(savefile_details.save_device_id, 0x000065f0); // Turn on beep (if enabled)
    int8_t result = crayon_savefile_save_savedata(&savefile_details);
    vmu_beep(savefile_details.save_device_id, 0x00000000); // Turn off beep (if enabled)

#if defined(_arch_dreamcast) && OPENMENU_ICONS
    /* On successful save, show "SAVE OK" icon and spawn thread to restore after 2 seconds */
    if (result == 0 && vmu_screens_bitmap != 0) {
        crayon_peripheral_vmu_display_icon(vmu_screens_bitmap, OPENMENU_LCD_SAVE_OK);
        thd_create(0, vmu_icon_restore_thread, NULL);
    }
#endif

    return result;
}

/* ===== Save/Load Window Helper Functions ===== */

int8_t
savefile_get_device_status(int8_t device_id) {
    return crayon_savefile_save_device_status(&savefile_details, device_id);
}

uint32_t
savefile_get_device_version(int8_t device_id) {
    if (device_id < 0 || device_id >= CRAYON_SF_NUM_SAVE_DEVICES) {
        return 0;
    }
    return savefile_details.savefile_versions[device_id];
}

void
savefile_refresh_device_info(void) {
    crayon_savefile_update_all_device_infos(&savefile_details);
}

int8_t
savefile_save_to_device(int8_t device_id) {
    int8_t old_device = savefile_details.save_device_id;

    if (crayon_savefile_set_device(&savefile_details, device_id) != 0) {
        savefile_details.save_device_id = old_device;
        return -1;
    }

    settings_sanitize();
    vmu_beep(device_id, 0x000065f0);  /* Turn on beep */
    int8_t result = crayon_savefile_save_savedata(&savefile_details);
    vmu_beep(device_id, 0x00000000);  /* Turn off beep */

#if defined(_arch_dreamcast) && OPENMENU_ICONS
    if (result == 0 && vmu_screens_bitmap != 0) {
        uint8_t single_device = (1 << device_id) & vmu_screens_bitmap;
        if (single_device) {
            crayon_peripheral_vmu_display_icon(single_device, OPENMENU_LCD_SAVE_OK);
            thd_create(0, vmu_icon_restore_thread, NULL);
        }
    }
#endif

    return result;
}

int8_t
savefile_load_from_device(int8_t device_id) {
    int8_t old_device = savefile_details.save_device_id;

    if (crayon_savefile_set_device(&savefile_details, device_id) != 0) {
        savefile_details.save_device_id = old_device;
        return -1;
    }

    savefile_was_migrated = false;
    int8_t result = crayon_savefile_load_savedata(&savefile_details);

    if (result == 0) {
        settings_sanitize();
    }

    return result;
}

int8_t
savefile_get_startup_device_id(void) {
    return startup_device_id;
}

void
savefile_show_success_icon(int8_t device_id) {
#if defined(_arch_dreamcast) && OPENMENU_ICONS
    if (vmu_screens_bitmap != 0) {
        uint8_t single_device = (1 << device_id) & vmu_screens_bitmap;
        if (single_device) {
            crayon_peripheral_vmu_display_icon(single_device, OPENMENU_LCD_SAVE_OK);
            thd_create(0, vmu_icon_restore_thread, NULL);
        }
    }
#else
    (void)device_id;
#endif
}

uint32_t
savefile_get_save_size_blocks(void) {
    uint32_t size_bytes = crayon_savefile_get_savefile_size(&savefile_details);
    /* Convert bytes to 512-byte blocks, rounding up */
    return (size_bytes + 511) / 512;
}

uint32_t
savefile_get_device_free_blocks(int8_t device_id) {
    uint32_t free_bytes = crayon_savefile_devices_free_space(device_id);
    /* Convert bytes to 512-byte blocks */
    return free_bytes / 512;
}