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

#define MIDNIGHT 0

bool midnight_reset_done = false; 
bool enable_irrigation_flag = false; 

void igh_time_keeper_init( void )
{
    Time.zone(igh_current_system_settings.timezone);
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
                Serial.println("IRRIGATION DISABLED");
#endif
                igh_message_event(EVENT_IRRIGATION_DISABLED, true);
                enable_irrigation_flag = false;
            }
        }
        else
        {
            midnight_reset_done = false;
            igh_irrigation_enabled = true;

            if( false == enable_irrigation_flag )
            {
#ifdef IGH_DEBUG
                Serial.println("IRRIGATION ENABLED");
#endif
                igh_message_event(EVENT_IRRIGATION_ENABLED, true);
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