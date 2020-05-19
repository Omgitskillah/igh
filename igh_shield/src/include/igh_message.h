#ifndef IGH_MESSAGE
#define IGH_MESSAGE

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGE_SIZE 256
#define SIZE_OF_HEADER 2

#define FRAME_START 0x3C
#define FRAME_END   0x3E

// all possible message identifiers
enum igh_pkt_id
{
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
};



typedef enum igh_pkt_id igh_pkt_id;

enum igh_msg_type
{
    UNKNOWN_MSG = 0x00,
    SETTINGS_MSG = 0x22,
    MSG_ACK = 0xAC,
    SENSOR_DATA = 0xDA,
    ERROR_MSG = 0xEE  
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
uint8_t igh_message_add_tuple_to_payload(igh_pkt_id _pkt_id, uint8_t * data);

#ifdef __cplusplus
}
#endif

#endif
