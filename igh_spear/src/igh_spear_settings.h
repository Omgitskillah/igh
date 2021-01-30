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

typedef enum _operation_state_e
{
  STATE_SHIPPING = 0x00,
  STATE_LIVE =     0x42
} operation_state_e;

typedef enum _serial_sensor_type_e
{
  SERIAL_SENSOR_NPK = 0x01,
  SERIAL_SENSOR_CO2 = 0x02
} serial_sensor_type_e;
typedef struct Settings
{
  uint8_t checksum;
  uint8_t op_state;
  uint8_t serial_number[12];
  uint16_t parent_shield_rf_id;
  uint16_t spear_rf_id;
  uint16_t battery_low_threshold;
  // data collection interval in seconds
  unsigned long data_collection_interval;
  uint8_t serial_sensor_type;
} igh_spear_settings;

extern igh_spear_settings active_system_setting;
extern igh_spear_settings new_system_settings; 
extern const uint8_t fw_ver[];

void igh_spear_settings_init( void );
void igh_spear_settings_test_service(void);
void igh_spear_settings_read( igh_spear_settings * settings_buffer );
bool igh_spear_settings_save( igh_spear_settings settings );
uint8_t igh_spear_settings_calculate_checksum(void * p_struct, size_t total_bytes); 

#endif
