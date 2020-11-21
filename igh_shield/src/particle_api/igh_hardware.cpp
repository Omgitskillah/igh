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
uint8_t previous_hr = 0;
bool ok_to_irrigate = false;
uint32_t valve_open_seconds_counter = 0;
bool button_irrigate = false;
float water_dispensed_by_button = 0;
uint16_t soil_humidity = 0;
uint16_t soil_temperature = 0;
uint8_t  refreshed_soil_data = INVALID_SOIL_DATA;


// temp 
uint8_t hour_counter = 0;

#define FLOW_METER_CAL_FACTOR  (4.5)
#define ONE_MIN                (60)
#define WATER_SCALING_FACTOR_L (270) // FLOW_METER_CAL_FACTOR * ONE_MIN

#define TWENTY_FOUR_HOURS      (24)
#define MIDNIGHT               (0)
#define THREE_SECONDS          (3)


// button global variables
uint8_t igh_button_sec_counter;
Timer igh_button_timer(ONE_SECOND, igh_boron_button_press_duration);
void igh_app_water_counter_callback( void );
void igh_hardware_water_flow_setup( void );
void igh_hardware_litres_service( void );
void detach_flow_meter_interrupt( void );
void attach_flow_meter_interrupt( void );
uint8_t igh_get_local_time_hour( void );
void igh_hardware_water_management_service( void );

// local functions
static void igh_hardware_irrigiation_button_setup(void);
static void igh_hardware_onboard_led_setup(void);
static void igh_hardware_vlave_setup(void);
void igh_hardware_valve_open_timer_service( void );
void idle_valve( void );
void open_valve( void );
void close_valve( void );

Timer water_flow_timer(ONE_SECOND, igh_hardware_litres_service);



void igh_hardware_setup(void)
{
    igh_hardware_irrigiation_button_setup();
    igh_hardware_onboard_led_setup();
    igh_hardware_vlave_setup();
    igh_hardware_water_flow_setup();
}

void igh_hardware_service(void)
{
    igh_hardware_service_valve_state();
}


/* Valve control APIs */
static void igh_hardware_vlave_setup(void)
{
    pinMode(IGH_VALVE_YELLOW, OUTPUT);
    pinMode(IGH_VALVE_BLUE, OUTPUT);
    pinMode(IGH_VALVE_CLOSE, INPUT_PULLUP);
    pinMode(IGH_VALVE_OPEN, INPUT_PULLUP);
    
    current_valve_position = VALVE_CLOSE;
}

void igh_hardware_service_valve_state( void )
{
    if( VALVE_CLOSE == current_valve_position ) 
    {
        close_valve();
    }
    else if( VALVE_OPEN == current_valve_position )
    {
        open_valve();
    }
}

void close_valve( void )
{
    if( HIGH == digitalRead(IGH_VALVE_CLOSE) )
    {
        digitalWrite(IGH_VALVE_YELLOW, HIGH);
        digitalWrite(IGH_VALVE_BLUE, LOW);
    }
    else
    {
        idle_valve();
    }
}

void open_valve( void )
{
    if( HIGH == digitalRead(IGH_VALVE_OPEN) )
    {
        digitalWrite(IGH_VALVE_YELLOW, LOW);
        digitalWrite(IGH_VALVE_BLUE, HIGH);
    }
    else
    {
        idle_valve();
    }
}

void idle_valve( void )
{
    digitalWrite(IGH_VALVE_YELLOW, LOW);
    digitalWrite(IGH_VALVE_BLUE, LOW);
}

uint8_t igh_hardware_test_valve_state(void)
{
    return (uint8_t)current_valve_position;
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

    if( igh_button_sec_counter >= THREE_SECONDS )
    {
        // start or stop irrigation at any time with button
        button_irrigate = !button_irrigate;

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

    // if button is open, keep track of how much water is used up then
    if( true == button_irrigate )
    {
        water_dispensed_by_button += flow_Liters;
    }
    else
    {
        water_dispensed_by_button = 0;
    }

    // keep track of valve open state
    igh_hardware_valve_open_timer_service();
    // control valve state
    igh_hardware_water_management_service();
}

uint8_t igh_get_local_time_hour( void )
{
    int local_hour = 0;
    int utc_hour = 0;
    utc_hour = Time.hour();

    //correct to timezone
    local_hour = utc_hour + igh_current_system_settings.timezone;

    if( local_hour < 0 )
    {
        local_hour += TWENTY_FOUR_HOURS;
    }
    else if( local_hour > TWENTY_FOUR_HOURS )
    {
        local_hour -= TWENTY_FOUR_HOURS;
    }

    return (uint8_t)local_hour;
}

void igh_hardware_water_management_service( void )
{
    // This should run on a one second tick
    uint8_t current_hr = igh_get_local_time_hour();

    if( current_hr != previous_hr )
    {
        if( current_hr == igh_current_system_settings.irrigation_hr )
        {
            // it is ok to irrigat if it hits the irrigation hour
            ok_to_irrigate = true;
            time_t time = Time.now();
            Serial.print("IRRIGATION TIME STARTED: ");
            Serial.println( Time.format(time, TIME_FORMAT_DEFAULT) );
        }
        else if( MIDNIGHT == current_hr )
        {
            // reset the system at midnight
            ok_to_irrigate = false;
            total_water_dispensed_Liters = 0;
            
            time_t time = Time.now();
            Serial.print("RESETTING WATER PARAMS: ");
            Serial.println( Time.format(time, TIME_FORMAT_DEFAULT) );
        }

        previous_hr = current_hr;
    }

    if( true == button_irrigate )
    {
        if( (water_dispensed_by_button < (float)igh_current_system_settings.water_amount_by_button_press) &&
            (valve_open_seconds_counter < igh_current_system_settings.water_dispenser_period) )
        {
            current_valve_position = VALVE_OPEN;
        }
        else
        {
            button_irrigate = false;
            current_valve_position = VALVE_CLOSE;
        }
    }
    else
    {
        /* Only do auto irrigation if button irrigation is not set */
        if( true == ok_to_irrigate)
        {
            bool automatic_irrigation_mode = false;

            if( true == automatic_irrigation_mode )
            {
                if( VALID_SOIL_DATA == refreshed_soil_data )
                {
                    /**
                     * run the dispensor if we are within the dispensor window
                     * and if we have valid sensor data
                     * */
                    if( soil_humidity < igh_current_threshold_settings.soil_humidity_low &&
                        soil_humidity < igh_current_threshold_settings.soil_humidity_high &&
                        total_water_dispensed_Liters < (float)igh_current_threshold_settings.water_dispensed_period_high )
                    {
                        /**
                         * Dispense water only if
                         * 1. the soil moisture is too low
                         * 2. The soil moisture is not too high
                         * 3. We have not dispensed too much water 
                         * */
                        current_valve_position = VALVE_OPEN;
                    }
                    else
                    {
                        /**
                         * close valve if even one of these conditions is not met
                         * current_valve_position <--- check this
                         * */
                        current_valve_position = VALVE_CLOSE;
                    }
                }
            }
            else
            {
                // if it is now ok to irrigate, open the valve immediately 
                // then wait till the first batch of water is used up
                if( total_water_dispensed_Liters >= (float)igh_current_threshold_settings.water_dispensed_period_low )
                {
                    current_valve_position = VALVE_CLOSE;
                    automatic_irrigation_mode = true;
                }
                else
                {
                    current_valve_position = VALVE_OPEN;
                }
            }
        }
        else
        {
            // close valve
            current_valve_position = VALVE_CLOSE;
        }
    }
}

void igh_hardware_valve_open_timer_service( void )
{
    // count for how many seconds the valve is open
    if( VALVE_OPEN == current_valve_position )
    {
        valve_open_seconds_counter++;
    }
    else
    {
        valve_open_seconds_counter = 0;
    }
}

