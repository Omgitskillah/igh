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
#include "particle_api/igh_sd_log.h"
#include "particle_api/igh_hardware.h"

#define JAN_01_2020 1577836800

unsigned long log_service_timer = 0;
uint8_t device_restart = 1;
extern uint8_t igh_msg_buffer[MESSAGE_SIZE]; 

//temp
unsigned long water_disp_timer = millis();

uint8_t igh_app_add_payload( uint8_t *_buffer, uint8_t start, uint8_t * _payload, uint8_t _payload_len );
uint8_t igh_app_add_message_header( uint8_t *_buffer, uint8_t start, igh_msg_type msg_type, igh_msg_dir dir );
void igh_app_send_device_restart( void );
void igh_app_receive_and_stage_sensor_data( void );
uint8_t igh_app_get_serial_hex_data( uint8_t * buffer, uint8_t len );
void igh_app_get_new_settings( void );
void igh_app_commit_new_settings( void );
void igh_app_log_service( void );

void igh_app_setup( void )
{
    Serial.begin(19200);
    
    igh_boron_setup();

    // setup hardwar
    igh_hardware_setup();
    
    // get settings from eeprom
    igh_eeprom_init();
    
    // starte the radio
    igh_rfm69_setup();
    
    // setup MQTT
    igh_mqtt_setup();

    // setup the SD card
    igh_sd_log_setup();
}

void igh_main_application( void )
{
    // check if there are any settings to read
    igh_app_get_new_settings();

    // commit new settings if available
    igh_app_commit_new_settings();

    // process boron service
    igh_boron_service();
    
    // Send or store device restart message
    igh_app_send_device_restart();
    
    // Send or Store Spear data
    igh_app_receive_and_stage_sensor_data();

    // Run MQTT service in background
    igh_mqtt_service();

    // rfm69 service in the background
    igh_rfm69_service();

    // manage data on SD card
    igh_app_log_service();

}

void igh_app_receive_and_stage_sensor_data( void )
{
    uint8_t rx_buffer[RF69_MAX_DATA_LEN];
    uint8_t data_rx_len = 0;

    data_rx_len = igh_rfm69_receive_raw_bytes( rx_buffer, sizeof(rx_buffer) );

    if( 0 != data_rx_len )
    {
        igh_boron_toggle_boron_led(ON);

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

        Serial.print("\nTIME: "); Serial.println(igh_boron_unix_time(), HEX);

        // Serial.print("{");
        // for( uint8_t k = 0; k < i; k++ )
        // {
        //     if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
        //     Serial.print(igh_msg_buffer[k], HEX);
        // }
        // Serial.print("}\n");

        // publish the data or store it if the publish fails
        uint32_t current_time = igh_boron_unix_time();
        
        // Log data only if the time is synced
        if( JAN_01_2020 < current_time )
        igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );
        
        igh_boron_toggle_boron_led(OFF);
    }
}

void igh_app_send_device_restart( void )
{
    uint32_t current_time = igh_boron_unix_time();

    // only ssave a restart message if the time is synced. Otherwise wait till that happens
    if( (true == device_restart)
        && (JAN_01_2020 < current_time) )
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

        // Serial.print("{");
        // for( uint8_t k = 0; k < i; k++ )
        // {
        //     if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
        //     Serial.print(igh_msg_buffer[k], HEX);
        // }
        // Serial.print("}\n");

        // save restart message
        igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );

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
            boron_serial_number, 
            sizeof(boron_serial_number) );

    i += sizeof(boron_serial_number);

    // Add incremental message ID
    static uint8_t message_id = 0;
    _buffer[i++] = message_id++;

    return i;
}

uint8_t igh_app_add_payload( uint8_t *_buffer, uint8_t start, uint8_t * _payload, uint8_t _payload_len )
{
    uint32_t timestamp_to_store = igh_boron_unix_time();
    float battery_soc_float = igh_boron_SoC();
    uint32_t battery_soc;
    memcpy(&battery_soc, &battery_soc_float, sizeof battery_soc);

    uint8_t i = start;

    // Add DATA PKT PID
    _buffer[i++] = DATA_PKT;
    i++; // leave room for payload length

    // Add Shield ID
    _buffer[i++] = SHIELD_ID;
    _buffer[i++] = sizeof(boron_serial_number);
    memcpy(&_buffer[i], igh_current_system_settings.serial_number, sizeof(igh_current_system_settings.serial_number) );
    i += sizeof(boron_serial_number);

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
    uint32_t curr_water_L = 0;
    memcpy( &curr_water_L, &total_water_dispensed_Liters, sizeof curr_water_L );
    _buffer[i++] = WATER_DISPENSED;
    _buffer[i++] = SIZE_OF_WATER_DISPENSED;
    _buffer[i++] = (uint8_t)curr_water_L;
    _buffer[i++] = (uint8_t)( curr_water_L >> 8);
    _buffer[i++] = (uint8_t)( curr_water_L >> 16);
    _buffer[i++] = (uint8_t)( curr_water_L >> 24);

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

void igh_app_get_new_settings( void )
{
    memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );
    uint8_t rx_bytes = igh_app_get_serial_hex_data(igh_msg_buffer, sizeof(igh_msg_buffer));
    if ( 0 < rx_bytes ) // via Serial
    {

        Serial.print("SERIAL DATA SETTINGS --> {");
        for( uint8_t k = 0; k < rx_bytes; k++ )
        {
            if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(igh_msg_buffer[k], HEX);
        }
        Serial.print("}\n");

        if( IGH_SEND_SETTINGS == igh_msg_buffer[0] )
        {
            if( true == igh_settings_process_settings_tuples( igh_msg_buffer, 2, rx_bytes) )
            {
                new_settings_available = 1;
                // update the checksum of the system settings
                igh_current_system_settings.checksum = igh_settings_calculate_checksum(&igh_current_system_settings, sizeof(igh_current_system_settings));
                // update the checksum for the threshold settings
                igh_current_threshold_settings.checksum = igh_settings_calculate_checksum(&igh_current_threshold_settings, sizeof(igh_current_threshold_settings));
            }
            else
            {
                Serial.println("SETTINGS FAILED");
            }
            
        }
    }
}

void igh_app_commit_new_settings( void )
{
    if( 1 == new_settings_available )
    {
        Serial.print("OP STATE:"); Serial.println(igh_current_system_settings.op_state);
        Serial.print("REPORTING INTERVAL: "); Serial.println(igh_current_system_settings.reporting_interval);
        Serial.print("DATA RESOLUTION: "); Serial.println(igh_current_system_settings.data_resolution);
        Serial.print("SERIAL NUMBER: ");
        for( uint8_t i = 0; i < sizeof(igh_default_system_settings.serial_number); i++ )
        {
            if( igh_current_system_settings.serial_number[i] <= 0x0F ) Serial.print("0");
            Serial.print(igh_current_system_settings.serial_number[i], HEX);
        }
        Serial.print("\n");
        Serial.print("MQTT BROKER: "); Serial.println((char *)igh_current_system_settings.broker);
        Serial.print("MQTT BROKER PORT: "); Serial.println(igh_current_system_settings.broker_port);
        Serial.print("TIMEZONE: "); Serial.println(igh_current_system_settings.timezone);
        Serial.print("IRRIGATION HOUR: "); Serial.println(igh_current_system_settings.irrigation_hr);
        Serial.print("CHECKSUM: "); Serial.println(igh_current_system_settings.checksum);

        if ( true == igh_eeprom_save_system_settings( &igh_current_system_settings) )
        {
            Serial.println("Settings Saved successfully");
        }

        new_settings_available = 0;
    }
}

uint8_t igh_app_get_serial_hex_data( uint8_t * buffer, uint8_t len )
{
    uint8_t ret = 0;

    uint8_t bytes_available = Serial.available();
    if( bytes_available > 0 )
    {
        if( bytes_available <= len )
        {
            for( uint8_t i = 0; i < bytes_available; i++ )
            {
                // read all available bytes
                buffer[i] = Serial.read();
            }
            ret = bytes_available;
        }
        else
        {
            for( uint8_t i = 0; i < bytes_available; i++ )
            {
                // dump the data
                Serial.read();
            }
        }
    }
    return ret;
}

void igh_app_log_service( void )
{
    if( (millis() - log_service_timer) > ONE_SECOND )
    {
        // throttle sending data
        // Serial.print(".");
        if( 1 == mqtt_connected )
        {
            char next_file[FILE_NAME_SIZE];
            if( true == igh_sd_log_get_next_file_name(next_file) )
            {
                uint8_t sd_data_point[MAX_FILE_SIZE];
                if( true == igh_sd_log_read_data_point(next_file, sd_data_point, MAX_FILE_SIZE) )
                {
                    Serial.print("Uploading: "); Serial.print((String)next_file);
                    
                    // Serial.print("PAYLOAD: {");
                    // for(uint8_t i = 0; i < sd_data_point[1]; i++ )
                    // {
                    //     if( sd_data_point[i] <= 0x0F ) Serial.print("0");
                    //     Serial.print( sd_data_point[i], HEX );
                    // }
                    // Serial.println("}");

                    if( true == igh_mqtt_publish_data(sd_data_point, sd_data_point[1]) )
                    {
                        if( true == igh_sd_log_remove_data_point(next_file) ) 
                        {
                            Serial.println(" OK");
                        }
                        else
                        {
                            Serial.println(" DEL ERROR");
                        }
                    }
                    else
                    {
                        Serial.println(" MQTT ERROR");
                    }
                    
                }
            }
            else
            {
                // Serial.println("No new file to send");
            }
        }

        log_service_timer = millis();
    }
}


