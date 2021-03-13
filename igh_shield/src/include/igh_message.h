/*******************************************************************************
 * @file igh_message.h
 * @brief manage messages on the platform
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_MESSAGE
#define IGH_MESSAGE

#include <stdbool.h> 
#include <stdint.h> 

#define MESSAGE_SIZE 255
#define SIZE_OF_HEADER 2

#define FRAME_START 0x3C
#define FRAME_END   0x3E


#define LEN_INDEX       1
#define MSG_TYPE_INDEX   2
#define MSG_DIRECTION_INDEX 3
#define SN_INDEX 4
#define MSG_ID_INDEX 16
#define PAYLOAD_INDEX 17
#define HEADER_AND_FOOTER 18
#define TUPLE_HEADER_LEN 2

// all possible message identifiers
typedef enum igh_pkt_id
{
    MSG_ACK_TUPLE             = 0x00,
    SPEAR_ID                  = 0x01,
    STORE_TIMESTAMP           = 0x02,
    SEND_TIMESTAMP            = 0x03,
    SOIL_MOISTURE             = 0x04,
    AIR_HUMIDITY              = 0x05,
    SOIL_HUMIDITY             = 0x06,
    WATER_DISPENSED           = 0x07,
    CARBON_DIOXIDE            = 0x08,
    AIR_TEMPERATURE           = 0x09,
    SOIL_TEMPERATURE          = 0x0A,
    SOIL_NITROGEN             = 0x0B,
    LIGHT_INTENSITY           = 0x0C,
    SHIELD_BATTERY_LEVEL      = 0x0D,
    SPEAR_BATTERY_LEVEL       = 0x0E,
    VALVE_POSITION            = 0x0F,
    IGH_SEND_SETTINGS         = 0x10,
    IGH_READ_SETTINGS         = 0x11,
    SPEAR_DATA                = 0x12,
    SPEAR_RF_ID               = 0x13,
    SHIELD_RF_ID              = 0x14,
    SEND_INTERVAL             = 0x15,
    OP_STATE                  = 0x16,
    SHIELD_ID                 = 0x17,
    SPEAR_BATT_LOW_THRESHOLD  = 0x18,
    SHIELD_BATT_LOW_THRESHOLD = 0x19,
    BUTTON_PRESS              = 0x1A,
    SHIELD_FW_VERSION         = 0x1B,
    SOIL_POTASSIUM            = 0x1C,
    SOIL_PHOSPHOROUS          = 0x1D,
    SPEAR_SERIAL_SENSOR_TYPE  = 0x1E,
    SPEAR_FW_VERSION          = 0x1F,
    SYSTEM_EVENT              = 0xFB,
    EVENT                     = 0xFC,
    RESTART                   = 0xFD,
    DATA_PKT                  = 0xFE,
    END_OF_PKT_ID             = 0xFF
} pkt_id;


// size of each message type in bytes
#define SIZE_OF_MSG_ACK_TUPLE           1
#define SIZE_OF_SPEAR_ID                12
#define SIZE_OF_STORE_TIMESTAMP         4 
#define SIZE_OF_SEND_TIMESTAMP          4
#define SIZE_OF_SOIL_MOISTURE           2
#define SIZE_OF_AIR_HUMIDITY            2
#define SIZE_OF_SOIL_HUMIDITY           2
#define SIZE_OF_WATER_DISPENSED         4
#define SIZE_OF_CARBON_DIOXIDE          2
#define SIZE_OF_AIR_TEMPERATURE         2
#define SIZE_OF_SOIL_TEMPERATURE        2
#define SIZE_OF_SOIL_NPK                2
#define SIZE_OF_LIGHT_INTENSITY         2
#define SIZE_OF_SHIELD_BATTERY_LEVEL    4
#define SIZE_OF_SPEAR_BATTERY_LEVEL     2
#define SIZE_OF_RESTART                 1
#define SIZE_OF_SYSTEM_EVENT            4
#define SIZE_OF_EVENT                   1
#define SIZE_OF_VALVE_POSITION          1
#define SIZE_OF_BUTTON_PRESS            1
#define SIZE_OF_FW_VERSION              3

typedef enum igh_pkt_id igh_pkt_id;

enum igh_msg_type
{
    UNKNOWN_MSG = 0x00,
    MSG_ACK = 0x41,
    SENSOR_DATA = 0x44,
    EVENT_MSG = 0x45,
    SETTINGS_MSG = 0x53,  
    SETTINGS_REQUEST = 0x54  
};
typedef enum igh_msg_type igh_msg_type;

enum igh_msg_dir
{
    IGH_DOWNLOAD = 0x44,
    IGH_UPLOAD = 0x55
};
typedef enum igh_msg_dir igh_msg_dir;

typedef enum _igh_event_id_e
{
    // start error ID from 50
    EVENT_DEVICE_RESTART = 0x50,
    EVENT_SD_CARD_ERROR,
    EVANT_BUTTON_IRRIGATION_ON,
    EVANT_BUTTON_IRRIGATION_OFF,
    EVENT_VAVLE_OPENED,
    EVENT_VAVLE_CLOSED,
    EVENT_IRRIGATION_SUSPENDED,
    EVENT_IRRIGATION_RESUMED,
    EVENT_IRRIGATION_ENABLED,
    EVENT_IRRIGATION_DISABLED,
    EVENT_MQTT_ERROR,
    EVENT_UNKNOWN_MQTT_CMD,
    EVENT_CMD_SENT_TO_WRONG_DEVICE,
    EVENT_SETTINGS_UPDATE_SUCCESS,
    EVENT_SETTINGS_UPDATE_FAIL,
    EVENT_CALL_HOME,
    EVENT_RESET_IRRIGATION,
    EVENT_SYSTEM_RESET,
    EVENT_INVALID_SOIL_DATA
} igh_event_id_e;

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
    SUBID_CLOCK_IRRIGATION_INTERVAL,
    
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
#define MAX_HOUR                                    23
#define MIN_HOUR                                    0
#define LENGTH_SUBID_WATER_DISP_PERIOD              4
#define LENGTH_SUBID_SUBID_WATER_AMOUNT_BY_BUTTON   4
#define LENGTH_SUBID_CLOCK_IRRIGATION_INTERVAL      4
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

#define EVENT_PAYLOAD_LEN 3
#define SYSTEM_EVENT_PAYLOAD_LEN 6

extern volatile uint8_t irrigation_settings_updated;
extern volatile uint8_t irrigation_sensor_data_updated;
extern uint16_t new_humidity;
extern volatile uint8_t timezone_updated;
extern uint8_t initialize_rfm69;
extern volatile uint8_t mqtt_set_broker; // make sure we set the broker on init
extern volatile uint8_t new_settings_available;
extern volatile uint8_t new_reporting_interval_set;
extern volatile uint8_t valve_state_to_send;
extern uint8_t remote_valve_state;
extern volatile bool remote_valve_command;
extern float total_water_dispensed_snapshot;
extern volatile bool reset_water_flow;

void igh_message_setup( void );
void igh_message_ping_home( void );
void igh_message_build_payload( igh_msg_type msg_type, uint8_t * payload, uint8_t payload_len, bool store_data_point );
void igh_message_event( igh_event_id_e event, bool store_data_point );
void igh_message_churn( void );
uint8_t igh_message_process_mqtt_data( uint8_t * buffer, uint8_t incoming_len );

#endif
