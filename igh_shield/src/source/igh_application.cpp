/*******************************************************************************
 * @file igh_application.cpp
 * @brief Main IGH application for the Shield board
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "particle_api/igh_boron.h"
#include "particle_api/igh_eeprom.h"
#include "particle_api/igh_hardware.h"
#include "particle_api/igh_rfm69.h"
#include "particle_api/igh_sd_log.h"
#include "particle_api/igh_mqtt.h"
#include "include/igh_button.h"
#include "include/igh_message.h"
#include "include/igh_valve.h"
#include "include/igh_irrigation.h"
#include "include/igh_water_flow_meter.h"
#include "include/igh_application.h"
#include "include/igh_time_keeper.h"

bool system_reset = true;

void igh_application_publish_restart( void );

void igh_application_setup( void )
{
    Serial.begin( 19200 );
    init_valve();
    igh_boron_setup();
    igh_button_setup();
    igh_hardware_setup();
    igh_eeprom_init();
    igh_water_flow_meter_setup();
    igh_rfm69_setup();
    igh_sd_log_setup();
    igh_message_setup();
    igh_time_keeper_init();
}

void igh_application_churn( void )
{
    /**
     * Functions that should run as fast as possible
    */
    igh_message_get_new_settings();
    igh_message_commit_new_settings();
    igh_time_keeper_churn();
    igh_button_mngr();
    igh_irrigation_mngr();
    igh_boron_service();
    igh_rfm69_service();
    igh_message_receive_and_stage_sensor_data();
    igh_mqtt_service();
    igh_application_publish_restart();
    igh_valve_mngr();
}

void igh_application_publish_restart( void )
{
    if( (true == Time.isValid()) &&
        (true == system_reset) )
    {
        igh_message_event(EVENT_DEVICE_RESTART, true);
        system_reset = false;
    }
}

