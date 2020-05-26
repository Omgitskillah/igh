/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Illuminum Greenhouses Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SETTINGS
#define IGH_SETTINGS

#ifdef __cplusplus
extern "C" {
#endif


enum device_op_state
{
    OP_INACTIVE = 0x00,
    OP_BASIC = 0x01,
    OP_STANDARD = 0x02,
    OP_PREMIUM = 0x03
};
typedef enum device_op_state device_op_state;

// should be moved to valve control file
enum valve_position
{
    VALVE_CLOSE,
    VALVE_OPEN
};
typedef enum valve_position valve_position;

// Should be moved to system file
struct thresholds
{
    //High Threshold tirggers
    uint16_t soil_moisture_low;          
    uint16_t air_humidity_low;           
    uint16_t soil_humidity_low;            
    uint16_t carbon_dioxide_low;
    uint16_t air_temperature_low;        
    uint16_t soil_temperature_low;       
    uint16_t soil_npk_low;               
    uint16_t light_intensity_low;        
    uint16_t shield_battery_level_low;   
    uint16_t spear_battery_level_low;   
    uint32_t water_dispensed_period_low;
    // Low Threshold Trigger
    uint16_t soil_moisture_high;          
    uint16_t air_humidity_high;           
    uint16_t soil_humidity_high;                
    uint16_t carbon_dioxide_high;
    uint16_t air_temperature_high;        
    uint16_t soil_temperature_high;       
    uint16_t soil_npk_high;               
    uint16_t light_intensity_high;        
    uint16_t shield_battery_level_high;   
    uint16_t spear_battery_level_high;             
    uint32_t water_dispensed_period_high;
};
typedef struct thresholds thresholds;

struct system_settings
{
    device_op_state op_state; // inactive, basic, standard, premium
    uint32_t reporting_interval; // frequency of data sending to cloud
    uint32_t data_resolution; // frequency of data collection
    uint8_t serial_number[12]; // The device serial number/ID
};
typedef struct system_settings system_settings;

enum igh_settings_subid
{
    // System settings
    SUBID_OPSTATE = 0x01,
    SUBID_REPORTING_INTERVAL,
    SUBID_DATA_RESOLUTION,
    SUBID_SET_SERIAL_NUMBER,
    
    //High Threshold tirggers
    SUBID_SOIL_MOISTURE_LOW,          
    SUBID_AIR_HUMIDITY_LOW,           
    SUBID_SOIL_HUMIDITY_LOW,               
    SUBID_CARBON_DIOXIDE_LOW,
    SUBID_AIR_TEMPERATURE_LOW,        
    SUBID_SOIL_TEMPERATURE_LOW,       
    SUBID_SOIL_NPK_LOW,               
    SUBID_LIGHT_INTENSITY_LOW,        
    SUBID_SHIELD_BATTERY_LEVEL_LOW,   
    SUBID_SPEAR_BATTERY_LEVEL_LOW,   
    SUBID_WATER_DISPENSED_PERIOD_LOW,

    // Low Threshold Trigger
    SUBID_SOIL_MOISTURE_HIGH,          
    SUBID_AIR_HUMIDITY_HIGH,           
    SUBID_SOIL_HUMIDITY_HIGH,                
    SUBID_CARBON_DIOXIDE_HIGH,
    SUBID_AIR_TEMPERATURE_HIGH,        
    SUBID_SOIL_TEMPERATURE_HIGH,       
    SUBID_SOIL_NPK_HIGH,               
    SUBID_LIGHT_INTENSITY_HIGH,        
    SUBID_SHIELD_BATTERY_LEVEL_HIGH,   
    SUBID_SPEAR_BATTERY_LEVEL_HIGH,             
    SUBID_WATER_DISPENSED_PERIOD_HIGH
};

    // System settings
#define LENGTH_SUBID_OPSTATE                    1
#define LENGTH_SUBID_REPORTING_INTERVAL             4
#define LENGTH_SUBID_DATA_RESOLUTION                4
#define LENGTH_SUBID_SET_SERIAL_NUMBER              12
//High Threshold tirggers
#define LENGTH_SUBID_SOIL_MOISTURE_LOW              2          
#define LENGTH_SUBID_AIR_HUMIDITY_LOW               2           
#define LENGTH_SUBID_SOIL_HUMIDITY_LOW              2               
#define LENGTH_SUBID_CARBON_DIOXIDE_LOW             2
#define LENGTH_SUBID_AIR_TEMPERATURE_LOW            2        
#define LENGTH_SUBID_SOIL_TEMPERATURE_LOW           2       
#define LENGTH_SUBID_SOIL_NPK_LOW                   2               
#define LENGTH_SUBID_LIGHT_INTENSITY_LOW            2        
#define LENGTH_SUBID_SHIELD_BATTERY_LEVEL_LOW       2   
#define LENGTH_SUBID_SPEAR_BATTERY_LEVEL_LOW        2   
#define LENGTH_SUBID_WATER_DISPENSED_PERIOD_LOW     4
// Low Threshold Trigger
#define LENGTH_SUBID_SOIL_MOISTURE_HIGH             2          
#define LENGTH_SUBID_AIR_HUMIDITY_HIGH              2           
#define LENGTH_SUBID_SOIL_HUMIDITY_HIGH             2                
#define LENGTH_SUBID_CARBON_DIOXIDE_HIGH            2
#define LENGTH_SUBID_AIR_TEMPERATURE_HIGH           2           
#define LENGTH_SUBID_SOIL_TEMPERATURE_HIGH          2       
#define LENGTH_SUBID_SOIL_NPK_HIGH                  2               
#define LENGTH_SUBID_LIGHT_INTENSITY_HIGH           2        
#define LENGTH_SUBID_SHIELD_BATTERY_LEVEL_HIGH      2   
#define LENGTH_SUBID_SPEAR_BATTERY_LEVEL_HIGH       2             
#define LENGTH_SUBID_WATER_DISPENSED_PERIOD_HIGH    4

// functions
void igh_settings_get_defaults(void);
uint8_t igh_settings_parse_new_settings(uint8_t * settings);
void igh_settings_reset_system_to_default(void);
uint8_t igh_settings_build_settings_request_payload(uint8_t * settings_req, uint8_t * buffer, uint8_t start_index);
uint8_t igh_settings_remote_valvle_control(uint8_t * settings);
// Reporting interval
// Data Collection interval



#ifdef __cplusplus
}
#endif

#endif