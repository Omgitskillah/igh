/*******************************************************************************
 * @file igh_time_keeper.cpp
 * @brief Manages time sensitive features of the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_shield.h"
#include "include/igh_settings.h"
#include "include/igh_valve.h"
#include "include/igh_irrigation.h"
#include "include/igh_message.h"
#include "include/igh_water_flow_meter.h"
#include "include/igh_time_keeper.h"
#include "particle_api/igh_eeprom.h"

#define MIDNIGHT 0

bool midnight_reset_done = false; 
bool enable_irrigation_flag = false; 
bool day_watch_validated = false;

void igh_time_keeper_day_watch( void );

void igh_time_keeper_init( void )
{
    Time.zone(igh_current_system_settings.timezone);
}

void igh_time_keeper_day_watch( void )
{
    if( false == day_watch_validated )
    {
        // this should only ever take effect on a reset
        uint32_t unix_time_in_nv = igh_eeprom_get_time_in_nv();
        uint32_t day_in_nv = Time.day(unix_time_in_nv);
        uint32_t current_unix_time = Time.now();
        uint32_t current_day = Time.day(current_unix_time);

        if( current_day != day_in_nv )
        {
            // if we find ourselves in a different day, reset irrigation
            igh_water_flow_meter_reset_nv();
            igh_eeprom_update_time_in_nv(current_unix_time);
        }

        day_watch_validated = true;
    }
}

void igh_time_keeper_churn( void )
{
    // runs on system clock
    if( true == timezone_updated )
    {
        igh_time_keeper_init();
        timezone_updated = false;
    }

    if( true == Time.isValid() )
    {
        int irrigation_o_clock = igh_current_system_settings.irrigation_hr;
        int current_hour = Time.hour();

        if( current_hour < irrigation_o_clock )
        {
            /**if it is not yet time to irrigate,
             * reset the system */
            if( false == midnight_reset_done )
            {
                igh_water_flow_meter_reset_nv();
                midnight_reset_done = true;
            }
            igh_irrigation_enabled = false;
            if( true == enable_irrigation_flag )
            {
#ifdef IGH_DEBUG
                Serial.println("EVENT: IRRIGATION DISABLED");
#endif
                igh_message_event(EVENT_IRRIGATION_DISABLED, true);
                enable_irrigation_flag = false;
            }
        }
        else
        {
            midnight_reset_done = false;
            igh_irrigation_enabled = true;

            igh_time_keeper_day_watch();

            if( false == enable_irrigation_flag )
            {
#ifdef IGH_DEBUG
                Serial.println("EVENT: IRRIGATION ENABLED");
#endif
                igh_message_event(EVENT_IRRIGATION_ENABLED, true);
                
                // update the time in NV
                uint32_t current_unix_time = Time.now();
                igh_eeprom_update_time_in_nv(current_unix_time);

                enable_irrigation_flag = true;
            }
        }
    }
    else
    {
        /* keep irrigation off if we do not have the right time */
        igh_irrigation_enabled = false;
    }
    
}