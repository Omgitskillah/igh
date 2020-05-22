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
    SUBID_NEW_OPSTATE = 0x01,
    SUBID_REPORTING_INTERVAL,
    SUBID_DATA_RESOLUTION,
    
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

// Reporting interval
// Data Collection interval



#ifdef __cplusplus
}
#endif

#endif