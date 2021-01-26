/*******************************************************************************
 * @file igh_button.cpp
 * @brief Manages the Button function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_shield.h"
#include "include/igh_button.h"
#include "include/igh_valve.h"
#include "include/igh_irrigation.h"
#include "include/igh_settings.h"
#include "include/igh_message.h"
#include "particle_api/igh_hardware.h"

uint32_t button_seconds_counter = 0;

void igh_button_setup(void);
void igh_button_mngr_callback( void );

Timer igh_button_timer(ONE_SECOND, igh_button_mngr_callback);

void igh_button_setup(void)
{
    pinMode(IGH_IRRIGATION_BUTTON, INPUT_PULLUP);
    button_seconds_counter = 0;
    igh_button_timer.start();
}

void igh_button_mngr_callback( void )
{
    if( LOW == digitalRead(IGH_IRRIGATION_BUTTON) )
    {
        button_seconds_counter++;
        // Serial.print("Button Pressed: "); Serial.print(button_seconds_counter); Serial.println(" Seconds");
    }
    else
    {
        button_seconds_counter = 0;
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
            if( true == Time.isValid() )
            {
                // only enable button irrigation if time is valid
                igh_irrigation_by_button();
            }
        }
        else if ( button_seconds_counter >= BUTTON_PRESS_TOGGLE_SYS )
        {
            /* disable/enable irrigation */
            igh_irrigation_toggle();
        }
        else
        {
            /* do nothing */
        }
        button_seconds_counter = 0;
    }
}