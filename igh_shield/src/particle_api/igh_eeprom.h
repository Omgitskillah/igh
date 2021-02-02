/*******************************************************************************
 * @file igh_eeprom.h
 * @brief Save and retreave persistent settings from particle eeprom emulator
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_EEPROM
#define IGH_EEPROM

#include "include/igh_settings.h"

uint8_t igh_eeproom_test(void);

void igh_eeprom_init( void );
uint8_t igh_eeprom_save_system_settings(system_settings * settings_to_save);
uint8_t igh_eeprom_save_threshold_settings(thresholds * settings_to_save);
float igh_eeprom_get_water_flow_in_nv( void );
void igh_eeprom_update_water_flow_in_nv( float nv_water );
void igh_eeprom_update_time_in_nv( uint32_t new_unix_time );
uint32_t igh_eeprom_get_time_in_nv( void );

#endif