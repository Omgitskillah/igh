/*******************************************************************************
 * @file igh_settings.h
 * @brief manage settings on the system
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

typedef enum _auto_irrigation_type_e
{
    UNKNOWN,
    SENSOR_IRRIGATION,
    HOURLY_IRRIGATION
}auto_irrigation_type_e;

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
    uint32_t clock_irrigation_interval; // how often should clock irrigation occur
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

// Macro functions for breaking large numbers into multiple bytes and the opposite
#define GET16(buf)          (((uint16_t)(buf)[0]<<8)+(buf)[1])
#define GET16_LI(buf)       (((uint16_t)(buf)[1]<<8)+(buf)[0])
#define PUT16(val, buf)     {(buf)[1]=((val)&0xff);(buf)[0]=((val)>>8);}
#define PUT16_LI(val, buf)  {(buf)[0]=((val)&0xff);(buf)[1]=((val)>>8);}
#define GET32(buf)          (((uint32_t)(buf)[0]<<24)+((uint32_t)(buf)[1]<<16)+((uint32_t)(buf)[2]<<8)+(buf)[3])
#define GET32_LI(buf)       (((uint32_t)(buf)[3]<<24)+((uint32_t)(buf)[2]<<16)+((uint32_t)(buf)[1]<<8)+(buf)[0])
#define PUT32(val, buf)     {(buf)[3]=((val)&0xff);(buf)[2]=(((val)>>8)&0xff);(buf)[1]=(((val)>>16)&0xff);(buf)[0]=(((val)>>24)&0xff);}
#define PUT32_LI(val, buf)  {(buf)[0]=((val)&0xff);(buf)[1]=(((val)>>8)&0xff);(buf)[2]=(((val)>>16)&0xff);(buf)[3]=(((val)>>24)&0xff);}

extern system_settings igh_default_system_settings;
extern system_settings igh_current_system_settings;

extern thresholds igh_default_thresholds;
extern thresholds igh_current_threshold_settings;

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
#endif

void igh_settings_reset_system_to_default(void);
uint8_t igh_settings_calculate_checksum(void * p_struct, size_t total_bytes);

// Reporting interval
// Data Collection interval
extern thresholds igh_current_threshold_settings;
extern system_settings igh_current_system_settings;

#ifdef __cplusplus
}
#endif

#endif