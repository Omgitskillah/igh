/*******************************************************************************
 * @file igh_eeprom.h
 * @brief Save and retreave persistent settings from particle eeprom emulator
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_EEPROM
#define IGH_EEPROM

#include "include/igh_settings.h"

// We only have 4KB for the Boron
#define SYSTEM_SETTINGS_ADDRESS     (int)0x0000
#define SYSTEM_THRESHOLDS_ADDRESS   (int)0x0200
#define SYSTEM_ERRORS_ADDRESS       (int)0x0400
#define SYSTEM_IRRIGATION_FLAGS     (int)0x0600
#define SYSTEM_FW_VERSION           (int)0x0800

uint8_t igh_eeproom_test(void);

void igh_eeprom_init( void );
uint8_t igh_eeprom_save_system_settings(system_settings * settings_to_save);
uint8_t igh_eeprom_save_threshold_settings(thresholds * settings_to_save);
void igh_eeprom_update_errors(uint32_t error_bit_field);
uint8_t igh_eeprom_read_system_settings(system_settings * running_system_settings_buffer);
uint8_t igh_eeprom_read_threshold_settings(thresholds * running_thresholds);
void igh_eeprom_get_errors(uint32_t * error_bit_field);

#endif