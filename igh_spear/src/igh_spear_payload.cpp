/*******************************************************************************
 * @file igh_spear_payload.h
 * @brief API for building the igh payload from spear to shield
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "igh_spear_payload.h"
#include "igh_spear_log.h"

#include "igh_spear_hardware.h"
#include "igh_spear_settings.h"
#include "igh_spear_rfm69.h"

#include "igh_spear_soil_moisture_sensor.h"
#include "igh_spear_lux_meter.h"
#include "igh_spear_sht10.h"
#include "igh_spear_dht22.h"
#include "igh_spear_mhz19.h"
#include "igh_spear_payload.h"



/* uncomment to enable debug */
#define LOG_IGH_SPEAR_PAYLOAD
//igh_spear_log("SETTINGS...............OK\n");

/**
 *  this is limited by the maximum 
 * bytes the RFM module can transmit 
 * with encryption
 * */
#define SIZE_OF_MSG_HEADER    2
#define SIZE_OF_RF_ID         2
#define SIZE_OF_SEND_INTERVAL 4
#define MAX_PAYLOAD_LENGTH    60 
#define ONE_SECOND            1000
#define SETTINGS_CLIENT       1984

uint8_t resp[] = "<SETTINGS:OK>";

unsigned long payload_tick = 0;
unsigned long payload_millis_counter = 0;

//global variables
uint8_t payload_scratchpad[MAX_PAYLOAD_LENGTH];

igh_spear_sensor_data  payload_data_store[NUM_OF_SENSORS];

void igh_spear_payload_collect_sensor_data( void );
uint8_t igh_spear_payload_parse_new_settings( uint8_t * data );
void igh_spear_payload_get_new_settings( void );

uint8_t igh_spear_payload_build_pkt( void )
{
    uint8_t i = 0; // data counter
    
    // clean scratchpad
    memset(payload_scratchpad, 0, sizeof(payload_scratchpad));

    // add spear data id
    payload_scratchpad[i++] = SPEAR_DATA; 
    i++;// leave room for length

    // add id
    payload_scratchpad[i++] = SPEAR_ID; 
    payload_scratchpad[i++] = sizeof(active_system_setting.serial_number); 
    memcpy(&payload_scratchpad[i], active_system_setting.serial_number, sizeof(active_system_setting.serial_number)); 
    i += sizeof(active_system_setting.serial_number);

    // add sensor data
    uint8_t sensor_count = 0;
    for( sensor_count; sensor_count < NUM_OF_SENSORS; sensor_count++ )
    {
        if( true == payload_data_store[sensor_count].new_data )
        {
            payload_scratchpad[i++] = payload_data_store[sensor_count].id;
            payload_scratchpad[i++] = sizeof(payload_data_store[sensor_count].bytes);
            
            memcpy(&payload_scratchpad[i], payload_data_store[sensor_count].bytes, 
                    sizeof(payload_data_store[sensor_count].bytes));

            i += sizeof(payload_data_store[sensor_count].bytes);

            // don't send old data
            payload_data_store[sensor_count].new_data = false;
        }
    }

    // add the pkt length
    payload_scratchpad[1] = i;

    return i;
}

void igh_spear_payload_tick( void )
{
    if( (millis() - payload_millis_counter) >= ONE_SECOND )
    {
        payload_tick++;
#ifdef LOG_IGH_SPEAR_PAYLOAD
        igh_spear_log(".");
#endif
        payload_millis_counter = millis();
    }

    if( payload_tick >= active_system_setting.data_collection_interval )
    {
        payload_tick = 0;
        uint16_t parent_shield = 0; // by default. 
        uint8_t num_bytes_to_send = 0;

        // collect data
        igh_spear_payload_collect_sensor_data();

        // build the data
        num_bytes_to_send = igh_spear_payload_build_pkt();
        
        // if it is time, send it into the ether, don't store data on the spear
#ifdef LOG_IGH_SPEAR_PAYLOAD
        igh_spear_log("\nPAYLOAD --> ");
        for( int i = 0; i < num_bytes_to_send; i++ )
        {
            sprintf(debug_buff, "%02X", payload_scratchpad[i]);
            igh_spear_log(debug_buff);
        }
        igh_spear_log("\n");
#endif
        igh_spear_rfm69_send_2_shield( parent_shield, payload_scratchpad, num_bytes_to_send );
    }

    // process any new settings messages
    igh_spear_payload_get_new_settings();
}

void igh_spear_payload_get_new_settings( void )
{
    uint8_t _buffer[MAX_PAYLOAD_LENGTH];
    if ( SETTINGS_CLIENT == igh_spear_rfm69_process_incoming_msg( _buffer ) )
    {
        // only process seetings from specific rf device
        if( _buffe[0] != IGH_SEND_SETTINGS ) return;  // do nothing if first byte is not a rx settings command

        uint8_t len = _buffer[1];

        uint8_t i = 2;

        while( i < len )
        {
            uint8_t processed_len = igh_spear_payload_parse_new_settings( &buffer[i]);

            if( 0 == processed_len )
            {
                // drop message because the payload is unknown
                return;
            }
            // else, move on to next message
            i += processed_len;
        }

        uint8_t new_checksum = igh_spear_settings_calculate_checksum( &new_system_settings, sizeof(new_system_settings));
        new_system_settings.checksum = new_checksum;

        if( true == igh_spear_settings_save(new_system_settings) )
        {
            igh_spear_settings_read( &active_system_setting );

#ifdef LOG_IGH_SPEAR_PAYLOAD
            igh_spear_log("SETTINGS USED\nSN:");
            for( int i = 0; i < sizeof(active_system_setting.serial_number); i++)
            {
                sprintf(debug_buff, "%02X", active_system_setting.serial_number[i]);
            }
            igh_spear_log(debug_buff);
            sprintf(debug_buff, "SHIELD ID: %d\nSPEAR ID: %d\nDATA INTERVAL: %d\n", 
                    active_system_setting.parent_shield_rf_id,
                    active_system_setting.spear_rf_id,
                    active_system_setting.data_collection_interval);
            igh_spear_log(debug_buff);
#endif
        }
    }
}

uint8_t igh_spear_payload_parse_new_settings( uint8_t * data )
{
    uint8_t len = data[1];
    uint8_t pkt_len = 0;
    switch( data[0] )
    {
        case SPEAR_ID:
            if( len == sizeof(new_system_settings.serial_number) )
            {
                memcpy(new_system_settings.serial_number, &data[2], len);
                pkt_len = sizeof(new_system_settings.serial_number) + SIZE_OF_MSG_HEADER;
            }
            break;
        case SPEAR_RF_ID:
            if( len == SIZE_OF_RF_ID )
            {
                uint16_t new_id = 0;
                new_id = data[2] | ( 8 << data[3] );
                new_system_settings.spear_rf_id = new_id;
                pkt_len = SIZE_OF_RF_ID + SIZE_OF_MSG_HEADER;
            }
            break;
        case SHIELD_RF_ID:
            if( len == SIZE_OF_RF_ID )
            {
                uint16_t new_id = 0;
                new_id = data[2] | ( 8 << data[3] );
                new_system_settings.parent_shield_rf_id = new_id;
                pkt_len = SIZE_OF_RF_ID + SIZE_OF_MSG_HEADER;
            }
            break;
        case SEND_INTERVAL:
            if( len == SIZE_OF_SEND_INTERVAL )
            {
                uint16_t new_id = 0;
                new_id = data[2] | ( 8 << data[3] ) | ( 16 << data[4] ) | ( 24 << data[5] );
                new_system_settings.parent_shield_rf_id = new_id;
                pkt_len = SIZE_OF_SEND_INTERVAL + SIZE_OF_MSG_HEADER;
            }
            break;
        default:
            // ???
            break;
    }
    return pkt_len;
}

/* data collection routines */
void igh_spear_payload_collect_sensor_data( void )
{
    igh_spear_hardware_battery_service();
    igh_spear_lux_meter_service();
    igh_spear_soil_mousture_service();
    igh_spear_sht10_service();
    igh_spear_dht22_service();
    igh_spear_mhz19_service();
}
