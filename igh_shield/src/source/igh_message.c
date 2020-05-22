#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/igh_message.h"
#include "include/igh_settings.h"

// size of each message type in bytes
uint8_t SIZE_OF_SPEAR_ID = 12;
uint8_t SIZE_OF_STORE_TIMESTAMP = 4; 
uint8_t SIZE_OF_SEND_TIMESTAMP = 4;
uint8_t SIZE_OF_SOIL_MOISTURE = 2;
uint8_t SIZE_OF_AIR_HUMIDITY = 2;
uint8_t SIZE_OF_SOIL_HUMIDITY = 2;
uint8_t SIZE_OF_WATER_DISPENSED = 4;
uint8_t SIZE_OF_CARBON_DIOXIDE = 2;
uint8_t SIZE_OF_AIR_TEMPERATURE = 2;
uint8_t SIZE_OF_SOIL_TEMPERATURE = 2;
uint8_t SIZE_OF_SOIL_NPK = 2;
uint8_t SIZE_OF_LIGHT_INTENSITY = 2;
uint8_t SIZE_OF_SHIELD_BATTERY_LEVEL = 2;
uint8_t SIZE_OF_SPEAR_BATTERY_LEVEL = 2;
uint8_t SIZE_OF_VALVE_POSITION = 1;

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

LOCAL uint8_t igh_cmd_buffer[MESSAGE_SIZE];
LOCAL uint8_t igh_cmd_buffer_tracker = 0; // track globally how full the message buffer is

LOCAL uint8_t igh_device_serial[12];
LOCAL uint8_t igh_message_id = 0; // always start as zero and overflow whenever we reach 256

extern thresholds igh_current_threshold_settings;
extern system_settings igh_current_system_settings;


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

uint8_t igh_message_add_tuple(igh_pkt_id _pkt_id, uint8_t * data)
{
    uint8_t _length = 0;
    switch(_pkt_id)
    {
        case SPEAR_ID:
            _length = SIZE_OF_SPEAR_ID;
            break;
        case STORE_TIMESTAMP:  
            _length = SIZE_OF_STORE_TIMESTAMP;
            break;    
        case SEND_TIMESTAMP: 
            _length = SIZE_OF_SEND_TIMESTAMP;
            break;      
        case SOIL_MOISTURE:
            _length = SIZE_OF_SOIL_MOISTURE;
            break;        
        case AIR_HUMIDITY:
            _length = SIZE_OF_AIR_HUMIDITY;
            break;         
        case SOIL_HUMIDITY:
            _length = SIZE_OF_SOIL_HUMIDITY;
            break;        
        case WATER_DISPENSED:
            _length = SIZE_OF_WATER_DISPENSED;
            break;      
        case CARBON_DIOXIDE:
            _length = SIZE_OF_CARBON_DIOXIDE;
            break;       
        case AIR_TEMPERATURE:
            _length = SIZE_OF_AIR_TEMPERATURE;
            break;      
        case SOIL_TEMPERATURE:
            _length = SIZE_OF_SOIL_TEMPERATURE;
            break;     
        case SOIL_NPK:
            _length = SIZE_OF_SOIL_NPK;
            break;             
        case LIGHT_INTENSITY:
            _length = SIZE_OF_LIGHT_INTENSITY;
            break;      
        case SHIELD_BATTERY_LEVEL:
            _length = SIZE_OF_SHIELD_BATTERY_LEVEL;
            break; 
        case SPEAR_BATTERY_LEVEL:
            _length = SIZE_OF_SPEAR_BATTERY_LEVEL;
            break;  
        case VALVE_POSITION:
            _length = SIZE_OF_VALVE_POSITION;
            break; 
        default:
            _length = 0;
            // TODO: throw error here
            break;     
    }

    if( (_length > 0) &&  igh_message_check_tuple_fits(_length))
    {
        // add the packet id
        igh_msg_buffer[igh_msg_buffer_tracker] = _pkt_id;
        igh_msg_buffer_tracker++;
        // add the length
        igh_msg_buffer[igh_msg_buffer_tracker] = _length;
        igh_msg_buffer_tracker++;
        memcpy(&igh_msg_buffer[igh_msg_buffer_tracker], data, _length);
        igh_msg_buffer_tracker += _length;
    }
    else
    {
        // throw error
    }

    return igh_msg_buffer_tracker;
}

uint8_t igh_message_process_incoming_msg(uint8_t * buffer)
{
    igh_msg_type message_type = UNKNOWN_MSG;
    uint8_t length = buffer[1]; // get the length

    if( (buffer[0] != FRAME_START) && (buffer[length-1] != FRAME_END) )
    {
        // if there is no frame start or no frame end, do nothing and return zer0
        // This may mean the length was wrong or the message was corrupted
    }
    else
    {
        // check the serial number
        if( 0 != memcmp(igh_current_system_settings.serial_number, &buffer[SN_INDEX], 12))
        {
            // is the serial number does not match, do nothing as this message wasn't meant for this device,
            // THis ideally should never happen
        }
        else
        {
            // Message process message if all is well
            message_type = (igh_msg_type)buffer[MSG_TYPE_INDEX];

            if( MSG_ACK == message_type )
            {
                // TODO: prcoess ACK here
            }
            else if( SETTINGS_MSG == message_type )
            {
                // TODO: process settings here
            }
        }
    }


    return message_type; // should return the extracted tuple id for processing later
}













































