#ifndef IGH_MESSAGE
#define IGH_MESSAGE

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGE_SIZE 256
#define SIZE_OF_HEADER 2

#define FRAME_START 0x3C
#define FRAME_END   0x3E

// Macro functions for breaking large numbers into multiple bytes and the opposite
#define GET16(buf)          (((uint16_t)(buf)[1]<<8)+(buf)[0])
#define PUT16(val, buf)     {(buf)[0]=((val)&0xff);(buf)[1]=((val)>>8);}
#define GET32(buf)          (((uint32_t)(buf)[3]<<24)+((uint32_t)(buf)[2]<<16)+((uint32_t)(buf)[1]<<8)+(buf)[0])
#define PUT32(val, buf)     {(buf)[0]=((val)&0xff);(buf)[1]=(((val)>>8)&0xff);(buf)[2]=(((val)>>16)&0xff);(buf)[3]=(((val)>>24)&0xff);}

// all possible message identifiers
enum igh_pkt_id
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
    IGH_READ_SETTINGS       = 0x11
};



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


// function APIs
uint8_t igh_message_reset_buffer(void);
uint8_t igh_message_add_frame_end(void);
uint8_t igh_message_add_length(void);
igh_msg_type igh_message_add_msg_type(igh_msg_type msg_type);
igh_msg_dir igh_message_add_direction(igh_msg_dir msg_dir);
uint8_t igh_message_add_serial_number(uint8_t * serial_number);
uint8_t igh_message_add_msg_id(void);
uint8_t igh_message_check_tuple_fits(uint8_t length);
uint8_t igh_message_add_tuple(igh_pkt_id _pkt_id, uint8_t * data);
uint8_t igh_message_process_incoming_msg(uint8_t * buffer);

#ifdef __cplusplus
}
#endif

#endif
