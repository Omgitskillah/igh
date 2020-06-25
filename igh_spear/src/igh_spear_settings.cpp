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

/* uncomment to enable debug */
// #define LOG_IGH_SPEAR_SETTINGS

FlashStorage(settings_store, igh_spear_settings);

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
        igh_spear_log("SETTINGS OK\n");
#endif
    }
    else
    {
#ifdef LOG_IGH_SPEAR_SETTINGS
        igh_spear_log("SETTINGS ERROR\n");
#endif
    }
    
}

