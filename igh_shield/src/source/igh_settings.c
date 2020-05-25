#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/igh_message.h"
#include "include/igh_settings.h"
#include "include/igh_default_settings.h"
// #include "include/igh_message.h"

#define SETTINGS_LEN_INDEX (PAYLOAD_INDEX + 1)
#define FIRST_SETTINGS_TUPLE_INDEX (SETTINGS_LEN_INDEX + 1)

device_op_state current_op_state;
device_op_state previous_op_state;

thresholds igh_default_thresholds;
system_settings igh_default_system_settings;
uint8_t default_serial_number[] = DEFAULT_SERIAL_NUMBER;

thresholds igh_current_threshold_settings;
system_settings igh_current_system_settings;

void igh_settings_get_defaults(void) // Total bytes 
{
    // System settings
    memcpy(igh_default_system_settings.serial_number, default_serial_number, 12);
    igh_default_system_settings.op_state                    = DEFAULT_NEW_OPSTATE;
    igh_default_system_settings.reporting_interval          = DEFAULT_REPORTING_INTERVAL;
    igh_default_system_settings.data_resolution             = DEFAULT_DATA_RESOLUTION;
    //High Threshold tirggers
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
} // test this function

uint8_t igh_settings_parse_new_settings(uint8_t * settings)
{
    // get the length
    uint8_t length = settings[SETTINGS_LEN_INDEX]; // bit at position one should always be length
    // guard the process
    if(0 >= length)
    {
        return 0; // do not allow settings with zero payload to be processed
    }
    // get index of last byte to process
    uint8_t settings_end_index = FIRST_SETTINGS_TUPLE_INDEX + length;
    // get the location of first tuple
    uint8_t settings_byte_tracker = FIRST_SETTINGS_TUPLE_INDEX;
    //cycle through tuples to get the settings data
    uint8_t current_tuple_id;
    uint8_t current_tuple_length;
    uint8_t current_data_index;
    while(settings_byte_tracker < settings_end_index)
    {
        // extract tuples
        current_tuple_id = settings[settings_byte_tracker];
        // extract the length
        current_tuple_length = settings[settings_byte_tracker + 1]; // should always follow
        // extract the tuple data based on tuple id
        current_data_index = settings_byte_tracker+2;

        switch(current_tuple_id)
        {
            case SUBID_NEW_OPSTATE:
                if(LENGTH_SUBID_NEW_OPSTATE == current_tuple_length)
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

            case SUBID_SET_SERIAL_NUMBER:
                if(LENGTH_SUBID_SET_SERIAL_NUMBER == current_tuple_length)
                {
                    memcpy(igh_current_system_settings.serial_number, &settings[current_data_index], LENGTH_SUBID_SET_SERIAL_NUMBER);
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
        settings_byte_tracker += current_tuple_length + TUPLE_HEADER_LEN;
    }

    return 1;
}

void igh_settings_reset_system_to_default(void)
{
    igh_settings_get_defaults();
    igh_current_system_settings = igh_default_system_settings;
    igh_current_threshold_settings = igh_default_thresholds;
}

// payload build settings for sending

uint8_t igh_settings_build_settings_request_payload(uint8_t * settings_req, uint8_t * buffer)
{
}


