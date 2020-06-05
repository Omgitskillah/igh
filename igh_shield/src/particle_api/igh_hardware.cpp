/*******************************************************************************
 * @file igh_hardware.cpp
 * @brief API to IGH hardware peripherals
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_hardware.h"

// button global variables
uint8_t igh_button_sec_counter;
Timer igh_button_timer(1000, igh_boron_button_press_duration);

// valve variables
volatile valve_position current_valve_position;

// local functions
static void igh_hardware_irrigiation_button_setup(void);
static void igh_hardware_onboard_led_setup(void);
static void igh_hardware_vlave_setup(void);

void igh_hardware_setup(void)
{
    igh_hardware_irrigiation_button_setup();
    igh_hardware_onboard_led_setup();
    igh_hardware_vlave_setup();
}

void igh_hardware_service(void)
{
    igh_hardware_service_valve_state();
}


/* Valve control APIs */
static void igh_hardware_vlave_setup(void)
{
    pinMode(IGH_IRRIGATION_VALVE, OUTPUT);
    current_valve_position = VALVE_CLOSE;
}

void igh_hardware_service_valve_state(void)
{
    digitalWrite(IGH_IRRIGATION_VALVE, current_valve_position);
}

uint8_t igh_hardware_test_valve_state(void)
{
    return digitalRead(IGH_IRRIGATION_VALVE);
}

































/* Onboard LED APIs */
void igh_boron_toggle_boron_led(uint8_t _state)
{
    digitalWrite(BORON_LED, _state);
}

static void igh_hardware_onboard_led_setup(void)
{
    pinMode(BORON_LED, OUTPUT);
    igh_boron_toggle_boron_led(OFF);
}


/* Button APIs */
static void igh_hardware_irrigiation_button_setup(void)
{
    pinMode(IGH_IRRIGATION_BUTTON, INPUT_PULLUP);
    igh_button_timer.start();
    igh_button_sec_counter = 0;
}

uint8_t igh_boron_read_button(void)
{
    return digitalRead(IGH_IRRIGATION_BUTTON);
}

void igh_boron_button_press_duration(void)
{
    if(!igh_boron_read_button())
    {
        igh_button_sec_counter++;
    }
    else
    {
        igh_button_sec_counter = 0;
    } 
}