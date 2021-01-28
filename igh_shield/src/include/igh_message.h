/*******************************************************************************
 * @file igh_message.h
 * @brief manage messages on the platform
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_MESSAGE
#define IGH_MESSAGE

#include <stdbool.h> 

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

// Macro functions for breaking large numbers into multiple bytes and the opposite
#define GET16(buf)          (((uint16_t)(buf)[0]<<8)+(buf)[1])
#define GET16_LI(buf)       (((uint16_t)(buf)[1]<<8)+(buf)[0])
#define PUT16(val, buf)     {(buf)[1]=((val)&0xff);(buf)[0]=((val)>>8);}
#define PUT16_LI(val, buf)  {(buf)[0]=((val)&0xff);(buf)[1]=((val)>>8);}
#define GET32(buf)          (((uint32_t)(buf)[0]<<24)+((uint32_t)(buf)[1]<<16)+((uint32_t)(buf)[2]<<8)+(buf)[3])
#define GET32_LI(buf)       (((uint32_t)(buf)[3]<<24)+((uint32_t)(buf)[2]<<16)+((uint32_t)(buf)[1]<<8)+(buf)[0])
#define PUT32(val, buf)     {(buf)[3]=((val)&0xff);(buf)[2]=(((val)>>8)&0xff);(buf)[1]=(((val)>>16)&0xff);(buf)[0]=(((val)>>24)&0xff);}
#define PUT32_LI(val, buf)  {(buf)[0]=((val)&0xff);(buf)[1]=(((val)>>8)&0xff);(buf)[2]=(((val)>>16)&0xff);(buf)[3]=(((val)>>24)&0xff);}

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
    SOIL_NPK                  = 0x0B,
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
    FW_VERSION                = 0x1B,
    SOIL_POTASSIUM            = 0x1C,
    SOIL_PHOSPHOROUS          = 0x1D,
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
    SETTINGS_MSG = 0x53  
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

#define EVENT_PAYLOAD_LEN 3

void igh_message_setup( void );
void igh_message_ping_home( void );
void igh_message_build_payload( igh_msg_type msg_type, uint8_t * payload, uint8_t payload_len, bool store_data_point );
void igh_message_event( igh_event_id_e event, bool store_data_point );
void igh_message_get_new_settings( void );
void igh_message_commit_new_settings( void );
void igh_message_receive_and_stage_sensor_data( void );
uint8_t igh_message_process_mqtt_data( uint8_t * buffer, uint8_t incoming_len );

#endif
