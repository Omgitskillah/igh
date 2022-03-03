/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/igh_settings.h"
#include "include/igh_default_settings.h"

char boron_serial_number[BORON_SN_LEN];
uint8_t deviceID_string[24];

device_op_state current_op_state;
device_op_state previous_op_state;

thresholds igh_default_thresholds;
system_settings igh_default_system_settings;
uint8_t default_serial_number[] = DEFAULT_SERIAL_NUMBER;
uint8_t default_broker_url[] = DEFAULT_MQTT_BROKER;
uint8_t default_broker_uname[] = DEFAULT_MQTT_USERNAME;
uint8_t default_broker_pword[] = DEFAULT_MQTT_PASSWORD;

thresholds igh_current_threshold_settings;
system_settings igh_current_system_settings;

// functions
#ifndef TEST
LOCAL void igh_settings_get_defaults(void);
#endif

LOCAL void igh_settings_get_defaults(void) // Total bytes 
{
    // System settings
    memset(&igh_default_system_settings, 0, sizeof(igh_default_system_settings));
    igh_default_system_settings.timezone                     = DEFAULT_TIMEZONE;
    igh_default_system_settings.irrigation_hr                = DEFAULT_IRRIGATION_HR;
    igh_default_system_settings.water_dispenser_period       = DEFAULT_WATER_DISP_PERIOD;
    igh_default_system_settings.water_amount_by_button_press = DEFAULT_WATER_BY_BUTTON;
    igh_default_system_settings.op_state                     = DEFAULT_NEW_OPSTATE;
    igh_default_system_settings.auto_irrigation_type         = DEFAULT_AUTO_IRRIGATION_TYPE;
    igh_default_system_settings.reporting_interval           = DEFAULT_REPORTING_INTERVAL;
    igh_default_system_settings.data_resolution              = DEFAULT_DATA_RESOLUTION;
    igh_default_system_settings.broker_port                  = DEFAULT_MQTT_BROKER_PORT;
    igh_default_system_settings.clock_irrigation_interval    = DEFAULT_CLOCK_IRRIGATION_INTERVAL;

    memcpy(igh_default_system_settings.serial_number, default_serial_number, sizeof(default_serial_number));
    memcpy(igh_default_system_settings.broker, default_broker_url, sizeof(default_broker_url));
    memcpy(igh_default_system_settings.mqtt_username, default_broker_uname, sizeof(default_broker_uname));
    memcpy(igh_default_system_settings.mqtt_password, default_broker_pword, sizeof(default_broker_pword));
    
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

