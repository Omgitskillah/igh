/*******************************************************************************
 * @file igh_power_mgt.cpp
 * @brief manage power on the platform
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_message.h"
#include "include/igh_settings.h"
#include "include/igh_power_mgt.h"
#include "particle_api/igh_sd_log.h"
#include "./simcard.inc"

#define MODEM_POWER_ON_INTERVAL 7200000 // 2 hours in milliseconds
#define MIN_MODEM_ON_TIME       300000 // 5 mins
SYSTEM_MODE(CURRENT_SYSTEM_MODE);

bool modem_on_period_expired = true;

FuelGauge igh_power;

void set_modem_on_period_expired( void );
void igh_power_mgt_turn_on_modem( void );


Timer modem_power_on_timer( MODEM_POWER_ON_INTERVAL, igh_power_mgt_turn_on_modem, true );
Timer min_modem_on_period( MIN_MODEM_ON_TIME, set_modem_on_period_expired, true );


void set_modem_on_period_expired( void )
{
    modem_on_period_expired = true;
}

bool igh_power_mgt_battery_ok( void )
{
    bool ret = true;

    float batt_low_threshold = (float)igh_current_threshold_settings.shield_battery_level_low / 1000;

    if( igh_power.getVCell() < batt_low_threshold )
    {
        ret = false;
    }

    return ret;
}

void igh_power_mgt_turn_on_modem( void )
{
    Cellular.on();
    Cellular.connect();
    Particle.connect();
    modem_on_period_expired = false;
    igh_message_event( EVENT_CALL_HOME, true );

    if( false == min_modem_on_period.isActive() )
    {
        min_modem_on_period.start();
    }
}

void igh_power_mgt_modem_comms_manager( void )
{
    char next_file[FILE_NAME_SIZE];
    if( (true == modem_on_period_expired) &&
        (true == Time.isValid()) &&
        (false == igh_sd_log_get_next_file_name(next_file)) )
    {
        Cellular.disconnect();
        Cellular.off();
        if( false == modem_power_on_timer.isActive() )
        {
            modem_power_on_timer.changePeriod( igh_current_system_settings.reporting_interval );
            modem_power_on_timer.start();
        }
    }
}



