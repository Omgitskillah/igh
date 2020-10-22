/*******************************************************************************
 * @file igh_hardware.cpp
 * @brief API to IGH hardware peripherals
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_hardware.h"
#include "include/igh_settings.h"

// water flow variables
unsigned long liters_timer = 0;
float total_water_dispensed_Liters = 0;
uint32_t flow_meter_pulses = 0;

#define FLOW_METER_CAL_FACTOR  (4.5)
#define ONE_MIN                (60)
#define WATER_SCALING_FACTOR_L (270) // FLOW_METER_CAL_FACTOR * ONE_MIN


// button global variables
uint8_t igh_button_sec_counter;
Timer igh_button_timer(1000, igh_boron_button_press_duration);
void igh_app_water_counter_callback( void );
void igh_hardware_water_flow_setup( void );
void igh_hardware_litres_service( void );
void detach_flow_meter_interrupt( void );
void attach_flow_meter_interrupt( void );

// local functions
static void igh_hardware_irrigiation_button_setup(void);
static void igh_hardware_onboard_led_setup(void);
static void igh_hardware_vlave_setup(void);

Timer water_flow_timer(ONE_SECOND, igh_hardware_litres_service);


void igh_hardware_setup(void)
{
    igh_hardware_irrigiation_button_setup();
    igh_hardware_onboard_led_setup();
    // igh_hardware_vlave_setup();
    igh_hardware_water_flow_setup();
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

void igh_hardware_water_flow_setup( void )
{
    attach_flow_meter_interrupt();
    water_flow_timer.start();
}

void igh_app_water_counter_callback( void )
{
    flow_meter_pulses++;
}

void attach_flow_meter_interrupt( void )
{
    pinMode( IGH_WATER_FLOW_PIN, INPUT_PULLUP );
    attachInterrupt( IGH_WATER_FLOW_PIN , igh_app_water_counter_callback, FALLING );
}

void detach_flow_meter_interrupt( void )
{
    detachInterrupt( IGH_WATER_FLOW_PIN );
}

void igh_hardware_litres_service( void )
{
    uint32_t copy_flow_meter_pulses = 0;
    detach_flow_meter_interrupt();
    // copy the current flow meter pulses
    copy_flow_meter_pulses = flow_meter_pulses;
    // reset and enable interrupts to avoid missing too many pulses
    flow_meter_pulses = 0;
    attach_flow_meter_interrupt();

    // because we are using a software timer, 
    // this routine will always happen once every second
    float flowRate = copy_flow_meter_pulses / FLOW_METER_CAL_FACTOR;
    float flow_Liters = flowRate / ONE_MIN;

    // add to the total amount of water flow
    total_water_dispensed_Liters += flow_Liters; 

    Serial.print("TOTAL WATER DISP: "); Serial.print(total_water_dispensed_Liters); 
    Serial.print("L SECOND WATER DISP: "); Serial.print(flow_Liters); Serial.println("L");
}