/*******************************************************************************
 * @file igh_message.cpp
 * @brief manage messages on the platform
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_shield.h"
#include "include/igh_fwv.h"
#include "include/igh_settings.h"
#include "include/igh_message.h"
#include "include/igh_valve.h"
#include "include/igh_water_flow_meter.h"
#include "include/igh_irrigation.h"
#include "particle_api/igh_rfm69.h"
#include "particle_api/igh_sd_log.h"
#include "particle_api/igh_mqtt.h"
#include "particle_api/igh_hardware.h"
#include "particle_api/igh_boron.h"
#include "particle_api/igh_eeprom.h"

#define THIRTY_MINS 1800000

const uint8_t fw_ver[] = {
#include "./version.inc"
};

uint8_t igh_msg_buffer[MESSAGE_SIZE];
void igh_message_publish_built_payload( uint32_t current_time, uint8_t * buffer, uint8_t msg_len, bool store_data_point );
uint8_t igh_message_get_serial_hex_data( uint8_t * buffer, uint8_t len );
void igh_message_print_valid_settings( void );
void igh_message_parse_current_humidity( uint8_t * incoming_data );
void igh_message_process_sd_data( void );
void igh_message_setup_home_ping( void );
uint8_t igh_message_process_settings_request(uint8_t * settings);
bool igh_message_build_settings_request_packet( uint8_t * request );

Timer message_store_timer( ONE_SECOND, igh_message_process_sd_data );
Timer messag_ping_home_timer( THIRTY_MINS, igh_message_ping_home );

void igh_message_setup( void )
{
    if( message_store_timer.isActive() )
    {
        message_store_timer.stop();
    }
    message_store_timer.start();
}

void igh_message_setup_home_ping( void )
{
    if( messag_ping_home_timer.isActive() )
    {
        messag_ping_home_timer.stop();
    }
    messag_ping_home_timer.changePeriod(igh_current_system_settings.reporting_interval);
    messag_ping_home_timer.start();
}

void igh_message_build_payload( igh_msg_type msg_type, uint8_t * payload, uint8_t payload_len, bool store_data_point )
{
    /**
     * | Start         | length | Msg type | direction | s/n      | msg id | payload  | end            |  
     * | :---          | :----: | :----:   | :----:    | :----:   | :----: | :----:   | ---:           |  
     * | FRAME_START   | 1 byte | 1 byte   | MSG_DIR   | 12 bytes | 1 byte | variable | FRAME_END      |
     */
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t payload_len_index = 0;

    // first clear the buffer
    memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );
    // Add frame start
    igh_msg_buffer[i++] = FRAME_START;
    i++; // leave room for pkg length
    // message type
    igh_msg_buffer[i++] = msg_type;
    // direction
    igh_msg_buffer[i++] = IGH_UPLOAD;
    // Add shield serial number
    memcpy( &igh_msg_buffer[i], 
            boron_serial_number, 
            sizeof(boron_serial_number) );
    i += sizeof(boron_serial_number);
    // Add incremental message ID
    static uint8_t message_id = 0;
    igh_msg_buffer[i++] = message_id++;

    // Add DATA PKT PID
    igh_msg_buffer[i++] = DATA_PKT;
    payload_len_index = i;
    i++; // leave room for payload length
    j = i;
    // Add Shield ID
    igh_msg_buffer[i++] = SHIELD_ID;
    igh_msg_buffer[i++] = sizeof(boron_serial_number);
    memcpy(&igh_msg_buffer[i], igh_current_system_settings.serial_number, sizeof(igh_current_system_settings.serial_number) );
    i += sizeof(igh_current_system_settings.serial_number);
    // Add Timestamp
    uint32_t timestamp_to_store = igh_boron_unix_time();
    igh_msg_buffer[i++] = STORE_TIMESTAMP;
    igh_msg_buffer[i++] = SIZE_OF_STORE_TIMESTAMP;
    igh_msg_buffer[i++] = (uint8_t)timestamp_to_store;
    igh_msg_buffer[i++] = (uint8_t)( timestamp_to_store >> 8);
    igh_msg_buffer[i++] = (uint8_t)( timestamp_to_store >> 16);
    igh_msg_buffer[i++] = (uint8_t)( timestamp_to_store >> 24);
    // Add fw version
    igh_msg_buffer[i++] = SHIELD_FW_VERSION;
    igh_msg_buffer[i++] = SIZE_OF_FW_VERSION;
    igh_msg_buffer[i++] = fw_ver[0];
    igh_msg_buffer[i++] = fw_ver[1];
    igh_msg_buffer[i++] = fw_ver[2];
    // Add Shield battery level
    float battery_soc_float = igh_boron_SoC();
    uint32_t battery_soc;
    memcpy(&battery_soc, &battery_soc_float, sizeof battery_soc);
    igh_msg_buffer[i++] = SHIELD_BATTERY_LEVEL;
    igh_msg_buffer[i++] = SIZE_OF_SHIELD_BATTERY_LEVEL;
    igh_msg_buffer[i++] = (uint8_t)battery_soc;
    igh_msg_buffer[i++] = (uint8_t)( battery_soc >> 8);
    igh_msg_buffer[i++] = (uint8_t)( battery_soc >> 16);
    igh_msg_buffer[i++] = (uint8_t)( battery_soc >> 24);
    // Add Valve position
    igh_msg_buffer[i++] = VALVE_POSITION;
    igh_msg_buffer[i++] = SIZE_OF_VALVE_POSITION;
    igh_msg_buffer[i++] = (uint8_t)current_valve_ctrl.valve_state;
    // Add water dispensed 
    uint32_t curr_water_L = 0;
    float total_water_dispensed_snapshot = total_water_dispensed_Liters;
    memcpy( &curr_water_L, &total_water_dispensed_snapshot, sizeof curr_water_L );
    igh_msg_buffer[i++] = WATER_DISPENSED;
    igh_msg_buffer[i++] = SIZE_OF_WATER_DISPENSED;
    igh_msg_buffer[i++] = (uint8_t)curr_water_L;
    igh_msg_buffer[i++] = (uint8_t)( curr_water_L >> 8);
    igh_msg_buffer[i++] = (uint8_t)( curr_water_L >> 16);
    igh_msg_buffer[i++] = (uint8_t)( curr_water_L >> 24);
    // only add payload if the package fits, prevvent memory leak
    if( (payload_len + i + 1) < MESSAGE_SIZE )
    {
        memcpy(&igh_msg_buffer[i], payload, payload_len);
        i += payload_len;
    }
    else
    {
        // dump the message if it cannot fit
#ifdef IGH_DEBUG
        Serial.println("MESSAGING ERROR: PACKET TOO LONG");
#endif
        memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );
        return;
    }
    // add the payload length
    igh_msg_buffer[payload_len_index] = i - j;
    // Add Frame End 
    igh_msg_buffer[i++] = FRAME_END;
    // add message length
    igh_msg_buffer[LEN_INDEX] = i;

    igh_message_publish_built_payload( timestamp_to_store,
                                       igh_msg_buffer,
                                       i,
                                       store_data_point );
}

void igh_message_publish_built_payload( uint32_t current_time, uint8_t * buffer, uint8_t msg_len, bool store_data_point )
{
    if( true == Time.isValid() )
    {
        // only publish valid data

        if( true == mqtt_connected )
        {
            igh_mqtt_publish_data(buffer, msg_len);
        }
        else
        {
            if( true == store_data_point )
            {
                // store data
                if( false == igh_sd_log_save_data_point( (unsigned long)current_time, buffer, msg_len ) )
                {
#ifdef IGH_DEBUG
                    Serial.println("EVENT: SD WRITE ERROR");
#endif
                    igh_message_event(EVENT_SD_CARD_ERROR, false);
                }
            }
        }
    }
}

bool igh_message_build_settings_request_packet( uint8_t * request )
{
    bool ret = false;
    uint8_t settings_read[MESSAGE_SIZE];
    uint8_t settings_request_start_index = PAYLOAD_INDEX + 2;

    uint8_t read_settings_len = igh_settings_build_settings_request_payload( request, settings_read, settings_request_start_index);

    if( 0 != read_settings_len )
    {
        // send the data back to the broker
        igh_message_build_payload( SETTINGS_MSG, settings_read, read_settings_len, true );
        ret = true;
    }

    return ret;
}

void igh_message_event( igh_event_id_e event, bool store_data_point )
{
    uint8_t event_payload[EVENT_PAYLOAD_LEN];
    event_payload[0] = EVENT;
    event_payload[0] = SIZE_OF_EVENT;
    event_payload[0] = (uint8_t)event;

    igh_message_build_payload( EVENT_MSG, event_payload, EVENT_PAYLOAD_LEN, store_data_point );
}

void igh_message_ping_home( void )
{
#ifdef IGH_DEBUG
    Serial.println("EVENT: PING HOME");
#endif
    igh_message_event( EVENT_CALL_HOME, true );
}

uint8_t igh_message_get_serial_hex_data( uint8_t * buffer, uint8_t len )
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

uint8_t igh_message_process_mqtt_data( uint8_t * buffer, uint8_t incoming_len )
{
    igh_msg_type message_type = UNKNOWN_MSG;
    uint8_t length = buffer[LEN_INDEX]; // get the length

    if( (length != 0) && 
        ( length <= incoming_len) )
    {
        if( (buffer[0] == FRAME_START) && (buffer[length-1] == FRAME_END) )
        {
            /**check the first and last byte*/
            // check the serial number
            if( 0 != memcmp(boron_serial_number, &buffer[SN_INDEX], sizeof(boron_serial_number)))
            {
                // if the serial number does not match, do nothing as this message wasn't meant for this device,
                // This ideally should never happen
#ifdef IGH_DEBUG
                Serial.println("EVENT: WRONG MQTT DEVICE COMMAND");
#endif
                igh_message_event(EVENT_CMD_SENT_TO_WRONG_DEVICE, true);
            }
            else
            {
                if( SETTINGS_MSG == buffer[MSG_TYPE_INDEX] )
                {
                    // process settings here
                    if(igh_settings_process_settings(buffer))
                    {
                        message_type = SETTINGS_MSG;
                    }
                }
                if( SETTINGS_REQUEST == buffer[MSG_TYPE_INDEX] )
                {
                    // process settings here
                    if(igh_message_process_settings_request(buffer))
                    {
                        message_type = SETTINGS_REQUEST;
                    }
                }
                else
                {
#ifdef IGH_DEBUG
                    Serial.println("EVENT: UNKNOWN MQTT COMMAND");
#endif                    
                    igh_message_event(EVENT_UNKNOWN_MQTT_CMD, true);
                }
            }
        }
        else
        {
#ifdef IGH_DEBUG
            Serial.println("EVENT: UNKNOWN MQTT COMMAND");
#endif 
            igh_message_event(EVENT_UNKNOWN_MQTT_CMD, true);
        }
        
    }
    else
    {
#ifdef IGH_DEBUG
        Serial.println("EVENT: UNKNOWN MQTT COMMAND");
#endif 
        igh_message_event(EVENT_UNKNOWN_MQTT_CMD, true);
    }

    return message_type; // should return the extracted tuple id for processing later
}

uint8_t igh_message_process_settings_request(uint8_t * settings)
{
    uint8_t ret = 0; 
    igh_pkt_id settings_type = (igh_pkt_id)settings[PAYLOAD_INDEX];
    switch(settings_type)
    {
        case IGH_READ_SETTINGS:
            ret = igh_message_build_settings_request_packet(settings);
            break;
        default:
            break;
    }
    return ret;
}

void igh_message_get_new_settings( void )
{
    memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );
    uint8_t rx_bytes = igh_message_get_serial_hex_data(igh_msg_buffer, sizeof(igh_msg_buffer));
    if ( 0 < rx_bytes ) // via Serial
    {
#ifdef IGH_DEBUG
        Serial.print("SERIAL DATA SETTINGS --> {");
        for( uint8_t k = 0; k < rx_bytes; k++ )
        {
            if( igh_msg_buffer[k] <= 0x0F ) Serial.print("0");
            Serial.print(igh_msg_buffer[k], HEX);
        }
        Serial.print("}\n");
#endif

        if( IGH_SEND_SETTINGS == igh_msg_buffer[0] )
        {
            if( true == igh_settings_process_settings_tuples( igh_msg_buffer, 2, rx_bytes) )
            {
                new_settings_available = 1;
                // update the checksum of the system settings
                igh_current_system_settings.checksum = igh_settings_calculate_checksum(&igh_current_system_settings, sizeof(igh_current_system_settings));
                // update the checksum for the threshold settings
                igh_current_threshold_settings.checksum = igh_settings_calculate_checksum(&igh_current_threshold_settings, sizeof(igh_current_threshold_settings));
#ifdef IGH_DEBUG
                Serial.println("EVENT: SETTINGS UPDATED SUCCESSFULY");
#endif 
                igh_message_event( EVENT_SETTINGS_UPDATE_SUCCESS, true );
            }
            else
            {
#ifdef IGH_DEBUG
                Serial.println("EVENT: SETTINGS FAILED");
#endif
                igh_message_event( EVENT_SETTINGS_UPDATE_FAIL, true );
            }
            
        }
        else if( (igh_msg_buffer[0] == '?') &&
                 (igh_msg_buffer[1] == '?') &&
                 (igh_msg_buffer[2] == '?') )
        {
            igh_message_print_valid_settings();
        }
        else if( (igh_msg_buffer[0] == 'R') &&
                 (igh_msg_buffer[1] == 'E') &&
                 (igh_msg_buffer[2] == 'S') &&
                 (igh_msg_buffer[3] == 'E') &&
                 (igh_msg_buffer[4] == 'T') &&
                 (igh_msg_buffer[5] == '-') &&
                 (igh_msg_buffer[6] == 'I') &&
                 (igh_msg_buffer[7] == 'R') &&
                 (igh_msg_buffer[8] == 'R') )
        {
            // reset irrigation
            igh_water_flow_meter_reset_nv();
        }
        else if( (igh_msg_buffer[0] == 'R') &&
                 (igh_msg_buffer[1] == 'E') &&
                 (igh_msg_buffer[2] == 'S') &&
                 (igh_msg_buffer[3] == 'E') &&
                 (igh_msg_buffer[4] == 'T') &&
                 (igh_msg_buffer[5] == '-') &&
                 (igh_msg_buffer[6] == 'S') &&
                 (igh_msg_buffer[7] == 'Y') &&
                 (igh_msg_buffer[8] == 'S') )
        {
            // reset system
            igh_settings_reset_system_to_default();
            igh_water_flow_meter_reset_nv();
            new_settings_available = 1;
            mqtt_set_broker = 1;
            initialize_rfm69 = 1;
#ifdef IGH_DEBUG
            Serial.println("EVENT: SYSTEM SETTINGS RESET");
#endif             
            igh_message_event( EVENT_SYSTEM_RESET, true );
        }
    }

}

void igh_message_commit_new_settings( void )
{
    if( 1 == new_settings_available )
    {
        igh_message_print_valid_settings();

        if ( true == igh_eeprom_save_system_settings( &igh_current_system_settings) )
        {
#ifdef IGH_DEBUG
            Serial.println("System Settings Saved successfully");
#endif
        }

        if ( true == igh_eeprom_save_threshold_settings( &igh_current_threshold_settings) )
        {
#ifdef IGH_DEBUG
            Serial.println("Threshold Settings Saved successfully");
#endif
        }

        new_settings_available = 0;
    }
}

void igh_message_print_valid_settings( void )
{
#ifdef IGH_DEBUG
    Serial.print("FW VERSION: v"); Serial.print(fw_ver[0]); Serial.print("."); Serial.print(fw_ver[1]); Serial.print("."); Serial.println(fw_ver[2]);
    Serial.print("OP STATE: "); Serial.println(igh_current_system_settings.op_state);
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
    Serial.print("MQTT USERNAME: "); Serial.println((char *)igh_current_system_settings.mqtt_username);
    Serial.print("MQTT PASSWORD: "); Serial.println((char *)igh_current_system_settings.mqtt_password);
    Serial.print("TIMEZONE: "); Serial.println(igh_current_system_settings.timezone);
    Serial.print("IRRIGATION HOUR: "); Serial.println(igh_current_system_settings.irrigation_hr);
    Serial.print("IRRIGATION CLOCK INTERVAL: "); Serial.println(igh_current_system_settings.clock_irrigation_interval);
    Serial.print("WATER DISPENSER PERIOD: "); Serial.println(igh_current_system_settings.water_dispenser_period);
    Serial.print("IRRIGATION TYPE: "); Serial.println(igh_current_system_settings.auto_irrigation_type);
    Serial.print("AMOUNT BUTTON CAN DISPENSE: "); Serial.print(igh_current_system_settings.water_amount_by_button_press); Serial.println("L");
    Serial.print("SYSTEM SETTINGS CHECKSUM: "); Serial.println(igh_current_system_settings.checksum);

    Serial.print("\n\nSOIL HUMIDITY LOW THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_low);
    Serial.print("SOIL HUMIDITY HIGH THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_high);
    Serial.print("MIN WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_low); Serial.println("L");
    Serial.print("MAX WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_high); Serial.println("L");
    Serial.print("THRESHOLDS SETTINGS CHECKSUM: "); Serial.println(igh_current_threshold_settings.checksum);

    Serial.print("TOTAL WATER DISPENSED: "); Serial.print(String(total_water_dispensed_Liters)); Serial.println("L");
#endif
}

void igh_message_receive_and_stage_sensor_data( void )
{
    uint8_t rx_buffer[RF69_MAX_DATA_LEN];
    uint8_t data_rx_len = 0;

    data_rx_len = igh_rfm69_receive_raw_bytes( rx_buffer, sizeof(rx_buffer) );

    if( 0 != data_rx_len )
    {
        igh_boron_toggle_boron_led(ON);

        if( (SPEAR_DATA == rx_buffer[0]) ||
            ( rx_buffer[1] == data_rx_len ))
        {
            // only process valid data
            // read data needed by the shield
            igh_message_parse_current_humidity(rx_buffer);
#ifdef IGH_DEBUG
            Serial.print("SPEAR DATA: ");
            for( uint8_t i = 0; i < sizeof(rx_buffer); i++ )
            {
                Serial.print(rx_buffer[i], HEX);
            }
            Serial.println();
#endif  
            igh_message_build_payload( SENSOR_DATA,
                                       rx_buffer,
                                       data_rx_len,
                                       true );
        }
        igh_boron_toggle_boron_led(OFF);
    }
}

void igh_message_parse_current_humidity( uint8_t * incoming_data )
{
    uint8_t data_len = incoming_data[1];
    uint8_t current_tuple_id = 0;
    uint8_t current_tuple_length = 0;
    uint8_t current_data_index = 0;
    uint8_t byte_tracker = 2;
    uint16_t new_humidity = 0xFFFF;

    while(byte_tracker < data_len)
    {
        // extract tuples
        current_tuple_id = incoming_data[byte_tracker];
        // extract the length
        current_tuple_length = incoming_data[byte_tracker + 1];
        // extract the tuple data based on tuple id
        current_data_index = byte_tracker + 2;

        if( SOIL_HUMIDITY == current_tuple_id )
        {
            if( SIZE_OF_SOIL_HUMIDITY == current_tuple_length )
            {
                uint8_t new_humidity_reading[SIZE_OF_SOIL_HUMIDITY]; 
                memcpy(new_humidity_reading, &incoming_data[current_data_index], SIZE_OF_SOIL_HUMIDITY);
                new_humidity = GET16_LI(new_humidity_reading);
                igh_irrigation_update_sensor_data( new_humidity );
#ifdef IGH_DEBUG
            Serial.print("NEW HUMIDITY DATA: "); Serial.println(new_humidity);
#endif 
            }
            else
            {
                /* Do nothing */
#ifdef IGH_DEBUG
                Serial.println("EVENT: INVALID SOIL DATA");
#endif 
                igh_message_event( EVENT_INVALID_SOIL_DATA, true );
            }
        }
        // move index to next tuple id
        byte_tracker += current_tuple_length + TUPLE_HEADER_LEN;
    }
}

void igh_message_process_sd_data( void )
{
    // throttle sending data
    if( 1 == mqtt_connected )
    {
        char next_file[FILE_NAME_SIZE];
        if( true == igh_sd_log_get_next_file_name(next_file) )
        {
            uint8_t sd_data_point[MAX_FILE_SIZE];
            if( true == igh_sd_log_read_data_point(next_file, sd_data_point, MAX_FILE_SIZE) )
            {
#ifdef IGH_DEBUG
                Serial.print("Uploading: "); Serial.print((String)next_file);
#endif

                if( true == igh_mqtt_publish_data(sd_data_point, sd_data_point[1]) )
                {
                    if( true == igh_sd_log_remove_data_point(next_file) ) 
                    {
#ifdef IGH_DEBUG
                        Serial.println(" OK");
#endif
                    }
                    else
                    {
#ifdef IGH_DEBUG
                        Serial.println("EVENT: SD DEL ERROR");
#endif
                        igh_message_event(EVENT_SD_CARD_ERROR, false);
                    }
                }
                else
                {
#ifdef IGH_DEBUG
                    Serial.println("EVENT:  MQTT ERROR");
#endif
                    igh_message_event(EVENT_MQTT_ERROR, true);
                }   
            }
            else
            {
#ifdef IGH_DEBUG
                Serial.println("EVENT: SD OPEN ERROR");
#endif
                igh_message_event(EVENT_SD_CARD_ERROR, false);
            }
        }
        else
        {
#ifdef IGH_DEBUG
            // Serial.println("No new file to send");
#endif
        }
    }
}