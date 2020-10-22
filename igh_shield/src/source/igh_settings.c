/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/igh_message.h"
#include "include/igh_settings.h"
#include "include/igh_default_settings.h"

#define PAYLOAD_LEN_INDEX (PAYLOAD_INDEX + 1)
#define FIRST_TUPLE_INDEX (PAYLOAD_LEN_INDEX + 1)

char boron_serial_number[BORON_SN_LEN];
uint8_t deviceID_string[24];

device_op_state current_op_state;
device_op_state previous_op_state;

thresholds igh_default_thresholds;
system_settings igh_default_system_settings;
uint8_t default_serial_number[] = DEFAULT_SERIAL_NUMBER;
uint8_t default_broker_url[] = DEFAULT_MQTT_BROKER;

thresholds igh_current_threshold_settings;
system_settings igh_current_system_settings;
valve_position current_valve_position;

// flags to re-initialize various modules in case of settings change
uint8_t initialize_rfm69 = 0;
uint8_t mqtt_set_broker = 1; // make sure we set the broker on init
uint8_t new_settings_available = 0;


// functions
#ifndef TEST
LOCAL void igh_settings_get_defaults(void);
LOCAL uint8_t igh_settings_remote_valvle_control(uint8_t * settings);
LOCAL uint8_t igh_settings_build_settings_request_payload(uint8_t * settings_req, uint8_t * buffer, uint8_t start_index);
#endif


LOCAL void igh_settings_get_defaults(void) // Total bytes 
{
    // System settings
    memset(&igh_default_system_settings, 0, sizeof(igh_default_system_settings));
    igh_default_system_settings.timezone                    = DEFAULT_TIMEZONE;
    igh_default_system_settings.irrigation_hr               = DEFAULT_IRRIGATION_HR;
    igh_default_system_settings.op_state                    = DEFAULT_NEW_OPSTATE;
    igh_default_system_settings.reporting_interval          = DEFAULT_REPORTING_INTERVAL;
    igh_default_system_settings.data_resolution             = DEFAULT_DATA_RESOLUTION;
    memcpy(igh_default_system_settings.serial_number, default_serial_number, LENGTH_SUBID_SET_SERIAL_NUMBER);
    memcpy(igh_default_system_settings.broker, default_broker_url, sizeof(default_broker_url));
    igh_default_system_settings.broker_port                 = DEFAULT_MQTT_BROKER_PORT;
    igh_default_system_settings.checksum = igh_settings_calculate_checksum(&igh_default_system_settings, sizeof(igh_default_system_settings));

    //High Threshold tirggers
    memset(&igh_default_thresholds, 0, sizeof(igh_default_thresholds));
    igh_default_thresholds.soil_moisture_low                = DEFAULT_SOIL_MOISTURE_LOW;                   
    igh_default_thresholds.air_humidity_low                 = DEFAULT_AIR_HUMIDITY_LOW;                     
    igh_default_thresholds.soil_humidity_low                = DEFAULT_SOIL_HUMIDITY_LOW;                   
    igh_default_thresholds.carbon_dioxide_low               = DEFAULT_CARBON_DIOXIDE_LOW;        
    igh_default_thresholds.air_temperature_low              = DEFAULT_AIR_TEMPERATURE_LOW;               
    igh_default_thresholds.soil_temperature_low             = DEFAULT_SOIL_TEMPERATURE_LOW;             
    igh_default_thresholds.soil_npk_low                     = DEFAULT_SOIL_NPK_LOW;                             
    igh_default_thresholds.light_intensity_low              = DEFAULT_LIGHT_INTENSITY_LOW;               
    igh_default_thresholds.shield_battery_level_low         = DEFAULT_SHIELD_BATTERY_LEVEL_LOW;     
    igh_default_thresholds.spear_battery_level_low          = DEFAULT_SPEAR_BATTERY_LEVEL_LOW;      
    igh_default_thresholds.water_dispensed_period_low       = DEFAULT_WATER_DISPENSED_PERIOD_LOW;
    // Low Threshold Trigger
    igh_default_thresholds.soil_moisture_high               = DEFAULT_SOIL_MOISTURE_HIGH;                 
    igh_default_thresholds.air_humidity_high                = DEFAULT_AIR_HUMIDITY_HIGH;                   
    igh_default_thresholds.soil_humidity_high               = DEFAULT_SOIL_HUMIDITY_HIGH;                 
    igh_default_thresholds.carbon_dioxide_high              = DEFAULT_CARBON_DIOXIDE_HIGH;        
    igh_default_thresholds.air_temperature_high             = DEFAULT_AIR_TEMPERATURE_HIGH;             
    igh_default_thresholds.soil_temperature_high            = DEFAULT_SOIL_TEMPERATURE_HIGH;           
    igh_default_thresholds.soil_npk_high                    = DEFAULT_SOIL_NPK_HIGH;                           
    igh_default_thresholds.light_intensity_high             = DEFAULT_LIGHT_INTENSITY_HIGH;             
    igh_default_thresholds.shield_battery_level_high        = DEFAULT_SHIELD_BATTERY_LEVEL_HIGH;   
    igh_default_thresholds.spear_battery_level_high         = DEFAULT_SPEAR_BATTERY_LEVEL_HIGH;     
    igh_default_thresholds.water_dispensed_period_high      = DEFAULT_WATER_DISPENSED_PERIOD_HIGH;
    igh_default_thresholds.checksum = igh_settings_calculate_checksum(&igh_default_thresholds, sizeof(igh_default_thresholds));

} 

uint8_t igh_settings_process_settings_tuples( uint8_t * settings, uint8_t byte_tracker, uint8_t end_index )
{
    //cycle through tuples to get the settings data
    uint8_t current_tuple_id;
    uint8_t current_tuple_length;
    uint8_t current_data_index;

    while(byte_tracker < end_index)
    {
        // extract tuples
        current_tuple_id = settings[byte_tracker];
        // extract the length
        current_tuple_length = settings[byte_tracker + 1]; // should always follow
        // extract the tuple data based on tuple id
        current_data_index = byte_tracker + 2;

        switch(current_tuple_id)
        {
            case SUBID_OPSTATE:
                if(LENGTH_SUBID_OPSTATE == current_tuple_length)
                {
                    // check if new state is valid
                    if( (OP_INACTIVE == (settings[current_data_index]))|| 
                        (OP_BASIC == (settings[current_data_index]))||
                        (OP_STANDARD == (settings[current_data_index]))||
                        (OP_PREMIUM == (settings[current_data_index]))  
                    )
                    {
                        igh_current_system_settings.op_state = (device_op_state)settings[current_data_index];
                    }  
                    else
                    {
                        // do nothing
                    }
                    
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_TIMEZONE:
                if(LENGTH_SUBID_SUBID_TIMEZONE == current_tuple_length)
                {
                    // check if new timezone is valid
                    if( POSITIVE_TIME_ZONE == settings[current_data_index] )
                    {
                        if( MAX_TIMEZONE > settings[current_data_index + 1] )
                        {
                            igh_current_system_settings.timezone = (int)settings[current_data_index + 1];
                        }
                    }

                    else if( NEGATIVE_TIME_ZONE == settings[current_data_index] )
                    {
                        if( MAX_TIMEZONE > settings[current_data_index + 1] )
                        {
                            igh_current_system_settings.timezone = -1 * settings[current_data_index + 1];
                        }
                    }
                    
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_IRRIGATION_HR:
                if(LENGTH_SUBID_SUBID_IRRIGATION_HR == current_tuple_length)
                {
                    // check if the time is within 24 hours
                    if( MAX_HOUR >  (settings[current_data_index]) ||
                        MIN_HOUR <= (settings[current_data_index]))
                    {
                        igh_current_system_settings.irrigation_hr = settings[current_data_index];
                    }  
                    else
                    {
                        // do nothing
                    }                   
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_SET_SERIAL_NUMBER:
                if(LENGTH_SUBID_SET_SERIAL_NUMBER == current_tuple_length)
                {
                    memcpy(igh_current_system_settings.serial_number, &settings[current_data_index], LENGTH_SUBID_SET_SERIAL_NUMBER);
                    initialize_rfm69 = 1;
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_REPORTING_INTERVAL:
                if(LENGTH_SUBID_REPORTING_INTERVAL == current_tuple_length)
                {
                    uint8_t new_reporting_interval[LENGTH_SUBID_REPORTING_INTERVAL]; 
                    memcpy(new_reporting_interval, &settings[current_data_index], LENGTH_SUBID_REPORTING_INTERVAL);
                    igh_current_system_settings.reporting_interval = GET32(new_reporting_interval);
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_DATA_RESOLUTION:
                if(LENGTH_SUBID_DATA_RESOLUTION == current_tuple_length)
                {
                    uint8_t new_data_resolution[LENGTH_SUBID_DATA_RESOLUTION]; 
                    memcpy(new_data_resolution, &settings[current_data_index], LENGTH_SUBID_DATA_RESOLUTION);
                    igh_current_system_settings.data_resolution = GET32(new_data_resolution);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_MQTT_BROKER:
                if( sizeof(igh_current_system_settings.broker) > current_tuple_length)
                {
                    memset(igh_current_system_settings.broker, '\0', sizeof(igh_current_system_settings.broker));
                    memcpy(igh_current_system_settings.broker, &settings[current_data_index], current_tuple_length);
                    igh_current_system_settings.broker[current_tuple_length] = '\0'; // terminate the string
                    mqtt_set_broker = 1;
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_MQTT_BROKER_PORT:
                if(LENGTH_SUBID_MQTT_PORT == current_tuple_length)
                {
                    uint8_t new_mqtt_port[LENGTH_SUBID_MQTT_PORT]; 
                    memcpy(new_mqtt_port, &settings[current_data_index], LENGTH_SUBID_MQTT_PORT);
                    igh_current_system_settings.broker_port = GET16(new_mqtt_port);
                    mqtt_set_broker = 1;
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_SOIL_MOISTURE_LOW:
                if(LENGTH_SUBID_SOIL_MOISTURE_LOW == current_tuple_length)
                {
                    uint8_t new_moisture_low_th[LENGTH_SUBID_SOIL_MOISTURE_LOW]; 
                    memcpy(new_moisture_low_th, &settings[current_data_index], LENGTH_SUBID_SOIL_MOISTURE_LOW);
                    igh_current_threshold_settings.soil_moisture_low = GET16(new_moisture_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_SOIL_MOISTURE_HIGH:
                if(LENGTH_SUBID_SOIL_MOISTURE_HIGH == current_tuple_length)
                {
                    uint8_t new_moisture_high_th[LENGTH_SUBID_SOIL_MOISTURE_HIGH]; 
                    memcpy(new_moisture_high_th, &settings[current_data_index], LENGTH_SUBID_SOIL_MOISTURE_LOW);
                    igh_current_threshold_settings.soil_moisture_high = GET16(new_moisture_high_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_AIR_HUMIDITY_LOW:
                if(LENGTH_SUBID_AIR_HUMIDITY_LOW  == current_tuple_length)
                {
                    uint8_t new_air_humidity_low_th[LENGTH_SUBID_AIR_HUMIDITY_LOW]; 
                    memcpy(new_air_humidity_low_th, &settings[current_data_index], LENGTH_SUBID_AIR_HUMIDITY_LOW);
                    igh_current_threshold_settings.air_humidity_low = GET16(new_air_humidity_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_AIR_HUMIDITY_HIGH:
                if(LENGTH_SUBID_AIR_HUMIDITY_HIGH  == current_tuple_length)
                {
                    uint8_t new_air_humidity_high_th[LENGTH_SUBID_AIR_HUMIDITY_HIGH]; 
                    memcpy(new_air_humidity_high_th, &settings[current_data_index], LENGTH_SUBID_AIR_HUMIDITY_HIGH);
                    igh_current_threshold_settings.air_humidity_high = GET16(new_air_humidity_high_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_SOIL_HUMIDITY_LOW:
                if(LENGTH_SUBID_SOIL_HUMIDITY_LOW  == current_tuple_length)
                {
                    uint8_t new_soil_humidity_low_th[LENGTH_SUBID_SOIL_HUMIDITY_LOW]; 
                    memcpy(new_soil_humidity_low_th, &settings[current_data_index], LENGTH_SUBID_SOIL_HUMIDITY_LOW);
                    igh_current_threshold_settings.soil_humidity_low = GET16(new_soil_humidity_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_SOIL_HUMIDITY_HIGH:
                if(LENGTH_SUBID_SOIL_HUMIDITY_HIGH  == current_tuple_length)
                {
                    uint8_t new_soil_humidity_high_th[LENGTH_SUBID_SOIL_HUMIDITY_HIGH]; 
                    memcpy(new_soil_humidity_high_th, &settings[current_data_index], LENGTH_SUBID_SOIL_HUMIDITY_HIGH);
                    igh_current_threshold_settings.soil_humidity_high = GET16(new_soil_humidity_high_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_CARBON_DIOXIDE_LOW:
                if(LENGTH_SUBID_CARBON_DIOXIDE_LOW  == current_tuple_length)
                {
                    uint8_t new_carbon_dioxide_low_th[LENGTH_SUBID_CARBON_DIOXIDE_LOW]; 
                    memcpy(new_carbon_dioxide_low_th, &settings[current_data_index], LENGTH_SUBID_CARBON_DIOXIDE_LOW);
                    igh_current_threshold_settings.carbon_dioxide_low = GET16(new_carbon_dioxide_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_CARBON_DIOXIDE_HIGH:
                if(LENGTH_SUBID_CARBON_DIOXIDE_HIGH  == current_tuple_length)
                {
                    uint8_t new_carbon_dioxide_high_th[LENGTH_SUBID_CARBON_DIOXIDE_HIGH]; 
                    memcpy(new_carbon_dioxide_high_th, &settings[current_data_index], LENGTH_SUBID_CARBON_DIOXIDE_HIGH);
                    igh_current_threshold_settings.carbon_dioxide_high = GET16(new_carbon_dioxide_high_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_AIR_TEMPERATURE_LOW:
                if(LENGTH_SUBID_AIR_TEMPERATURE_LOW  == current_tuple_length)
                {
                    uint8_t new_air_temperature_low_th[LENGTH_SUBID_AIR_TEMPERATURE_LOW]; 
                    memcpy(new_air_temperature_low_th, &settings[current_data_index], LENGTH_SUBID_AIR_TEMPERATURE_LOW);
                    igh_current_threshold_settings.air_temperature_low = GET16(new_air_temperature_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_AIR_TEMPERATURE_HIGH:
                if(LENGTH_SUBID_AIR_TEMPERATURE_HIGH  == current_tuple_length)
                {
                    uint8_t new_air_temperature_high_th[LENGTH_SUBID_AIR_TEMPERATURE_HIGH]; 
                    memcpy(new_air_temperature_high_th, &settings[current_data_index], LENGTH_SUBID_AIR_TEMPERATURE_HIGH);
                    igh_current_threshold_settings.air_temperature_high = GET16(new_air_temperature_high_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_SOIL_TEMPERATURE_LOW:
                if(LENGTH_SUBID_SOIL_TEMPERATURE_LOW  == current_tuple_length)
                {
                    uint8_t new_soil_temperature_low_th[LENGTH_SUBID_SOIL_TEMPERATURE_LOW]; 
                    memcpy(new_soil_temperature_low_th, &settings[current_data_index], LENGTH_SUBID_SOIL_TEMPERATURE_LOW);
                    igh_current_threshold_settings.soil_temperature_low = GET16(new_soil_temperature_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_SOIL_TEMPERATURE_HIGH:
                if(LENGTH_SUBID_SOIL_TEMPERATURE_HIGH  == current_tuple_length)
                {
                    uint8_t new_soil_temperature_high_th[LENGTH_SUBID_SOIL_TEMPERATURE_HIGH]; 
                    memcpy(new_soil_temperature_high_th, &settings[current_data_index], LENGTH_SUBID_SOIL_TEMPERATURE_HIGH);
                    igh_current_threshold_settings.soil_temperature_high = GET16(new_soil_temperature_high_th);
                }
                else
                {
                    return 0;
                }
                break;    

            case SUBID_SOIL_NPK_HIGH:
                if(LENGTH_SUBID_SOIL_NPK_HIGH  == current_tuple_length)
                {
                    uint8_t new_soil_npk_high_th[LENGTH_SUBID_SOIL_NPK_HIGH]; 
                    memcpy(new_soil_npk_high_th, &settings[current_data_index], LENGTH_SUBID_SOIL_NPK_HIGH);
                    igh_current_threshold_settings.soil_npk_high = GET16(new_soil_npk_high_th);
                }
                else
                {
                    return 0;
                }
                break;   

            case SUBID_SOIL_NPK_LOW:
                if(LENGTH_SUBID_SOIL_NPK_LOW  == current_tuple_length)
                {
                    uint8_t new_soil_npk_low_th[LENGTH_SUBID_SOIL_NPK_LOW]; 
                    memcpy(new_soil_npk_low_th, &settings[current_data_index], LENGTH_SUBID_SOIL_NPK_LOW);
                    igh_current_threshold_settings.soil_npk_low = GET16(new_soil_npk_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_LIGHT_INTENSITY_LOW:
                if(LENGTH_SUBID_LIGHT_INTENSITY_LOW  == current_tuple_length)
                {
                    uint8_t new_light_intensity_low_th[LENGTH_SUBID_LIGHT_INTENSITY_LOW]; 
                    memcpy(new_light_intensity_low_th, &settings[current_data_index], LENGTH_SUBID_LIGHT_INTENSITY_LOW);
                    igh_current_threshold_settings.light_intensity_low = GET16(new_light_intensity_low_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_LIGHT_INTENSITY_HIGH:
                if(LENGTH_SUBID_LIGHT_INTENSITY_HIGH  == current_tuple_length)
                {
                    uint8_t new_light_intensity_high_th[LENGTH_SUBID_LIGHT_INTENSITY_HIGH]; 
                    memcpy(new_light_intensity_high_th, &settings[current_data_index], LENGTH_SUBID_LIGHT_INTENSITY_HIGH);
                    igh_current_threshold_settings.light_intensity_high = GET16(new_light_intensity_high_th);
                }
                else
                {
                    return 0;
                }
                break; 

            case SUBID_SHIELD_BATTERY_LEVEL_LOW:
                if(LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW  == current_tuple_length)
                {
                    uint8_t new_shield_battery_low_th[LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW]; 
                    memcpy(new_shield_battery_low_th, &settings[current_data_index], LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW);
                    igh_current_threshold_settings.shield_battery_level_low = GET16(new_shield_battery_low_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_SHIELD_BATTERY_LEVEL_HIGH:
                if(LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH  == current_tuple_length)
                {
                    uint8_t new_shield_battery_high_th[LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH]; 
                    memcpy(new_shield_battery_high_th, &settings[current_data_index], LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH);
                    igh_current_threshold_settings.shield_battery_level_high = GET16(new_shield_battery_high_th);
                }
                else
                {
                    return 0;
                }
                break;  

            case SUBID_SPEAR_BATTERY_LEVEL_LOW:
                if(LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW  == current_tuple_length)
                {
                    uint8_t new_spear_battery_low_th[LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW]; 
                    memcpy(new_spear_battery_low_th, &settings[current_data_index], LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW);
                    igh_current_threshold_settings.spear_battery_level_low = GET16(new_spear_battery_low_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_SPEAR_BATTERY_LEVEL_HIGH:
                if(LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH  == current_tuple_length)
                {
                    uint8_t new_spear_battery_high_th[LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH]; 
                    memcpy(new_spear_battery_high_th, &settings[current_data_index], LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH);
                    igh_current_threshold_settings.spear_battery_level_high = GET16(new_spear_battery_high_th);
                }
                else
                {
                    return 0;
                }
                break;

            case SUBID_WATER_DISPENSED_PERIOD_LOW:
                if(LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW == current_tuple_length)
                {
                    uint8_t new_water_dispensed_period_low[LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW]; 
                    memcpy(new_water_dispensed_period_low, &settings[current_data_index], LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW);
                    igh_current_threshold_settings.water_dispensed_period_low = GET32(new_water_dispensed_period_low);
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break;

            case SUBID_WATER_DISPENSED_PERIOD_HIGH:
                if(LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH == current_tuple_length)
                {
                    uint8_t new_water_dispensed_period_high[LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH]; 
                    memcpy(new_water_dispensed_period_high, &settings[current_data_index], LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH);
                    igh_current_threshold_settings.water_dispensed_period_high = GET32(new_water_dispensed_period_high);
                }
                else
                {
                    // stop processing any more settings as they may be corrupt
                    return 0;
                }
                break; 

            default: // nothing to do hear
                break;
        }
        // move index to next tuple id
        byte_tracker += current_tuple_length + TUPLE_HEADER_LEN;
    }
    return 1;
}

uint8_t igh_settings_parse_new_settings(uint8_t * settings)
{
    // get the length
    uint8_t length = settings[PAYLOAD_LEN_INDEX]; // byte at position one should always be length
    // guard the process
    if(0 >= length)
    {
        return 0; // do not allow settings with zero payload to be processed
    }
    // get index of last byte to process
    uint8_t settings_end_index = FIRST_TUPLE_INDEX + length;
    // get the location of first tuple
    uint8_t settings_byte_tracker = FIRST_TUPLE_INDEX;
    
    if( 0 == igh_settings_process_settings_tuples( settings, settings_byte_tracker, settings_end_index ) ) 
    {
        return 0;
    }
    else
    {
        // update the checksum of the system settings
        igh_current_system_settings.checksum = igh_settings_calculate_checksum(&igh_current_system_settings, sizeof(igh_current_system_settings));
        // update the checksum for the threshold settings
        igh_current_threshold_settings.checksum = igh_settings_calculate_checksum(&igh_current_threshold_settings, sizeof(igh_current_threshold_settings));
        
        new_settings_available = 1;
    }
    
    return 1;
}

void igh_settings_reset_system_to_default(void)
{
    igh_settings_get_defaults();
    igh_current_system_settings = igh_default_system_settings;
    igh_current_threshold_settings = igh_default_thresholds;
    // update the checksum of the system settings
    igh_current_system_settings.checksum = igh_settings_calculate_checksum(&igh_current_system_settings, sizeof(igh_current_system_settings));
    // update the checksum for the threshold settings
    igh_current_threshold_settings.checksum = igh_settings_calculate_checksum(&igh_current_threshold_settings, sizeof(igh_current_threshold_settings));
}

// payload build settings for sending

LOCAL uint8_t igh_settings_build_settings_request_payload(uint8_t * settings_req, uint8_t * buffer, uint8_t start_index)
{
    uint8_t buffer_index_tracker = start_index;
    uint8_t settings_request_tracker = FIRST_TUPLE_INDEX;
    // // get the number of all requested settings
    uint8_t length = settings_req[PAYLOAD_LEN_INDEX]; // byte at position one should always be length
    uint8_t end_index = FIRST_TUPLE_INDEX + length;
    // // check that the request is valid
    if ( length <= 0)
    {
        return buffer_index_tracker;
    }

    uint8_t current_settings_request;
    // start processing the requested settings here
    buffer[buffer_index_tracker++] = IGH_READ_SETTINGS;
    buffer[buffer_index_tracker++] = 0x00; // preserve space for length
    while(settings_request_tracker < end_index)
    {
        current_settings_request = settings_req[settings_request_tracker];
        switch(current_settings_request)
        {
            case SUBID_OPSTATE:
                buffer[buffer_index_tracker++] = SUBID_OPSTATE;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_OPSTATE;
                buffer[buffer_index_tracker++] = igh_current_system_settings.op_state;
                break;

            case SUBID_REPORTING_INTERVAL:
                buffer[buffer_index_tracker++] = SUBID_REPORTING_INTERVAL;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_REPORTING_INTERVAL;
                PUT32(igh_current_system_settings.reporting_interval, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_REPORTING_INTERVAL;
                break;

            case SUBID_DATA_RESOLUTION:
                buffer[buffer_index_tracker++] = SUBID_DATA_RESOLUTION;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_DATA_RESOLUTION;
                PUT32(igh_current_system_settings.data_resolution, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_DATA_RESOLUTION;
                break;

            case SUBID_SET_SERIAL_NUMBER:
                buffer[buffer_index_tracker++] = SUBID_SET_SERIAL_NUMBER;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SET_SERIAL_NUMBER;
                memcpy(&buffer[buffer_index_tracker], igh_current_system_settings.serial_number, LENGTH_SUBID_SET_SERIAL_NUMBER);
                buffer_index_tracker += LENGTH_SUBID_SET_SERIAL_NUMBER;
                break;
            
            case SUBID_MQTT_BROKER:
                buffer[buffer_index_tracker++] = SUBID_MQTT_BROKER;
                uint8_t _len = 0;
                for( _len; _len < sizeof(igh_current_system_settings.broker); _len++)
                {
                    if(igh_current_system_settings.broker[_len] == '\0')
                    {
                        break;
                    }
                    else
                    {
                        // run through the buffer till you find the first null byte or wait till the end
                    }  
                } // new _len will be the length of valid string in buffer
                buffer[buffer_index_tracker++] = _len;
                memcpy(&buffer[buffer_index_tracker], igh_current_system_settings.broker, _len);
                buffer_index_tracker += _len;
                break;

            case SUBID_MQTT_BROKER_PORT: 
                buffer[buffer_index_tracker++] = SUBID_MQTT_BROKER_PORT;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_MQTT_PORT;
                PUT16(igh_current_system_settings.broker_port, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_MQTT_PORT;
                break;

            case SUBID_SOIL_MOISTURE_LOW: 
                buffer[buffer_index_tracker++] = SUBID_SOIL_MOISTURE_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_MOISTURE_LOW;
                PUT16(igh_current_threshold_settings.soil_moisture_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_MOISTURE_LOW;
                break;

            case SUBID_AIR_HUMIDITY_LOW:
                buffer[buffer_index_tracker++] = SUBID_AIR_HUMIDITY_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_AIR_HUMIDITY_LOW;
                PUT16(igh_current_threshold_settings.air_humidity_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_AIR_HUMIDITY_LOW;           
                break;

            case SUBID_SOIL_HUMIDITY_LOW:
                buffer[buffer_index_tracker++] = SUBID_SOIL_HUMIDITY_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_HUMIDITY_LOW;
                PUT16(igh_current_threshold_settings.soil_humidity_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_HUMIDITY_LOW;                 
                break;

            case SUBID_CARBON_DIOXIDE_LOW:
                buffer[buffer_index_tracker++] = SUBID_CARBON_DIOXIDE_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_CARBON_DIOXIDE_LOW;
                PUT16(igh_current_threshold_settings.carbon_dioxide_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_CARBON_DIOXIDE_LOW;  
                break;

            case SUBID_AIR_TEMPERATURE_LOW:        
                buffer[buffer_index_tracker++] = SUBID_AIR_TEMPERATURE_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_AIR_TEMPERATURE_LOW;
                PUT16(igh_current_threshold_settings.air_temperature_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_AIR_TEMPERATURE_LOW;  
                break;

            case SUBID_SOIL_TEMPERATURE_LOW:       
                buffer[buffer_index_tracker++] = SUBID_SOIL_TEMPERATURE_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_TEMPERATURE_LOW;
                PUT16(igh_current_threshold_settings.soil_temperature_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_TEMPERATURE_LOW;  
                break;

            case SUBID_SOIL_NPK_LOW:               
                buffer[buffer_index_tracker++] = SUBID_SOIL_NPK_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_NPK_LOW;
                PUT16(igh_current_threshold_settings.soil_npk_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_NPK_LOW;  
                break;

            case SUBID_LIGHT_INTENSITY_LOW:        
                buffer[buffer_index_tracker++] = SUBID_LIGHT_INTENSITY_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_LIGHT_INTENSITY_LOW;
                PUT16(igh_current_threshold_settings.light_intensity_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_LIGHT_INTENSITY_LOW;  
                break;

            case SUBID_SHIELD_BATTERY_LEVEL_LOW:   
                buffer[buffer_index_tracker++] = SUBID_SHIELD_BATTERY_LEVEL_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW;
                PUT16(igh_current_threshold_settings.shield_battery_level_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW;  
                break;

            case SUBID_SPEAR_BATTERY_LEVEL_LOW:   
                buffer[buffer_index_tracker++] = SUBID_SPEAR_BATTERY_LEVEL_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW;
                PUT16(igh_current_threshold_settings.spear_battery_level_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW;  
                break;

            case SUBID_WATER_DISPENSED_PERIOD_LOW:
                buffer[buffer_index_tracker++] = SUBID_WATER_DISPENSED_PERIOD_LOW;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW;
                PUT32(igh_current_threshold_settings.water_dispensed_period_low, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW;  
                break;

            case SUBID_SOIL_MOISTURE_HIGH: 
                buffer[buffer_index_tracker++] = SUBID_SOIL_MOISTURE_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_MOISTURE_HIGH;
                PUT16(igh_current_threshold_settings.soil_moisture_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_MOISTURE_HIGH;
                break;

            case SUBID_AIR_HUMIDITY_HIGH:
                buffer[buffer_index_tracker++] = SUBID_AIR_HUMIDITY_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_AIR_HUMIDITY_HIGH;
                PUT16(igh_current_threshold_settings.air_humidity_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_AIR_HUMIDITY_HIGH;           
                break;

            case SUBID_SOIL_HUMIDITY_HIGH:
                buffer[buffer_index_tracker++] = SUBID_SOIL_HUMIDITY_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_HUMIDITY_HIGH;
                PUT16(igh_current_threshold_settings.soil_humidity_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_HUMIDITY_HIGH;                 
                break;

            case SUBID_CARBON_DIOXIDE_HIGH:
                buffer[buffer_index_tracker++] = SUBID_CARBON_DIOXIDE_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_CARBON_DIOXIDE_HIGH;
                PUT16(igh_current_threshold_settings.carbon_dioxide_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_CARBON_DIOXIDE_HIGH;  
                break;

            case SUBID_AIR_TEMPERATURE_HIGH:        
                buffer[buffer_index_tracker++] = SUBID_AIR_TEMPERATURE_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_AIR_TEMPERATURE_HIGH;
                PUT16(igh_current_threshold_settings.air_temperature_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_AIR_TEMPERATURE_HIGH;  
                break;

            case SUBID_SOIL_TEMPERATURE_HIGH:       
                buffer[buffer_index_tracker++] = SUBID_SOIL_TEMPERATURE_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_TEMPERATURE_HIGH;
                PUT16(igh_current_threshold_settings.soil_temperature_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_TEMPERATURE_HIGH;  
                break;

            case SUBID_SOIL_NPK_HIGH:               
                buffer[buffer_index_tracker++] = SUBID_SOIL_NPK_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SOIL_NPK_HIGH;
                PUT16(igh_current_threshold_settings.soil_npk_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SOIL_NPK_HIGH;  
                break;

            case SUBID_LIGHT_INTENSITY_HIGH:        
                buffer[buffer_index_tracker++] = SUBID_LIGHT_INTENSITY_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_LIGHT_INTENSITY_HIGH;
                PUT16(igh_current_threshold_settings.light_intensity_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_LIGHT_INTENSITY_HIGH;  
                break;

            case SUBID_SHIELD_BATTERY_LEVEL_HIGH:   
                buffer[buffer_index_tracker++] = SUBID_SHIELD_BATTERY_LEVEL_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH;
                PUT16(igh_current_threshold_settings.shield_battery_level_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH;  
                break;

            case SUBID_SPEAR_BATTERY_LEVEL_HIGH:   
                buffer[buffer_index_tracker++] = SUBID_SPEAR_BATTERY_LEVEL_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH;
                PUT16(igh_current_threshold_settings.spear_battery_level_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH;  
                break;

            case SUBID_WATER_DISPENSED_PERIOD_HIGH:
                buffer[buffer_index_tracker++] = SUBID_WATER_DISPENSED_PERIOD_HIGH;
                buffer[buffer_index_tracker++] = LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH;
                PUT32(igh_current_threshold_settings.water_dispensed_period_high, &buffer[buffer_index_tracker]);
                buffer_index_tracker += LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH;  
                break;

            default:
            // do nothing
                break;
        }
        settings_request_tracker++;
    }

    uint8_t setting_payload_len = (buffer_index_tracker - start_index) - SIZE_OF_HEADER;
    buffer[start_index + 1] = setting_payload_len;
    
    return buffer_index_tracker;
}

LOCAL uint8_t igh_settings_remote_valvle_control(uint8_t * settings)
{
    if( ((valve_position)settings[FIRST_TUPLE_INDEX] == VALVE_CLOSE) ||
        ((valve_position)settings[FIRST_TUPLE_INDEX] == VALVE_OPEN) )
    {
        current_valve_position =  (valve_position)settings[FIRST_TUPLE_INDEX];
        return 1;
    }

    return 0;
}

/*******************************************************************************
 * igh_settings_process_settings
 *******************************************************************************/
/**
 * \brief Process valve control message or settings message
 * \param settings the whole incoming message array
 * \return true or false based on processing success
 */
uint8_t igh_settings_process_settings(uint8_t * settings)
{
    uint8_t ret = 0; 
    igh_pkt_id settings_type = (igh_pkt_id)settings[PAYLOAD_INDEX];
    switch(settings_type)
    {
        case VALVE_POSITION:
            ret = igh_settings_remote_valvle_control(settings);
            break;
        case IGH_SEND_SETTINGS:
            ret = igh_settings_parse_new_settings(settings);
            break;
        case IGH_READ_SETTINGS:
            break;
        default:
            break;
    }

    return ret;
}

/*******************************************************************************
 * igh_settings_calculate_checksum
 *******************************************************************************/
/**
 * \brief Calculate the checksum of the igh memory structs, 
 * struct must have a single byte at the end designated for the checksum for this to work
 * \param p_struct: pointer to data struct, total_bytes: sizeof(data struct)
 * \return the checksum
 */
uint8_t igh_settings_calculate_checksum(void * p_struct, size_t total_bytes)
{
    uint8_t length = (uint8_t)total_bytes; 
    uint8_t * data;
    int sum = 0;
    uint8_t checksum = 0;

    data = (uint8_t *)p_struct;

    // uint8_t data_buffer[length];
    // memcpy(data_buffer, data, length);

    data += 1; // ignore checksum byte
    // start at 1 to ignore the checksum byte
    for( int i = 1; i < length; i++)
    {
        sum += (0xFF & *data++);
    }

    checksum = (uint8_t)(sum % 256);

    return checksum;
}

