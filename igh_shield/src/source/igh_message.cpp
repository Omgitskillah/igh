// #include "Particle.h"
#include "include/igh_message.h"

// size of each message type in bytes
uint8_t size_of_message_id = 4;
uint8_t size_of_shield_id = 12;  // eg: e00fce683f0ea854ec94ca8d
uint8_t size_of_spear_id = 12;
uint8_t size_of_store_timestamp = 4; 
uint8_t size_of_send_timestamp = 4;
uint8_t size_of_soil_moisture = 2;
uint8_t size_of_air_humidity = 2;
uint8_t size_of_soil_humidity = 2;
uint8_t size_of_water_dispensed = 4;
uint8_t size_of_carbon_dioxide = 2;
uint8_t size_of_air_temperature = 2;
uint8_t size_of_soil_temperature = 2;
uint8_t size_of_soil_npk = 2;
uint8_t size_of_light_intensity = 2;
uint8_t size_of_shield_battery_level = 2;
uint8_t size_of_spear_battery_level = 2;
uint8_t size_of_valve_position = 1;

uint8_t igh_msg_buffer[MESSAGE_SIZE];  // global variable to be used to hold page for sending, maximum message size is 256 bytes
volatile uint8_t igh_msg_buffer_tracker = 2; // track globally how full the message buffer is
uint32_t current_message_id = 0;

// local functions
void add_message_id(void);
void add_bytes(uint8_t bytes, uint8_t * _msg);

void igh_message_init(void)
{
    igh_message_clear_buffer();
}

void igh_message_add_to_payload(igh_pkt_id pkt_id, uint8_t * _msg)
{
    uint8_t id = unknown_pkt_id;
    uint8_t msg_size = 0;

    switch (pkt_id)
    {
        case message_id:
            id = message_id;
            msg_size = size_of_message_id;
            break;
        case shield_id:
            id = shield_id;
            msg_size = size_of_shield_id;
            break;
        case spear_id:
            id = spear_id;
            msg_size = size_of_spear_id;
            break;
        case send_timestamp:
            id = send_timestamp;
            msg_size = size_of_send_timestamp;
            break;   
        case shield_battery_level:
            id = shield_battery_level;
            msg_size = size_of_shield_battery_level;
            break;
        case valve_position:
            id = valve_position;
            msg_size = size_of_valve_position;
            break;
        default:
            // don't build message if pkt id is unknown
            return;            
    }

    if( MESSAGE_FITS((msg_size + SIZE_OF_HEADER), igh_msg_buffer_tracker) )
    {
        igh_msg_buffer[igh_msg_buffer_tracker] = id;
        igh_msg_buffer_tracker++; 
        igh_msg_buffer[igh_msg_buffer_tracker] = msg_size;
        igh_msg_buffer_tracker++;
        add_bytes(msg_size, _msg);
    }
    else
    {
        // Throw error
    }
    
}

void add_bytes(uint8_t bytes, uint8_t * _msg)
{
    for( int i = 0; i < bytes; i++ )
    {
        igh_msg_buffer[igh_msg_buffer_tracker] = _msg[i];
        igh_msg_buffer_tracker++;
    }
}

void igh_message_clear_buffer(void)
{
    igh_msg_buffer[0] = msg_padding;
    for(uint8_t i = 2; i < MESSAGE_SIZE; i++)
    {
        igh_msg_buffer[i] = '\0'; // null the buffer
    }
    igh_msg_buffer_tracker = 2; // always reset  to point to location two of the buffer
}

void igh_message_complete_package(void)
{
    igh_msg_buffer_tracker++;
    igh_msg_buffer[igh_msg_buffer_tracker] = msg_padding;
    igh_msg_buffer_tracker++;
    igh_msg_buffer[1] = igh_msg_buffer_tracker;
}

void add_message_id(void)
{
    uint8_t local_message_buffer[size_of_message_id];
    for(int i = 0; i < size_of_message_id; i++)
    {
        local_message_buffer[i] = uint8_t(current_message_id << (i*8) );
    }
    igh_message_add_to_payload(message_id, local_message_buffer);
}
