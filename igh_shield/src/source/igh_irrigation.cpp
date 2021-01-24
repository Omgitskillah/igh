/*******************************************************************************
 * @file igh_irrigation.cpp
 * @brief Manages the irrigation function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_settings.h"
#include "include/igh_valve.h"
#include "include/igh_water_flow_meter.h"
#include "include/igh_irrigation.h"

#define ONE_HOUR                    3600000
#define SENSOR_IRRIGATION_COOLDOWN  ONE_HOUR // wait this long before you can irrigate again using sensors

bool irrigation_suspended = false;
bool igh_irrigation_enabled = false;
bool max_water_threshold_reached = false;
bool remote_valve_command = false;
valve_position_e remote_valve_state = VALVE_CLOSE;

typedef struct _irrigation_sensor_data_str
{
    bool updated;
    uint16_t reading;
    uint16_t upper_threshold;
    uint16_t lower_threshold;
}irrigation_sensor_data_str;

irrigation_sensor_data_str current_irrigation_data;

extern uint8_t irrigation_settings_updated;

void igh_irrigation_by_the_clock( void );
void igh_irrigation_by_sensor_data( void );
void igh_irrigation_disable_unused_timers( void );

Timer igh_irrigation_by_the_clock_timer(ONE_HOUR, igh_irrigation_by_the_clock);
Timer igh_irrigation_by_sensor_data_timer(SENSOR_IRRIGATION_COOLDOWN, igh_irrigation_by_sensor_data);

void igh_irrigation_update_sensor_data( uint16_t sensor_data )
{
    current_irrigation_data.reading = sensor_data;
    current_irrigation_data.upper_threshold = igh_current_threshold_settings.soil_humidity_high;
    current_irrigation_data.lower_threshold = igh_current_threshold_settings.soil_humidity_low;
    current_irrigation_data.updated = true;
}

void igh_irrigation_minimum_water_to_dispense( void )
{
    igh_valve_change_state(
        VALVE_OPEN,
        ONE_HOUR, 
        (float)igh_current_threshold_settings.water_dispensed_period_low
    );
}

void igh_irrigation_by_the_clock( void )
{
    igh_valve_change_state(
        VALVE_OPEN,
        igh_current_system_settings.water_dispenser_period, 
        (float)igh_current_system_settings.water_amount_by_button_press
    );
}

void igh_irrigation_over_mqtt( void )
{
    if( true == remote_valve_command )
    {
        igh_valve_change_state(
            remote_valve_state,
            igh_current_system_settings.water_dispenser_period, 
            (float)igh_current_system_settings.water_amount_by_button_press
        );

        remote_valve_command = false;
    }
}

void igh_irrigation_by_sensor_data( void )
{
    /**Check sensor data and 
     * action it accordingly */
    if( true == current_irrigation_data.updated )
    {
        if( (current_irrigation_data.reading < current_irrigation_data.upper_threshold) &&
            (current_irrigation_data.reading < current_irrigation_data.lower_threshold) )
        {
            igh_valve_change_state(
                VALVE_OPEN,
                igh_current_system_settings.water_dispenser_period, 
                (float)igh_current_system_settings.water_amount_by_button_press
            );
        }

        // we have now used up this reading
        current_irrigation_data.updated = false;
    }
}

void igh_irrigation_by_button( void )
{
    if( VALVE_CLOSE == current_valve_ctrl.valve_state )
    {
        igh_valve_change_state(
            VALVE_OPEN,
            igh_current_system_settings.water_dispenser_period, 
            (float)igh_current_system_settings.water_amount_by_button_press
        );
    }
    else
    {
        /* else, asume open so close */
        igh_valve_change_state(
            VALVE_CLOSE,
            igh_current_system_settings.water_dispenser_period, 
            (float)igh_current_system_settings.water_amount_by_button_press
        );
    }
}

void igh_irrigation_disable_unused_timers( void )
{
    current_irrigation_data.updated = false;
    if(true == igh_irrigation_by_the_clock_timer.isActive()) 
    { 
        igh_irrigation_by_the_clock_timer.stop(); 
    }
    if(true == igh_irrigation_by_sensor_data_timer.isActive()) 
    { 
        igh_irrigation_by_sensor_data_timer.stop(); 
    }
}

void igh_irrigation_mngr( void )
{
    // runs on system clock
    if( true == irrigation_settings_updated )
    {
        igh_irrigation_disable_unused_timers();
        irrigation_settings_updated = false;
    }
    
    /* avoid going over the upper limit */
    if( total_water_dispensed_Liters >= igh_current_threshold_settings.water_dispensed_period_high )
    {
        max_water_threshold_reached = true;
    }

    if( (true == igh_irrigation_enabled) &&
        (false == max_water_threshold_reached) &&
        (false == irrigation_suspended) )
    {
        if( HOURLY_IRRIGATION == igh_current_system_settings.auto_irrigation_type )
        {
            /* start clock irrigation timer */
            if( false == igh_irrigation_by_the_clock_timer.isActive() )
            {
                igh_irrigation_by_the_clock_timer.changePeriod(igh_current_system_settings.clock_irrigation_interval);
                igh_irrigation_by_the_clock_timer.start();
            }
        }
        else if( SENSOR_IRRIGATION == igh_current_system_settings.auto_irrigation_type )
        {
            if( false == igh_irrigation_by_sensor_data_timer.isActive() )
            {
                igh_irrigation_by_sensor_data_timer.changePeriod(igh_current_system_settings.clock_irrigation_interval);
                igh_irrigation_by_sensor_data_timer.start();
            }
        }
        else
        {   /* in an unknown state */
            igh_irrigation_disable_unused_timers();
        }
    }
    else
    {
        igh_irrigation_disable_unused_timers();
    }
    
}