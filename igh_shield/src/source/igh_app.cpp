/*******************************************************************************
 * @file igh_app.cpp
 * @brief Main IGH application for the Shield board
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"

#include "include/igh_app.h"

#include "include/igh_settings.h"
#include "include/igh_message.h"
#include "particle_api/igh_boron.h"
#include "particle_api/igh_rfm69.h"
#include "particle_api/igh_mqtt.h"
#include "particle_api/igh_eeprom.h"

uint8_t device_restart = 1;
extern uint8_t igh_msg_buffer[MESSAGE_SIZE]; 

uint8_t igh_app_add_payload( uint8_t *_buffer, uint8_t start, uint8_t * _payload, uint8_t _payload_len );
uint8_t igh_app_add_message_header( uint8_t *_buffer, uint8_t start, igh_msg_type msg_type, igh_msg_dir dir );
void igh_app_send_device_restart( void );
void igh_app_receive_and_stage_sensor_data( void );
uint8_t igh_eeprom_get_serial_hex_data( uint8_t * buffer, uint8_t len );
void igh_spear_payload_get_new_settings( void );

void igh_app_setup( void )
{
    igh_boron_setup();
    // get settings from eeprom
    igh_eeprom_init();
    // starte the radio
    igh_rfm69_setup();
    // setup MQTT
    igh_mqtt_setup();
}

void igh_main_application( void )
{
    // check if there are any settings to read
    igh_spear_payload_get_new_settings();
    // process boron service
    igh_boron_service();
    // Send or store device restart message
    igh_app_send_device_restart();
    // Send or Store Spear data
    igh_app_receive_and_stage_sensor_data();
    igh_mqtt_service();
}

void igh_app_receive_and_stage_sensor_data( void )
{
    uint8_t rx_buffer[RF69_MAX_DATA_LEN];
    uint8_t data_rx_len = 0;

    data_rx_len = igh_rfm69_receive_raw_bytes( rx_buffer, sizeof(rx_buffer) );

    if( 0 != data_rx_len )
    {
        uint8_t i = 0; // keep track of pkt data

        // first clear the buffer
        memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );

        // Add frame start
        igh_msg_buffer[i++] = FRAME_START;
        i++; // leave room for pkg length

        i = igh_app_add_message_header( igh_msg_buffer, i, ERROR_MSG, IGH_UPLOAD );
        i = igh_app_add_payload( igh_msg_buffer, i, rx_buffer, data_rx_len );

        // Add Frame End 
        igh_msg_buffer[i++] = FRAME_END;

        igh_msg_buffer[1] = i; // add length

        Serial.print("{");
        for( uint8_t k = 0; k < i; k++ )
        {
            if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(igh_msg_buffer[k], HEX);
        }
        Serial.print("}\n");

        // publish the data or store it if the publish fails
        igh_mqtt_publish_data( igh_msg_buffer, (unsigned int)i );

    }
}

void igh_app_send_device_restart( void )
{
    if( true == device_restart )
    {
        uint8_t i = 0; // keep track of pkt data
        uint8_t restart_msg[3];

        // first clear the buffer
        memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );

        // Add Retart Message
        restart_msg[0] = RESTART;
        restart_msg[1] = SIZE_OF_RESTART;
        restart_msg[2] = true;

        // Add frame start
        igh_msg_buffer[i++] = FRAME_START;
        i++; // leave room for pkg length

        i = igh_app_add_message_header( igh_msg_buffer, i, ERROR_MSG, IGH_UPLOAD );
        i = igh_app_add_payload( igh_msg_buffer, i, restart_msg, sizeof(restart_msg) );

        // Add Frame End 
        igh_msg_buffer[i++] = FRAME_END;

        igh_msg_buffer[1] = i; // add length

        Serial.print("{");
        for( uint8_t k = 0; k < i; k++ )
        {
            if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(igh_msg_buffer[k], HEX);
        }
        Serial.print("}\n");

        // publish the data or store it if the publish fails
        igh_mqtt_publish_data( igh_msg_buffer, (unsigned int)i );

        device_restart = false;
    }
}

uint8_t igh_app_add_message_header( uint8_t *_buffer, uint8_t start, igh_msg_type msg_type, igh_msg_dir dir )
{
    uint8_t i = start;

    // Add Message type
    _buffer[i++] = ERROR_MSG;

    // Add direction
    _buffer[i++] = IGH_UPLOAD;

    // Add shield serial number
    memcpy( &_buffer[i], 
            igh_current_system_settings.serial_number, 
            sizeof(igh_current_system_settings.serial_number) );

    i += sizeof(igh_current_system_settings.serial_number);

    // Add incremental message ID
    static uint8_t message_id = 0;
    _buffer[i++] = message_id++;

    return i;
}

uint8_t igh_app_add_payload( uint8_t *_buffer, uint8_t start, uint8_t * _payload, uint8_t _payload_len )
{
    uint32_t timestamp_to_store = (uint32_t)igh_boron_unix_time();
    float battery_soc_float = igh_boron_SoC();
    uint32_t battery_soc;
    memcpy(&battery_soc, &battery_soc_float, sizeof battery_soc);
    uint32_t total_water_dispensed = 12364897; // replace with actual reading

    uint8_t i = start;

    // Add DATA PKT PID
    _buffer[i++] = DATA_PKT;
    i++; // leave room for payload length

    // Add Timestamp
    _buffer[i++] = STORE_TIMESTAMP;
    _buffer[i++] = SIZE_OF_STORE_TIMESTAMP;
    _buffer[i++] = (uint8_t)timestamp_to_store;
    _buffer[i++] = (uint8_t)( timestamp_to_store >> 8);
    _buffer[i++] = (uint8_t)( timestamp_to_store >> 16);
    _buffer[i++] = (uint8_t)( timestamp_to_store >> 24);

    // Add Shield battery level
    _buffer[i++] = SHIELD_BATTERY_LEVEL;
    _buffer[i++] = SIZE_OF_SHIELD_BATTERY_LEVEL;
    _buffer[i++] = (uint8_t)battery_soc;
    _buffer[i++] = (uint8_t)( battery_soc >> 8);
    _buffer[i++] = (uint8_t)( battery_soc >> 16);
    _buffer[i++] = (uint8_t)( battery_soc >> 24);

    // Add Valve position
    _buffer[i++] = VALVE_POSITION;
    _buffer[i++] = SIZE_OF_VALVE_POSITION;
    _buffer[i++] = current_valve_position;

    // Add water dispensed 
    _buffer[i++] = WATER_DISPENSED;
    _buffer[i++] = SIZE_OF_WATER_DISPENSED;
    _buffer[i++] = (uint8_t)total_water_dispensed;
    _buffer[i++] = (uint8_t)( total_water_dispensed >> 8);
    _buffer[i++] = (uint8_t)( total_water_dispensed >> 16);
    _buffer[i++] = (uint8_t)( total_water_dispensed >> 24);

    // only add payload if the package fits, prevvent memory leak
    if( (_payload_len + i + 1) < MESSAGE_SIZE )
    {
        memcpy(&_buffer[i], _payload, _payload_len);
        i += _payload_len;
    }

    // add the payload length
    _buffer[ start + 1 ] = i - start - 2;

    return i;
}

void igh_spear_payload_get_new_settings( void )
{
    memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );
    uint8_t rx_bytes = igh_eeprom_get_serial_hex_data(igh_msg_buffer, sizeof(igh_msg_buffer));
    if ( 0 < rx_bytes ) // via Serial
    {

        Serial.print("SERIAL DATA SETTINGS --> {");
        for( uint8_t k = 0; k < rx_bytes; k++ )
        {
            if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(igh_msg_buffer[k], HEX);
        }
        Serial.print("}\n");

        igh_message_process_incoming_msg( igh_msg_buffer );
        igh_settings_process_settings_tuples( igh_msg_buffer, 2, rx_bytes);
    }
}

uint8_t igh_eeprom_get_serial_hex_data( uint8_t * buffer, uint8_t len )
{
    uint8_t ret = 0;
    uint8_t bytes_available = Serial.available();

    if( bytes_available > 0 && 
        bytes_available <= len )
    {
        Serial.readBytes( (char *)buffer, len );

        Serial.print("SERIAL DATA SETTINGS --> {");
        for( uint8_t k = 0; k < bytes_available; k++ )
        {
            if( buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(buffer[k], HEX);
        }
        Serial.print("}\n");

        ret = bytes_available;
    }
    else if( bytes_available > len )
    {
        while( bytes_available )
        {
            Serial.read(); // clear out the buffer but still return false
        }
    }

    return ret;
}


