// #include "Particle.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

#define MESSAGE_FITS(X,Y) ((MESSAGE_SIZE - X) >= Y)

#define LEN_INDEX       1
#define MSG_TYPE_INDEX   2
#define MSG_DIRECTION_INDEX 3
#define SN_INDEX 4
#define MSG_ID_INDEX 16

#ifdef TEST
#define LOCAL 
#else
#define LOCAL static
#endif

LOCAL uint8_t igh_msg_buffer[MESSAGE_SIZE];  // global variable to be used to hold page for sending, maximum message size is 256 bytes
LOCAL uint8_t igh_msg_buffer_tracker = 0; // track globally how full the message buffer is
LOCAL uint8_t igh_device_serial[12];
LOCAL uint8_t igh_message_id = 0; // always start as zero and overflow whenever we reach 256

// local functions


uint8_t igh_message_reset_buffer(void)
{
    memset(igh_msg_buffer,'\0',MESSAGE_SIZE);

    igh_msg_buffer[0] = FRAME_START; // place the frame start at the beginning of the buffer
    igh_msg_buffer_tracker = 17; // move the index tracker to leave room for data header

    return igh_msg_buffer_tracker;
}

uint8_t igh_message_add_frame_end(void)
{
    igh_msg_buffer[igh_msg_buffer_tracker] = FRAME_END;

    return igh_msg_buffer_tracker;
}

uint8_t igh_message_add_length(void)
{
    if(igh_msg_buffer[igh_msg_buffer_tracker] != FRAME_END)
    {
        return igh_message_reset_buffer();
    }
    else
    {
        igh_msg_buffer[LEN_INDEX] = igh_msg_buffer_tracker;
        return igh_msg_buffer_tracker;
    }
}

igh_msg_type igh_message_add_msg_type(igh_msg_type msg_type)
{
    igh_msg_type _type; 

    switch(msg_type)
    {
        case SENSOR_DATA:
            _type = SENSOR_DATA;
            break;
        case SETTINGS_MSG:
            _type = SETTINGS_MSG;
            break;
        case ERROR_MSG:
            _type = ERROR_MSG;
            break;
        case MSG_ACK:
            _type = MSG_ACK;
            break;
        default:
            _type = UNKNOWN_MSG;
            break;
    }

    // should return unknown for anything else
    igh_msg_buffer[MSG_TYPE_INDEX] = _type;

    return (igh_msg_type)igh_msg_buffer[MSG_TYPE_INDEX];
}

igh_msg_dir igh_message_add_direction(igh_msg_dir msg_dir)
{
    igh_msg_type _dir;

    switch(msg_dir)
    {
        case IGH_UPLOAD:
            _dir = IGH_UPLOAD;
            break;
        case IGH_DOWNLOAD:
            _dir = IGH_DOWNLOAD;
            break;
        default:
            // TODO: through error here
            break;
    }

    igh_msg_buffer[MSG_DIRECTION_INDEX] = _dir;

    return (igh_msg_type)igh_msg_buffer[MSG_DIRECTION_INDEX];
}

uint8_t igh_message_add_serial_number(uint8_t * serial_number)
{
    memcpy(&igh_msg_buffer[SN_INDEX], serial_number, 12);
    return igh_msg_buffer_tracker;
}

uint8_t igh_message_add_msg_id(void)
{
    igh_msg_buffer[MSG_ID_INDEX] = igh_message_id; // igh_message_id is a global variable
    igh_message_id++;

    return igh_message_id;
}

uint8_t igh_message_check_tuple_fits(uint8_t length)
{
    uint8_t total_tuple_len = length + 2;

    if( MESSAGE_FITS(igh_msg_buffer_tracker, total_tuple_len) )
    {
        return 1;
    }
    else
    {
        return 0;
    }  
}

// uint8_t igh_message_add_to_payload()















































// void add_message_id(void);
// void add_bytes(uint8_t bytes, uint8_t * _msg);

// void igh_message_init(void)
// {
//     igh_message_clear_buffer();
// }

// void igh_message_add_to_payload(enum igh_pkt_id pkt_id, uint8_t * _msg)
// {
//     uint8_t id = unknown_pkt_id;
//     uint8_t msg_size = 0;

//     switch (pkt_id)
//     {
//         case message_id:
//             id = message_id;
//             msg_size = size_of_message_id;
//             break;
//         case shield_id:
//             id = shield_id;
//             msg_size = size_of_shield_id;
//             break;
//         case spear_id:
//             id = spear_id;
//             msg_size = size_of_spear_id;
//             break;
//         case send_timestamp:
//             id = send_timestamp;
//             msg_size = size_of_send_timestamp;
//             break;   
//         case shield_battery_level:
//             id = shield_battery_level;
//             msg_size = size_of_shield_battery_level;
//             break;
//         case valve_position:
//             id = valve_position;
//             msg_size = size_of_valve_position;
//             break;
//         default:
//             // don't build message if pkt id is unknown
//             return;            
//     }

//     if( MESSAGE_FITS((msg_size + SIZE_OF_HEADER), igh_msg_buffer_tracker) )
//     {
//         igh_msg_buffer[igh_msg_buffer_tracker] = id;
//         igh_msg_buffer_tracker++; 
//         igh_msg_buffer[igh_msg_buffer_tracker] = msg_size;
//         igh_msg_buffer_tracker++;
//         add_bytes(msg_size, _msg);
//     }
//     else
//     {
//         // Throw error
//     }
    
// }

// void add_bytes(uint8_t bytes, uint8_t * _msg)
// {
//     for( int i = 0; i < bytes; i++ )
//     {
//         igh_msg_buffer[igh_msg_buffer_tracker] = _msg[i];
//         igh_msg_buffer_tracker++;
//     }
// }


// void igh_message_complete_package(void)
// {
//     igh_msg_buffer_tracker++;
//     igh_msg_buffer[igh_msg_buffer_tracker] = msg_padding;
//     igh_msg_buffer_tracker++;
//     igh_msg_buffer[1] = igh_msg_buffer_tracker;
// }

// void add_message_id(void)
// {
//     uint8_t local_message_buffer[size_of_message_id];
//     for(int i = 0; i < size_of_message_id; i++)
//     {
//         local_message_buffer[i] = (uint8_t)(current_message_id << (i*8) );
//     }
//     igh_message_add_to_payload(message_id, local_message_buffer);
// }
