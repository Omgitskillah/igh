/*******************************************************************************
 * @file igh_button.cpp
 * @brief Manages the Button function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_button.h"
#include "include/igh_valve.h"
#include "include/igh_irrigation.h"
#include "include/igh_settings.h"
#include "particle_api/igh_hardware.h"

uint32_t button_seconds_counter = 0;

void igh_button_setup(void);
void igh_button_mngr_callback( void );
void igh_button_timer_ctrl( void );

Timer igh_button_timer(ONE_SECOND, igh_button_mngr_callback);

void igh_button_setup(void)
{
    pinMode(IGH_IRRIGATION_BUTTON, INPUT_PULLUP);
    attachInterrupt( IGH_IRRIGATION_BUTTON , igh_button_timer_ctrl, CHANGE );
}

void igh_button_timer_ctrl( void )
{
    if( LOW == digitalRead(IGH_IRRIGATION_BUTTON) )
    {
        /* start a timer */
        igh_button_timer.startFromISR();
    }
    else
    {
        /* stop timer */
        igh_button_timer.stopFromISR();
    }
}

void igh_button_mngr_callback( void )
{
    if( LOW == digitalRead(IGH_IRRIGATION_BUTTON) )
    {
        button_seconds_counter++;
    }
}

void igh_button_mngr( void )
{
    // runs on system clock
    if( (button_seconds_counter != 0) &&
        (HIGH == digitalRead(IGH_IRRIGATION_BUTTON)) )
    {
        if( (button_seconds_counter >= BUTTON_PRESS_TOGGLE_VALVE) &&
            (button_seconds_counter < BUTTON_PRESS_TOGGLE_SYS) )
        {
            /* close or open valve */ 
            igh_irrigation_by_button();
        }
        else if ( button_seconds_counter >= BUTTON_PRESS_TOGGLE_SYS )
        {
            /* disable/enable irrigation */
            irrigation_suspended = !irrigation_suspended; 
        }
        else
        {
            /* do nothing */
        }
        /* reset the seconds counter */
        button_seconds_counter = 0;
    }
}