/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_MESSAGE
#define IGH_MESSAGE

#ifdef __cplusplus
extern "C" {
#endif

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
#define PUT16(val, buf)     {(buf)[1]=((val)&0xff);(buf)[0]=((val)>>8);}
#define GET32(buf)          (((uint32_t)(buf)[0]<<24)+((uint32_t)(buf)[1]<<16)+((uint32_t)(buf)[2]<<8)+(buf)[3])
#define PUT32(val, buf)     {(buf)[3]=((val)&0xff);(buf)[2]=(((val)>>8)&0xff);(buf)[1]=(((val)>>16)&0xff);(buf)[0]=(((val)>>24)&0xff);}

// all possible message identifiers
typedef enum igh_pkt_id
{
    MSG_ACK_TUPLE           = 0x00,
    SPEAR_ID                = 0x01,
    STORE_TIMESTAMP         = 0x02,
    SEND_TIMESTAMP          = 0x03,
    SOIL_MOISTURE           = 0x04,
    AIR_HUMIDITY            = 0x05,
    SOIL_HUMIDITY           = 0x06,
    WATER_DISPENSED         = 0x07,
    CARBON_DIOXIDE          = 0x08,
    AIR_TEMPERATURE         = 0x09,
    SOIL_TEMPERATURE        = 0x0A,
    SOIL_NPK                = 0x0B,
    LIGHT_INTENSITY         = 0x0C,
    SHIELD_BATTERY_LEVEL    = 0x0D,
    SPEAR_BATTERY_LEVEL     = 0x0E,
    VALVE_POSITION          = 0x0F,
    IGH_SEND_SETTINGS       = 0x10,
    IGH_READ_SETTINGS       = 0x11,
    SPEAR_DATA              = 0x12,
    SPEAR_RF_ID             = 0x13,
    SHIELD_RF_ID            = 0x14,
    SEND_INTERVAL           = 0x15,
    OP_STATE                = 0x16,
    SHIELD_ID               = 0x17,
    RESTART                 = 0xFD,
    DATA_PKT                = 0xFE,
    END_OF_PKT_ID           = 0xFF
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
#define SIZE_OF_VALVE_POSITION          1

typedef enum igh_pkt_id igh_pkt_id;

enum igh_msg_type
{
    UNKNOWN_MSG = 0x00,
    MSG_ACK = 0x41,
    SENSOR_DATA = 0x44,
    ERROR_MSG = 0x45,
    SETTINGS_MSG = 0x53  
};
typedef enum igh_msg_type igh_msg_type;

enum igh_msg_dir
{
    IGH_DOWNLOAD = 0x44,
    IGH_UPLOAD = 0x55
};
typedef enum igh_msg_dir igh_msg_dir;

#ifdef TEST
#define LOCAL 
#else
#define LOCAL static
#endif


// function APIs
#ifdef TEST
LOCAL uint8_t igh_message_reset_buffer(void);
LOCAL uint8_t igh_message_add_frame_end(void);
LOCAL uint8_t igh_message_add_length(void);
LOCAL igh_msg_type igh_message_add_msg_type(igh_msg_type msg_type);
LOCAL igh_msg_dir igh_message_add_direction(igh_msg_dir msg_dir);
LOCAL uint8_t igh_message_add_serial_number(uint8_t * serial_number);
LOCAL uint8_t igh_message_add_msg_id(void);
LOCAL uint8_t igh_message_check_tuple_fits(uint8_t length);
LOCAL uint8_t igh_message_process_ACK(uint8_t * ack_msg);
LOCAL uint8_t igh_message_build_ACK_payload(void);
#endif

uint8_t igh_message_process_incoming_msg(uint8_t * buffer);
uint8_t igh_message_add_tuple(igh_pkt_id _pkt_id, uint8_t * data);
uint8_t igh_message_process_incoming_msg(uint8_t * buffer);
void igh_message_receive_and_stage_sensor_data( void );

#ifdef __cplusplus
}
#endif

#endif
