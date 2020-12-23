/*******************************************************************************
 * @file igh_hardware.cpp
 * @brief API to IGH hardware peripherals
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_hardware.h"
#include "include/igh_settings.h"
#include "igh_eeprom.h"

// water flow variables
unsigned long liters_timer = 0;
float total_water_dispensed_Liters = 0;
uint32_t flow_meter_pulses = 0;
uint8_t previous_hr = 0;
uint8_t current_hr = 0;
uint32_t valve_open_seconds_counter = 0;
bool button_irrigate = false;
bool sensor_irrigation = false;
float water_dispensed_by_button = 0;
float water_dispensed_automatically = 0;
uint16_t soil_humidity = 0;
uint16_t soil_temperature = 0;
uint8_t  refreshed_soil_data = INVALID_SOIL_DATA;

bool irrigation_params_updated = false;
uint8_t print_water_counte = 0;

// temp 
uint8_t hour_counter = 0;

bool automatic_irrigation_mode = true;
bool irrigate_by_the_hour = false;

#define FLOW_METER_CAL_FACTOR  (4.5)
#define ONE_MIN                (60)

#define WATER_SCALING_FACTOR_L (270) // FLOW_METER_CAL_FACTOR * ONE_MIN

#define OK_TO_IRRIGATE                     (0x02020202)
#define NOT_OK_TO_IRRIGATE                 (0x00000000)

#define MIN_AMOUNT_OF_WATER_WAS_DISPENSED  (0x09ABCDEF)
#define MIN_AMOUNT_OF_WATER_NOT_DISPENSED  (0x00000000)


// button global variables
uint8_t igh_button_sec_counter;
uint8_t igh_button_sec_counter_to_send = 0;
Timer igh_button_timer(ONE_SECOND, igh_boron_button_press_duration);
void igh_app_water_counter_callback( void );
void igh_hardware_water_flow_setup( void );
void igh_hardware_hourly_clock( void );
void igh_hardware_litres_service( void );
void detach_flow_meter_interrupt( void );
void attach_flow_meter_interrupt( void );
uint8_t igh_get_local_time_hour( void );
void igh_hardware_water_management_service( void );
void igh_hardware_manage_time_to_irrigate( void );

void igh_automatic_irrigation_service( void );
void igh_irrigation_by_sensor_data( void );
void igh_hourly_irrigation( void );
void igh_hourly_irrigation_timer( void );
void igh_irrigiation_by_button_service( void );
void igh_water_cap( void );

// local functions
static void igh_hardware_irrigiation_button_setup(void);
static void igh_hardware_onboard_led_setup(void);
static void igh_hardware_vlave_setup(void);
void igh_hardware_valve_open_timer_service( void );
void idle_valve( void );
void open_valve( void );
void close_valve( void );

Timer water_flow_timer(ONE_SECOND, igh_hardware_litres_service);
Timer hourly_clock_timer( 60000, igh_hardware_hourly_clock ); // check every min
Timer hour_clock( 3600000, igh_hourly_irrigation_timer ); // check every hour



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
        // only action the button press after it's been released 
        if( igh_button_sec_counter >= FIVE_SECONDS )
        {
            // start or stop irrigation at any time with button
            button_irrigate = !button_irrigate;
            igh_button_sec_counter_to_send = FIVE_SECONDS;
        }
        else if( igh_button_sec_counter >= TWO_SECONDS &&
                igh_button_sec_counter < FIVE_SECONDS)
        {
            // hold the button down for two seconds to shut down or enable auto irrigation
            automatic_irrigation_mode = !automatic_irrigation_mode;
            igh_button_sec_counter_to_send = TWO_SECONDS;
        }
        igh_button_sec_counter = 0;
    } 
}

void igh_hardware_water_flow_setup( void )
{
    attach_flow_meter_interrupt();
    water_flow_timer.start();
    hourly_clock_timer.start();
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

    // water dispensed by sensors
    if( true == sensor_irrigation )
    {
        water_dispensed_automatically += flow_Liters;
    }
    else
    {
        water_dispensed_automatically = 0;
    }

    // keep track of valve open state
    igh_hardware_valve_open_timer_service();

    // manage water dispensed 
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
    else if( local_hour >= TWENTY_FOUR_HOURS )
    {
        local_hour -= TWENTY_FOUR_HOURS;
    }

    return (uint8_t)local_hour;
}

void igh_hardware_hourly_clock( void )
{
    current_hr = igh_get_local_time_hour();

    if( current_hr != previous_hr )
    {
        if( current_hr == igh_current_system_settings.irrigation_hr )
        {
            irrigation_params_str irrigation_parameters;
            // it is ok to irrigat if it hits the irrigation hour
            EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

            irrigation_parameters.irrigation_state = OK_TO_IRRIGATE;
            // update the flags
            EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

            irrigation_params_updated = false;

            Serial.println("IRRIGATION TIME STARTED");
        }
        else if( MIDNIGHT == current_hr )
        {
            // reset the system at midnight
            Serial.println("RESETTING WATER PARAMS");
            reset_irrigation_params();
        }

        previous_hr = current_hr;
    }

    // manage irrigation hour
    igh_hardware_manage_time_to_irrigate();
}

void igh_hardware_manage_time_to_irrigate( void )
{
    if( current_hr >= igh_current_system_settings.irrigation_hr &&
        ( true == Cellular.ready()) ) // only if we have valid time
    {
        // if we find ourselves here, there has been a reset, check the flag, 
        // if the flag is not set, start irrigation
        irrigation_params_str irrigation_parameters;
        EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

        if( MIN_AMOUNT_OF_WATER_WAS_DISPENSED != irrigation_parameters.min_amount_of_water_dispens_status &&
            OK_TO_IRRIGATE != irrigation_parameters.irrigation_state )
        {
            // say that it is ok to irrigate, the rest of the system should take care of it
            irrigation_parameters.irrigation_state = OK_TO_IRRIGATE;

            // update the flags
            EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

            // Serial.println("SYSTEM SET TO IRRIGATION OK TO CATCH UP");
        }
    }

    if( current_hr >= MIDNIGHT &&
        current_hr < igh_current_system_settings.irrigation_hr &&
        ( true == Cellular.ready()) )
    {
        // if we didn't have time at midnight, we might have missed resetting the system, 
        // fix that here
        irrigation_params_str irrigation_parameters;
        EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

        if( MIN_AMOUNT_OF_WATER_NOT_DISPENSED != irrigation_parameters.min_amount_of_water_dispens_status &&
            NOT_OK_TO_IRRIGATE != irrigation_parameters.irrigation_state )
        {
            // if these two flags were not set, lets set them and reset irrigation
            reset_irrigation_params();
        }

    }
}

void reset_irrigation_params( void )
{
    irrigation_params_str irrigation_parameters;

    irrigation_parameters.irrigation_state = NOT_OK_TO_IRRIGATE;
    irrigation_parameters.min_amount_of_water_dispens_status = MIN_AMOUNT_OF_WATER_NOT_DISPENSED;
    // update the flags
    EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

    total_water_dispensed_Liters = 0;

    irrigation_params_updated = false;
}

void igh_water_cap( void )
{
    if( total_water_dispensed_Liters >= (float)igh_current_threshold_settings.water_dispensed_period_high )
    {
        // close the valve if we ever exceed this upper water limit
        current_valve_position = VALVE_CLOSE;
    }
}

void igh_irrigiation_by_button_service( void )
{
    if( true == button_irrigate )
    {
        if( (water_dispensed_by_button < (float)igh_current_system_settings.water_amount_by_button_press) &&
            (valve_open_seconds_counter < igh_current_system_settings.water_dispenser_period) )
        {
            current_valve_position = VALVE_OPEN;
            // Serial.println("--------------------------------------------------> 1");
        }
        else
        {
            button_irrigate = false;
            current_valve_position = VALVE_CLOSE;
            // Serial.println("--------------------------------------------------> 2");
        }
    }
}

void igh_hourly_irrigation_timer( void )
{
    irrigate_by_the_hour = true;
}

void igh_hourly_irrigation( void )
{
    if( false == hour_clock.isActive() )
    {
        // start the clock if it hasnt yet been started
        hour_clock.start();
    }

    if( true == irrigate_by_the_hour )
    {
        // open the valve if one hour has passed
        current_valve_position = VALVE_OPEN;
        // set it false so we only open again after an hour
        sensor_irrigation = true;
    }
}

void igh_irrigation_by_sensor_data( void )
{
    if( VALID_SOIL_DATA == refreshed_soil_data )
    {
        if( (soil_humidity < igh_current_threshold_settings.soil_humidity_low) &&
            (soil_humidity < igh_current_threshold_settings.soil_humidity_high) )
        {
            // open the valave till we reach the sweet spot
            current_valve_position = VALVE_OPEN;
            sensor_irrigation = true;
        }
        else
        {
            /* close the valave if the sensor data received is not valid and if we are already in the sweet spot */
            current_valve_position = VALVE_CLOSE;
            sensor_irrigation = false;
        }
    }
}

void igh_automatic_irrigation_service( void )
{
    if( true == automatic_irrigation_mode )
    {
        irrigation_params_str irrigation_parameters;
        EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

        if( OK_TO_IRRIGATE != irrigation_parameters.irrigation_state )
        {
            // close the valve if it is not ok to irrigate
            current_valve_position = VALVE_CLOSE;
        }
        else
        {
            /* check if water was already dispensed */
            if( MIN_AMOUNT_OF_WATER_WAS_DISPENSED == irrigation_parameters.min_amount_of_water_dispens_status )
            {
                if( total_water_dispensed_Liters < (float)igh_current_threshold_settings.water_dispensed_period_low )
                {
                    // if this happens, a system reset must have happened or an overflow
                    // correct the total water dispensed
                    total_water_dispensed_Liters += (float)igh_current_threshold_settings.water_dispensed_period_low;
                }
                else
                {
                    /* do either timed irrigation or sensor driven irrigation */

                    // irrigate using sensor 
                    // igh_irrigation_by_sensor_data();
                    
                    // irrigate by the hour
                    igh_hourly_irrigation();

                    if( valve_open_seconds_counter >= igh_current_system_settings.water_dispenser_period ||
                        water_dispensed_automatically >= (float)igh_current_system_settings.water_amount_by_button_press )
                    {
                        // close the valve if the valve has been open too long
                        refreshed_soil_data = INVALID_SOIL_DATA;
                        irrigate_by_the_hour = false;
                        current_valve_position = VALVE_CLOSE;
                        sensor_irrigation = true;
                    }
                }
            }
            else 
            {
                /* dispense the minimum amount of water to start */
                if( total_water_dispensed_Liters < (float)igh_current_threshold_settings.water_dispensed_period_low )
                {
                    current_valve_position = VALVE_OPEN;
                }
                else
                {
                    irrigation_parameters.min_amount_of_water_dispens_status = MIN_AMOUNT_OF_WATER_WAS_DISPENSED;
                    EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
                    Serial.print("MIN WATER DISPENSED FLAG SET AFTER "); Serial.print(total_water_dispensed_Liters); Serial.println("L");
                    current_valve_position = VALVE_CLOSE;
                }
            }
        }
        // check the upper limit of water we can dispense. Close if we exceed
        igh_water_cap();
    }
}

void igh_hardware_water_management_service( void )
{
    igh_irrigiation_by_button_service();
    igh_automatic_irrigation_service();

    if( print_water_counte++ >= 10 )
    {
        irrigation_params_str irrigation_parameters;
        EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
        Serial.print("IRRIGATION STATE FLAG: "); Serial.println(irrigation_parameters.irrigation_state, HEX);
        Serial.print("WATER DISPENSED STATE FLAG: "); Serial.println(irrigation_parameters.min_amount_of_water_dispens_status, HEX);
        Serial.print("WATER DISPENSED: "); Serial.print(total_water_dispensed_Liters); Serial.println("L");
        Serial.print("VALVE ");
        if( VALVE_CLOSE == current_valve_position ){ Serial.println("CLOSED "); }
        else{ Serial.println("OPEN"); }
        print_water_counte = 0;
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

