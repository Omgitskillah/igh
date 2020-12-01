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
float water_dispensed_by_sensors = 0;
uint16_t soil_humidity = 0;
uint16_t soil_temperature = 0;
uint8_t  refreshed_soil_data = INVALID_SOIL_DATA;


// temp 
uint8_t hour_counter = 0;

bool automatic_irrigation_mode = true;

#define FLOW_METER_CAL_FACTOR  (4.5)
#define ONE_MIN                (60)
#define WATER_SCALING_FACTOR_L (270) // FLOW_METER_CAL_FACTOR * ONE_MIN

#define OK_TO_IRRIGATE                     (0x02020202)
#define NOT_OK_TO_IRRIGATE                 (0x00000000)

#define MIN_AMOUNT_OF_WATER_WAS_DISPENSED  (0x09ABCDEF)
#define MIN_AMOUNT_OF_WATER_NOT_DISPENSED  (0x00000000)

typedef struct _irrigation_params_str
{
    uint32_t irrigation_state;
    uint32_t min_amount_of_water_dispens_status;
}irrigation_params_str;

// time constants
#define TWENTY_FOUR_HOURS      (24)
#define MIDNIGHT               (0)
#define TWO_SECONDS            (2)
#define THREE_SECONDS          (3)
#define FIVE_SECONDS           (5)


// button global variables
uint8_t igh_button_sec_counter;
uint8_t igh_button_sec_counter_to_send = 0;
Timer igh_button_timer(ONE_SECOND, igh_boron_button_press_duration);
void igh_app_water_counter_callback( void );
void igh_hardware_water_flow_setup( void );
void igh_hardware_litres_service( void );
void detach_flow_meter_interrupt( void );
void attach_flow_meter_interrupt( void );
uint8_t igh_get_local_time_hour( void );
void igh_hardware_water_management_service( void );
void igh_hardware_manage_time_to_irrigate( void );

// local functions
static void igh_hardware_irrigiation_button_setup(void);
static void igh_hardware_onboard_led_setup(void);
static void igh_hardware_vlave_setup(void);
void igh_hardware_valve_open_timer_service( void );
void idle_valve( void );
void open_valve( void );
void close_valve( void );

irrigation_params_str irrigation_parameters;

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
    EEPROM.get( SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
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

    // water dispensed by sensors
    if( true == sensor_irrigation )
    {
        water_dispensed_by_sensors += flow_Liters;
    }
    else
    {
        water_dispensed_by_sensors = 0;
    }

    // keep track of valve open state
    igh_hardware_valve_open_timer_service();

    // manage irrigation hour
    igh_hardware_manage_time_to_irrigate();

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
    else if( local_hour > TWENTY_FOUR_HOURS )
    {
        local_hour -= TWENTY_FOUR_HOURS;
    }

    return (uint8_t)local_hour;
}

void igh_hardware_manage_time_to_irrigate( void )
{
    current_hr = igh_get_local_time_hour();

    if( current_hr != previous_hr )
    {
        if( current_hr == igh_current_system_settings.irrigation_hr )
        {
            // it is ok to irrigat if it hits the irrigation hour
            irrigation_parameters.irrigation_state = OK_TO_IRRIGATE;
            // update the flags
            EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);

            time_t time = Time.now();
            Serial.print("IRRIGATION TIME STARTED, FLAG SET: ");
            Serial.println( Time.format(time, TIME_FORMAT_DEFAULT) );
        }
        else if( MIDNIGHT == current_hr )
        {
            // reset the system at midnight
            reset_irrigation_params();

            total_water_dispensed_Liters = 0;
            
            time_t time = Time.now();
            Serial.print("RESETTING WATER PARAMS: ");
            Serial.println( Time.format(time, TIME_FORMAT_DEFAULT) );
        }

        previous_hr = current_hr;
    }
}

void reset_irrigation_params( void )
{
    irrigation_parameters.irrigation_state = NOT_OK_TO_IRRIGATE;
    irrigation_parameters.min_amount_of_water_dispens_status = MIN_AMOUNT_OF_WATER_NOT_DISPENSED;
    // update the flags
    EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
}

void igh_hardware_water_management_service( void )
{
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
        if( OK_TO_IRRIGATE == irrigation_parameters.irrigation_state &&
            total_water_dispensed_Liters < (float)igh_current_threshold_settings.water_dispensed_period_high &&
            true == automatic_irrigation_mode )
        {
            if( MIN_AMOUNT_OF_WATER_WAS_DISPENSED != irrigation_parameters.min_amount_of_water_dispens_status )
            {
                // if we have not dispensed all the water needed in the morning, open the valve till we do
                current_valve_position = VALVE_OPEN;
                if( total_water_dispensed_Liters >= igh_current_threshold_settings.water_dispensed_period_low )
                {
                    // if we have now dispensed all the water, set the flag the close the valve
                    irrigation_parameters.min_amount_of_water_dispens_status = MIN_AMOUNT_OF_WATER_WAS_DISPENSED;
                    EEPROM.put(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
                    Serial.print("MIN WATER DISPENSED FLAG SET AFTER "); Serial.print(total_water_dispensed_Liters); Serial.println("L");
                    // then close the valve
                    current_valve_position = VALVE_CLOSE;
                }
            }
            else
            {
                // if all the water was dispensed in the morning, start using sensor data to maintain the valve
                if( VALID_SOIL_DATA == refreshed_soil_data )
                {
                    /**
                     * run the irrigation if we are within the dispensor window
                     * and if we have valid sensor data
                     * */
                    if( (soil_humidity < igh_current_threshold_settings.soil_humidity_low) &&
                        (soil_humidity < igh_current_threshold_settings.soil_humidity_high) &&
                        (water_dispensed_by_sensors < (float)igh_current_system_settings.water_amount_by_button_press) &&
                        (valve_open_seconds_counter < igh_current_system_settings.water_dispenser_period) )
                    {
                        /**
                         * Dispense water only if
                         * 1. the soil moisture is too low
                         * 2. The soil moisture is not too high
                         * 3. We have not dispensed too much water 
                         * */
                        sensor_irrigation = true;
                        current_valve_position = VALVE_OPEN;
                    }
                    else
                    {
                        /**
                         * close valve if even one of these conditions is not met
                         * 1. That is, if we have dispensed enough water till the next time we get a reading,
                         * 2. If the sensor readings are outside the irrigation window
                         * */
                        refreshed_soil_data = INVALID_SOIL_DATA;
                        sensor_irrigation = false;
                        current_valve_position = VALVE_CLOSE;
                    }
                }
            }
        }
        else
        {
            // close the valve if it is not ok to irrigate
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

