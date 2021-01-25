/*******************************************************************************
 * @file igh_water_flow_meter.cpp
 * @brief Manages the flow meter
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "include/igh_shield.h"
#include "particle_api/igh_hardware.h"
#include "particle_api/igh_eeprom.h"
#include "include/igh_valve.h"
#include "include/igh_message.h"
#include "include/igh_water_flow_meter.h"

#define FLOW_METER_CAL_FACTOR  (4.5)
#define ONE_MIN_IN_SECONDS (60)

uint32_t flow_meter_pulses = 0;
float total_water_dispensed_Liters = 0;
bool update_water_flow_nv = false;

void attach_flow_meter_interrupt( void );
void detach_flow_meter_interrupt( void );
void igh_app_water_counter_callback( void );
void igh_water_flow_meter_service( void );
void igh_water_flow_meter_update_nv( float _total_water_dispensed_Liters );

Timer water_flow_meter_timer(ONE_SECOND, igh_water_flow_meter_service);

void attach_flow_meter_interrupt( void )
{
    pinMode( IGH_WATER_FLOW_PIN, INPUT_PULLUP );
    attachInterrupt( IGH_WATER_FLOW_PIN , igh_app_water_counter_callback, FALLING );
}

void detach_flow_meter_interrupt( void )
{
    detachInterrupt( IGH_WATER_FLOW_PIN );
}

void igh_water_flow_meter_setup( void )
{
    attach_flow_meter_interrupt();
    EEPROM.get(SYSTEM_IRRIGATION_FLAGS, total_water_dispensed_Liters);
    water_flow_meter_timer.start();
}

void igh_app_water_counter_callback( void )
{
    flow_meter_pulses++;
}

void igh_water_flow_meter_service( void )
{
    uint32_t copy_flow_meter_pulses = 0;
    
    detach_flow_meter_interrupt();
    
    copy_flow_meter_pulses = flow_meter_pulses;
    flow_meter_pulses = 0;
    
    attach_flow_meter_interrupt();
    
    float flowRate = copy_flow_meter_pulses / FLOW_METER_CAL_FACTOR;
    float flow_Liters = flowRate / ONE_MIN_IN_SECONDS;

    total_water_dispensed_Liters += flow_Liters; 

    if( VALVE_OPEN == current_valve_ctrl.valve_state )
    {
        current_valve_open_water_flow += flow_Liters;
        update_water_flow_nv = true;
    }
    else
    {
        // update the water flow nv once every time the valve closes
        igh_water_flow_meter_update_nv(total_water_dispensed_Liters);
        current_valve_open_water_flow = 0;
    }
}

void igh_water_flow_meter_reset_nv( void )
{
    update_water_flow_nv = true;
    total_water_dispensed_Liters = 0;
    igh_water_flow_meter_update_nv( total_water_dispensed_Liters );
#ifdef IGH_DEBUG
    Serial.println("IRRIGATION SYSTEM RESET");
#endif
    igh_message_event(EVENT_RESET_IRRIGATION, true);
}

void igh_water_flow_meter_update_nv( float _total_water_dispensed_Liters )
{
    if( true == update_water_flow_nv )
    {
        EEPROM.put(SYSTEM_IRRIGATION_FLAGS, _total_water_dispensed_Liters);
        update_water_flow_nv = false;
    }
}


