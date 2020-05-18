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
    message_id = 1,
    shield_id,
    spear_id,
    store_timestamp,
    send_timestamp,
    soil_moisture,
    air_humidity,
    soil_humidity,
    water_dispensed, 
    carbon_dioxide,
    air_temperature,
    soil_temperature,
    soil_npk,
    light_intensity,
    shield_battery_level,
    spear_battery_level,
    valve_position,
    msg_padding = 254,
    unknown_pkt_id = 255
};

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

#ifdef __cplusplus
}
#endif

#endif
