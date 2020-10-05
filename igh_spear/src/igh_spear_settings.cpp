/*******************************************************************************
 * @file igh_spear_settings.cpp
 * @brief API for updatings device settings, uses mcu flash as form of eeprom,
 *        Settings are lost after every flash write of firmware
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_settings.h"
#include "igh_spear_log.h"
#include "igh_spear_rfm69.h"

/* uncomment to enable debug */
#define LOG_IGH_SPEAR_SETTINGS

FlashStorage(settings_store, igh_spear_settings);

/* System defaults */
uint8_t default_serail_number[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint16_t default_parent_shield_rf_id = 0;
uint16_t default_spear_rf_id = 1;
unsigned long default_data_collection_interval = 5; // time in seconds

igh_spear_settings active_system_setting;
igh_spear_settings default_system_settings;
igh_spear_settings new_system_settings;  

/* Local functions */
void igh_spear_settings_get_defaults( void );


void igh_spear_settings_get_defaults( void )
{
    memcpy( default_system_settings.serial_number, default_serail_number, sizeof(default_serail_number));
    default_system_settings.parent_shield_rf_id = 0;
    default_system_settings.spear_rf_id = 1;
    default_system_settings.data_collection_interval = 5;
}

uint8_t igh_spear_settings_calculate_checksum(void * p_struct, size_t total_bytes)
{
    uint8_t length = (uint8_t)total_bytes; 
    uint8_t * data;
    int sum = 0;
    uint8_t checksum = 0;

    data = (uint8_t *)p_struct;

    data += 1; // ignore checksum byte
    // start at 1 to ignore the checksum byte
    for( int i = 1; i < length; i++)
    {
        sum += (0xFF & *data++);
    }

    checksum = (uint8_t)(sum % 256);

    return checksum;
}

void igh_spear_settings_init( void )
{
    igh_spear_settings_get_defaults();
    // get the settings from memory
    igh_spear_settings_read(&active_system_setting);

    // compare the checksums
    uint8_t valid_checksum = 0;
    valid_checksum = igh_spear_settings_calculate_checksum( &active_system_setting, sizeof(active_system_setting));

    // default if the checksums do not match
    if( active_system_setting.checksum != valid_checksum )
    {
        active_system_setting = default_system_settings;
    }
    // else, procede with active settings 

#ifdef LOG_IGH_SPEAR_SETTINGS
    igh_spear_log("SETTINGS USED\nSN:");
    for( int i = 0; i < sizeof(active_system_setting.serial_number); i++)
    {
        sprintf(debug_buff, "%02X", active_system_setting.serial_number[i]);
    }
    igh_spear_log(debug_buff);
    sprintf(debug_buff, "SHIELD ID: %d\nSPEAR ID: %d\nDATA INTERVAL: %d\n", 
            active_system_setting.parent_shield_rf_id,
            active_system_setting.spear_rf_id,
            active_system_setting.data_collection_interval);
    igh_spear_log(debug_buff);
#endif
}

bool igh_spear_settings_save( igh_spear_settings settings )
{
    uint8_t ret = 0;
    igh_spear_settings _read;

    settings_store.write(settings);

    igh_spear_settings_read(&_read);

    if ( _read.checksum == settings.checksum )
    {
        ret = 1;
    }
    return ret;
}

void igh_spear_settings_read( igh_spear_settings * settings_buffer )
{
    *settings_buffer = settings_store.read();
}

void igh_spear_settings_test_service(void)
{
    // create settings insatnce
    igh_spear_settings current_settings;
    
    // populate settings
    current_settings.checksum = 99;
    uint8_t sn[] = {0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7,0xa7};
    memcpy(current_settings.serial_number, sn, sizeof(sn));
    if( true == igh_spear_settings_save(current_settings) )
    {
#ifdef LOG_IGH_SPEAR_SETTINGS
        igh_spear_log("SETTINGS...............OK\n");
#endif
    }
    else
    {
#ifdef LOG_IGH_SPEAR_SETTINGS
        igh_spear_log("SETTINGS...............ERROR\n");
#endif
    }
    
}

