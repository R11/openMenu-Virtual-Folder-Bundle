#ifndef OPENMENU_SAVEFILE_H
#define OPENMENU_SAVEFILE_H

#include <crayon_savefile/savefile.h>

void savefile_defaults();

uint8_t setup_savefile(crayon_savefile_details_t* details);

int8_t update_savefile(void** loaded_variables, crayon_savefile_version_t loaded_version,
                       crayon_savefile_version_t latest_version);

int8_t find_first_valid_savefile_device(crayon_savefile_details_t* details);
void savefile_init();
void savefile_close();
int8_t savefile_save();

/* Save/Load window helper functions */
int8_t savefile_get_device_status(int8_t device_id);
uint32_t savefile_get_device_version(int8_t device_id);
void savefile_refresh_device_info(void);
int8_t savefile_save_to_device(int8_t device_id);
int8_t savefile_load_from_device(int8_t device_id);
int8_t savefile_get_startup_device_id(void);
void savefile_show_success_icon(int8_t device_id);
uint32_t savefile_get_save_size_blocks(void);
uint32_t savefile_get_device_free_blocks(int8_t device_id);

#endif //OPENMENU_SAVEFILE_H
