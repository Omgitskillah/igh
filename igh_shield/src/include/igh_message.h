#ifndef IGH_MESSAGE
#define IGH_MESSAGE


#define MESSAGE_SIZE 255
#define SIZE_OF_HEADER 2
#define MESSAGE_FITS(X,Y) (((X)+(Y)) <= (MESSAGE_SIZE))

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

// function APIs
void igh_message_init(void);
void igh_message_clear_buffer(void);
void igh_message_add_to_payload(igh_pkt_id pkt_id, uint8_t * _msg);
void igh_message_complete_package(void);

#endif