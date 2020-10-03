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

typedef struct Settings
{
  uint8_t checksum;
  uint8_t serial_number[12];
  unsigned long data_collection_interval;
  unsigned long data_tx_interval;
} igh_spear_settings;


void igh_spear_settings_test_service(void);
void igh_spear_settings_read( igh_spear_settings * settings_buffer );
bool igh_spear_settings_save( igh_spear_settings settings );

#endif
