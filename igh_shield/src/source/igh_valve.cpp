/*******************************************************************************
 * @file igh_valve.cpp
 * @brief Manages the valve function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_shield.h"
#include "include/igh_message.h"
#include "particle_api/igh_hardware.h"
#include "include/igh_valve.h"

bool valve_timer_expired = false;
bool set_flow_expired = false;
float current_valve_open_water_flow = 0;
valve_ctrl_str current_valve_ctrl;
bool call_once = true;

#define FIVE_MINS 300000

void igh_valve_idle( void );
void igh_valve_reset_state( void );
void igh_valve_timer_action( void );
void igh_valve_flow_action( void );
void igh_valve_ctrl( void );
void igh_valve_close( void );
void igh_valve_open( void );
void igh_valve_idle( void );


Timer igh_valve_timer(FIVE_MINS, igh_valve_timer_action, call_once);

void igh_valve_reset_state( void )
{
    current_valve_ctrl.valve_state = VALVE_CLOSE;
    current_valve_ctrl.open_litres = 0;
    current_valve_ctrl.open_duration = 0;
}

void init_valve( void )
{
    /* init pins */
    pinMode(IGH_VALVE_YELLOW, OUTPUT);
    pinMode(IGH_VALVE_BLUE, OUTPUT);
    pinMode(IGH_VALVE_CLOSE, INPUT_PULLUP);
    pinMode(IGH_VALVE_OPEN, INPUT_PULLUP);

    /* init valve ctrs struct */
    igh_valve_reset_state();
}

void igh_valve_mngr( void )
{
    // runs in system clock
    if( VALVE_OPEN == current_valve_ctrl.valve_state )
    {
        /**if an application has intentionally enabled the valve,
         * evaluate what to do here
         **/
        if( false == igh_valve_timer.isActive() )
        {
            igh_valve_timer.changePeriod( current_valve_ctrl.open_duration );
            igh_valve_timer.start();
            valve_timer_expired = false;
        }

        if( (true == valve_timer_expired) ||
            (true == set_flow_expired) )
        {
            /**if the valve is open for more than a set duration, or
             * if the water flowing has hit the set threshold,
             * close the valve and reset the params
             * */
            igh_valve_reset_state();
            set_flow_expired = false;
            
            if( true == igh_valve_timer.isActive() )
            {
                /* stop the timer if it is still running, this should only happen if the flow flag is set */
                igh_valve_timer.stop();
            }
        }
    }

    /* action the valve state */
    igh_valve_ctrl();
}

void igh_valve_timer_action( void )
{
    valve_timer_expired = true;
}

void igh_valve_flow_action( void )
{
    if( current_valve_open_water_flow >= current_valve_ctrl.open_litres )
    {
        set_flow_expired = true;
    }
}


void igh_valve_ctrl( void )
{
    // runs on system clock
    if( VALVE_CLOSE == current_valve_ctrl.valve_state ) 
    {
        igh_valve_close();
    }
    else if( VALVE_OPEN == current_valve_ctrl.valve_state )
    {
        igh_valve_open();
    }

    // keep track of the water flow here
    igh_valve_flow_action();
}

void igh_valve_close( void )
{
    if( HIGH == digitalRead(IGH_VALVE_CLOSE) )
    {
        digitalWrite(IGH_VALVE_YELLOW, HIGH);
        digitalWrite(IGH_VALVE_BLUE, LOW);
    }
    else
    {
        igh_valve_close();
    }
}

void igh_valve_open( void )
{
    if( HIGH == digitalRead(IGH_VALVE_OPEN) )
    {
        digitalWrite(IGH_VALVE_YELLOW, LOW);
        digitalWrite(IGH_VALVE_BLUE, HIGH);
    }
    else
    {
        igh_valve_idle();
    }
}

void igh_valve_idle( void )
{
    if( (LOW == digitalRead(IGH_VALVE_YELLOW)) &&
        (LOW == digitalRead(IGH_VALVE_BLUE)) )
    {
        /* only change things if in the wrong state */
        digitalWrite(IGH_VALVE_YELLOW, LOW);
        digitalWrite(IGH_VALVE_BLUE, LOW);
    }
}

void igh_valve_change_state( valve_position_e _valve_state, uint32_t _open_duration, float _open_flow )
{
    if( _valve_state != current_valve_ctrl.valve_state )
    {
        /**only change things if things if there is a difference in valve state
         * otherwise keep system as is
         */
        if( VALVE_CLOSE == current_valve_ctrl.valve_state )
        {
            // if someone wants to close the valve, reset the state
            igh_valve_reset_state();
#ifdef IGH_DEBUG
            Serial.println("VALVE CLOSE REQUEST");
#endif
            igh_message_event(EVENT_VAVLE_CLOSED, true);
        }
        else
        {
            current_valve_ctrl.valve_state = _valve_state;
            current_valve_ctrl.open_duration = _open_duration;
            current_valve_ctrl.open_litres = _open_flow;
#ifdef IGH_DEBUG
            Serial.println("VALVE OPEN REQUEST");
#endif
            igh_message_event(EVENT_VAVLE_OPENED, true);
        }
    }
    else
    {
        /* ignore request? */
    }
}
