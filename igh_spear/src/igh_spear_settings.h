/*******************************************************************************
 * @file igh_spear_settings.h
 * @brief API for updatings device settings, uses mcu flash as form of eeprom,
 *        Settings are lost after every flash write of firmware
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SPEAR_SETTINGS
#define IGH_SPEAR_SETTINGS

#include "flash/FlashStorage.h"

typedef enum operation_state
{
  STATE_SHIPPING = 0x00,
  STATE_LIVE =     0x42
};
typedef struct Settings
{
  uint8_t checksum;
  uint8_t op_state;
  uint8_t serial_number[12];
  uint16_t parent_shield_rf_id;
  uint16_t spear_rf_id;
  // data collection interval in seconds
  unsigned long data_collection_interval;
} igh_spear_settings;

extern igh_spear_settings active_system_setting;
extern igh_spear_settings new_system_settings; 

void igh_spear_settings_init( void );
void igh_spear_settings_test_service(void);
void igh_spear_settings_read( igh_spear_settings * settings_buffer );
bool igh_spear_settings_save( igh_spear_settings settings );
uint8_t igh_spear_settings_calculate_checksum(void * p_struct, size_t total_bytes); 

#endif
