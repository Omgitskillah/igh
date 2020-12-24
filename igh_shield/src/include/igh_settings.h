/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
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

enum _auto_irrigation_type_e
{
    UNKNOWN,
    SENSOR_IRRIGATION,
    HOURLY_IRRIGATION
};

// Should be moved to system file
struct thresholds
{
    uint8_t checksum;
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
#define SIZE_OF_THRESHOLDS 49 // TODO: increment this whenever we expand the thresholds

struct system_settings
{
    uint8_t checksum; // should always start so that it is always buffered
    int timezone; // timezone in which the device is installed
    uint8_t irrigation_hr; // at what hour should we irrigate?
    uint8_t auto_irrigation_type; // hourly or using sensor data?
    device_op_state op_state; // inactive, basic, standard, premium
    uint32_t water_dispenser_period; // tracks time in seconds
    uint32_t water_amount_by_button_press; // how much water we will get when we press the button
    uint32_t reporting_interval; // frequency of data sending to cloud
    uint32_t data_resolution; // frequency of data collection
    uint8_t serial_number[12]; // The device serial number/ID
    uint8_t broker[64]; // MQTT broker url
    uint16_t broker_port; //MQTT broker connection port
    uint8_t mqtt_username[32];
    uint8_t mqtt_password[32];
};
typedef struct system_settings system_settings;

#define BORON_SN_LEN (12)
extern char boron_serial_number[BORON_SN_LEN];
extern uint8_t deviceID_string[24];

#define SIZE_OF_SYSTEM_SETTINGS 22 // TODO: increment this whenever we expand the system settings

#define INDEX_RFM69_NETWORK_ID  10 // 10th byte of the serial number is the Network ID
#define INDEX_RFM69_NODE_ID     11 // 11th byte of the Serial number is the Node ID
extern uint8_t initialize_rfm69;
extern uint8_t mqtt_set_broker;

enum igh_settings_subid
{
    // System settings
    SUBID_OPSTATE = 0x01,
    SUBID_REPORTING_INTERVAL,
    SUBID_DATA_RESOLUTION,
    SUBID_SET_SERIAL_NUMBER,
    SUBID_MQTT_BROKER,
    SUBID_MQTT_BROKER_PORT,
    SUBID_TIMEZONE,
    SUBID_IRRIGATION_HR,
    SUBID_WATER_DISP_PERIOD,
    SUBID_MQTT_USERNAME,
    SUBID_MQTT_PASSWORD,
    SUBID_WATER_AMOUNT_BY_BUTTON,
    SUBID_AUTO_IRRIGATION_TYPE,
    
    //High Threshold tirggers
    SUBID_SOIL_MOISTURE_LOW = 0x10,          
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
    SUBID_SOIL_MOISTURE_HIGH = 0x30,          
    SUBID_AIR_HUMIDITY_HIGH,           
    SUBID_SOIL_HUMIDITY_HIGH,                
    SUBID_CARBON_DIOXIDE_HIGH,
    SUBID_AIR_TEMPERATURE_HIGH,        
    SUBID_SOIL_TEMPERATURE_HIGH,       
    SUBID_SOIL_NPK_HIGH,               
    SUBID_LIGHT_INTENSITY_HIGH,        
    SUBID_SHIELD_BATTERY_LEVEL_HIGH,   
    SUBID_SPEAR_BATTERY_LEVEL_HIGH,             
    SUBID_WATER_DISPENSED_PERIOD_HIGH,
};

    // System settings
#define LENGTH_SUBID_OPSTATE                        1
#define LENGTH_SUBID_REPORTING_INTERVAL             4
#define LENGTH_SUBID_DATA_RESOLUTION                4
#define LENGTH_SUBID_SET_SERIAL_NUMBER              12
#define LENGTH_SUBID_MQTT_PORT                      2
#define LENGTH_SUBID_SUBID_TIMEZONE                 2
#define LENGTH_SUBID_AUTO_IRRIGATION_TYPE           1
#define MAX_TIMEZONE                                12
#define NEGATIVE_TIME_ZONE                          0x00
#define POSITIVE_TIME_ZONE                          0xFF
#define LENGTH_SUBID_SUBID_IRRIGATION_HR            1
#define MAX_HOUR                                    24
#define MIN_HOUR                                    0
#define LENGTH_SUBID_WATER_DISP_PERIOD              4
#define LENGTH_SUBID_SUBID_WATER_AMOUNT_BY_BUTTON   4
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


// SHT1x Multipliers
#define SOIL_TEMPERATURE_MULTIPLIER_D1              (-39.66)
#define SOIL_TEMPERATURE_MULTIPLIER_D2              (0.01)
#define SOIL_HUMIDITY_MULTIPLIER_C1                 (-2.0468)
#define SOIL_HUMIDITY_MULTIPLIER_C2                 (0.0367)
#define SOIL_HUMIDITY_MULTIPLIER_C3                 (-1.59955e-6)
#define SOIL_HUMIDITY_MULTIPLIER_T1                 (0.01)
#define SOIL_HUMIDITY_MULTIPLIER_T2                 (0.00008)
#define ROOM_TEMPERATURE                            (25.0)


extern system_settings igh_default_system_settings;
extern system_settings igh_current_system_settings;

extern thresholds igh_default_thresholds;
extern thresholds igh_current_threshold_settings;

extern valve_position current_valve_position;
extern uint8_t default_serial_number[];
extern uint8_t default_broker_url[];


// functions
#ifdef TEST
#define LOCAL 
#else
#define LOCAL static
#endif

#ifdef TEST
LOCAL void igh_settings_get_defaults(void);
LOCAL uint8_t igh_settings_parse_new_settings(uint8_t * settings);
LOCAL uint8_t igh_settings_build_settings_request_payload(uint8_t * settings_req, uint8_t * buffer, uint8_t start_index);
LOCAL uint8_t igh_settings_remote_valvle_control(uint8_t * settings);
#endif

uint8_t igh_settings_process_settings_tuples( uint8_t * settings, uint8_t byte_tracker, uint8_t end_index );
void igh_settings_reset_system_to_default(void);
uint8_t igh_settings_process_settings(uint8_t * settings);
uint8_t igh_settings_calculate_checksum(void * p_struct, size_t total_bytes);

// Reporting interval
// Data Collection interval
extern thresholds igh_current_threshold_settings;
extern system_settings igh_current_system_settings;
extern valve_position current_valve_position;
extern uint8_t new_settings_available;

#ifdef __cplusplus
}
#endif

#endif