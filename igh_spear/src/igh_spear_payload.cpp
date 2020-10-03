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

/* uncomment to enable debug */
#define LOG_IGH_SPEAR_PAYLOAD
//igh_spear_log("SETTINGS...............OK\n");

/**
 *  this is limited by the maximum 
 * bytes the RFM module can transmit 
 * with encryption
 * */
#define MAX_PAYLOAD_LENGTH 60 
#define ONE_SECOND         1000

unsigned long payload_tick = 0;;
unsigned long payload_millis_counter = 0;

//global variables
uint8_t payload_scratchpad[MAX_PAYLOAD_LENGTH];

igh_spear_sensor_data  payload_data_store[NUM_OF_SENSORS];

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
        if( payload_data_store[sensor_count].new_data )
        {
            payload_scratchpad[i++] = payload_data_store[sensor_count].id;
            payload_scratchpad[i++] = sizeof(payload_data_store[sensor_count].bytes);
            
            memcpy(&payload_scratchpad[i], payload_data_store[sensor_count].bytes, 
                    sizeof(payload_data_store[sensor_count].bytes));

            i += sizeof(payload_data_store[sensor_count].bytes);
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
    }

    if( payload_tick >= active_system_setting.data_collection_interval )
    {
        uint16_t parent_shield = 0; // by default. 
        uint8_t num_bytes_to_send = 0;
        num_bytes_to_send = igh_spear_payload_build_pkt();
        // if it is time, send it into the ether, don't store data on the spear
#ifdef LOG_IGH_SPEAR_PAYLOAD
        igh_spear_log("PAYLOAD --> ");
        for( int i = 0; i < num_bytes_to_send; i++ )
        {
            sprintf(debug_buff, "%02X", dht22_temperature, dht22_humidity);
            igh_spear_log(debug_buff);
        }
        igh_spear_log("\n");
#endif
        igh_spear_rfm69_send_2_shield( parent_shield, payload_scratchpad, num_bytes_to_send );
    }
}