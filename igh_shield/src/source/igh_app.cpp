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

#define MAX_HUMIDITY (3300)

unsigned long log_service_timer = 0;
uint8_t device_restart = 1;
extern uint8_t igh_msg_buffer[MESSAGE_SIZE]; 
valve_position previous_valve_position = VALVE_CLOSE;
extern bool button_irrigate;
extern bool automatic_irrigation_mode;
extern uint8_t igh_button_sec_counter_to_send;
bool previous_button_irrigate = false;
bool previous_automatic_irrigation_mode= false;


uint8_t igh_app_add_payload( uint8_t *_buffer, uint8_t start, uint8_t * _payload, uint8_t _payload_len );
uint8_t igh_app_add_message_header( uint8_t *_buffer, uint8_t start, igh_msg_type msg_type, igh_msg_dir dir );
void igh_app_send_device_restart( void );
void igh_app_receive_and_stage_sensor_data( void );
uint8_t igh_app_get_serial_hex_data( uint8_t * buffer, uint8_t len );
void igh_app_get_new_settings( void );
void igh_app_commit_new_settings( void );
void igh_app_log_service( void );
void igh_app_get_temperature_and_humidity( uint8_t * incoming_data );
void igh_app_send_button_and_valve_events( void );
uint16_t igh_app_calculate_humidity( uint16_t temperature, uint16_t humidity );
void igh_app_print_valid_settings( void );

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

    // control the valve
    igh_hardware_service_valve_state();

    // send button and valve events 
    igh_app_send_button_and_valve_events();

}

void igh_app_send_button_and_valve_events( void )
{
    if( previous_valve_position != current_valve_position )
    {
        // send a packet with the current valve state
        Serial.print("\nVALVE: "); 
        if( current_valve_position == VALVE_OPEN )
        {
            Serial.print("OPEN ");
        }
        else
        {
            Serial.println("CLOSED ");
        }
        uint32_t current_time = igh_boron_unix_time();

        uint8_t i = 0; // keep track of pkt data
        uint8_t valve_state_msg[3];

        memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );

        valve_state_msg[0] = VALVE_POSITION;
        valve_state_msg[1] = SIZE_OF_VALVE_POSITION;
        valve_state_msg[2] = current_valve_position;

        // Add frame start
        igh_msg_buffer[i++] = FRAME_START;
        i++; // leave room for pkg length

        i = igh_app_add_message_header( igh_msg_buffer, i, ERROR_MSG, IGH_UPLOAD );
        i = igh_app_add_payload( igh_msg_buffer, i, valve_state_msg, sizeof(valve_state_msg) );

        // Add Frame End 
        igh_msg_buffer[i++] = FRAME_END;

        igh_msg_buffer[1] = i; // add length

        igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );

        previous_valve_position = current_valve_position;
    }

    if( previous_button_irrigate != button_irrigate ||
        previous_automatic_irrigation_mode != automatic_irrigation_mode )
    {
        // send packet with button press event
        uint32_t current_time = igh_boron_unix_time();

        uint8_t i = 0; // keep track of pkt data
        uint8_t button_press_msg[3];

        memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );

        button_press_msg[0] = BUTTON_PRESS;
        button_press_msg[1] = SIZE_OF_BUTTON_PRESS;
        button_press_msg[2] = igh_button_sec_counter_to_send;

        // Add frame start
        igh_msg_buffer[i++] = FRAME_START;
        i++; // leave room for pkg length

        i = igh_app_add_message_header( igh_msg_buffer, i, ERROR_MSG, IGH_UPLOAD );
        i = igh_app_add_payload( igh_msg_buffer, i, button_press_msg, sizeof(button_press_msg) );

        // Add Frame End 
        igh_msg_buffer[i++] = FRAME_END;

        igh_msg_buffer[1] = i; // add length

        igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );

        previous_button_irrigate = button_irrigate;
        previous_automatic_irrigation_mode = automatic_irrigation_mode;
    }
}

void igh_app_receive_and_stage_sensor_data( void )
{
    uint8_t rx_buffer[RF69_MAX_DATA_LEN];
    uint8_t data_rx_len = 0;

    data_rx_len = igh_rfm69_receive_raw_bytes( rx_buffer, sizeof(rx_buffer) );

    if( 0 != data_rx_len )
    {
        igh_boron_toggle_boron_led(ON);

        // read data needed by the shield
        igh_app_get_temperature_and_humidity(rx_buffer);

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

        // rpint water dispensed data along with incoming sensor data
        Serial.print("\nWATER DISPENSED: "); Serial.print(total_water_dispensed_Liters); Serial.println("L");

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
        else if( (igh_msg_buffer[0] == '?') &&
                 (igh_msg_buffer[1] == '?') &&
                 (igh_msg_buffer[2] == '?') )
        {
            igh_app_print_valid_settings();
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
            reset_irrigation_params();
            Serial.println("IRRIGATION RESET SUCCESSFUL");
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
            new_settings_available = 1;
            mqtt_set_broker = 1;
            initialize_rfm69 = 1;
        }
    }
}

void igh_app_commit_new_settings( void )
{
    if( 1 == new_settings_available )
    {
        igh_app_print_valid_settings();

        if ( true == igh_eeprom_save_system_settings( &igh_current_system_settings) )
        {
            Serial.println("System Settings Saved successfully");
        }

        if ( true == igh_eeprom_save_threshold_settings( &igh_current_threshold_settings) )
        {
            Serial.println("Threshold Settings Saved successfully");
        }

        new_settings_available = 0;
    }
}

void igh_app_print_valid_settings( void )
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
    Serial.print("MQTT USERNAME: "); Serial.println((char *)igh_current_system_settings.mqtt_username);
    Serial.print("MQTT PASSWORD: "); Serial.println((char *)igh_current_system_settings.mqtt_password);
    Serial.print("TIMEZONE: "); Serial.println(igh_current_system_settings.timezone);
    Serial.print("IRRIGATION HOUR: "); Serial.println(igh_current_system_settings.irrigation_hr);
    Serial.print("WATER DISPENSER PERIOD: "); Serial.println(igh_current_system_settings.water_dispenser_period);
    Serial.print("AMOUNT BUTTON CAN DISPENSE: "); Serial.print(igh_current_system_settings.water_amount_by_button_press); Serial.println("L");
    Serial.print("SYSTEM SETTINGS CHECKSUM: "); Serial.println(igh_current_system_settings.checksum);

    Serial.print("\n\nSOIL HUMIDITY LOW THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_low);
    Serial.print("SOIL HUMIDITY HIGH THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_high);
    Serial.print("MIN WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_low); Serial.println("L");
    Serial.print("MAX WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_high); Serial.println("L");
    Serial.print("THRESHOLDS SETTINGS CHECKSUM: "); Serial.println(igh_current_threshold_settings.checksum);
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

void igh_app_get_temperature_and_humidity( uint8_t * incoming_data )
{
    uint8_t data_len = incoming_data[1];
    uint8_t current_tuple_id = 0;
    uint8_t current_tuple_length = 0;
    uint8_t current_data_index = 0;
    uint8_t byte_tracker = 2;

    uint16_t new_temperature = 0xFFFF;
    uint16_t new_humidity = 0xFFFF;
    bool valid_temerature = false;
    bool valid_humidity = false;

    while(byte_tracker < data_len)
    {
        // extract tuples
        current_tuple_id = incoming_data[byte_tracker];
        // extract the length
        current_tuple_length = incoming_data[byte_tracker + 1];
        // extract the tuple data based on tuple id
        current_data_index = byte_tracker + 2;

        switch( current_tuple_id )
        {
            case SOIL_TEMPERATURE:
                if( SIZE_OF_SOIL_TEMPERATURE == current_tuple_length )
                {
                    uint8_t new_temperature_reading[SIZE_OF_SOIL_TEMPERATURE]; 
                    memcpy(new_temperature_reading, &incoming_data[current_data_index], SIZE_OF_SOIL_TEMPERATURE);
                    new_temperature = GET16_LI(new_temperature_reading);
                    valid_temerature = true;
                }
                else
                {
                    /* Do nothing */
                }
                break;
                
            case SOIL_HUMIDITY:
                if( SIZE_OF_SOIL_HUMIDITY == current_tuple_length )
                {
                    uint8_t new_humidity_reading[SIZE_OF_SOIL_HUMIDITY]; 
                    memcpy(new_humidity_reading, &incoming_data[current_data_index], SIZE_OF_SOIL_HUMIDITY);
                    new_humidity = GET16_LI(new_humidity_reading);
                    valid_humidity = true;
                }
                else
                {
                    /* Do nothing */
                }
                break;

            default:
                /* Ignore other data */
                break;
        }
        // move index to next tuple id
        byte_tracker += current_tuple_length + TUPLE_HEADER_LEN;
    }


    if( true == valid_humidity &&
        true == valid_temerature )
    {
        soil_humidity = igh_app_calculate_humidity(new_temperature, new_humidity);
        if( soil_humidity <= MAX_HUMIDITY )
        {
            // only use valid sensor data
            refreshed_soil_data = VALID_SOIL_DATA;
            Serial.println("VALID HUMIDITY DATA");
        }
        else
        {
            refreshed_soil_data = VALID_SOIL_DATA;
            Serial.println("SATURATED HUMIDITY DATA");
        }
    }
    else
    {
        refreshed_soil_data = INVALID_SOIL_DATA;
        Serial.println("INVALID HUMIDITY DATA");
    }   
}

uint16_t igh_app_calculate_humidity( uint16_t new_temperature, uint16_t new_humidity )
{
/* Human readable
    double linearHumidity = SOIL_HUMIDITY_MULTIPLIER_C1 + SOIL_HUMIDITY_MULTIPLIER_C2 * new_humidity 
                          + SOIL_HUMIDITY_MULTIPLIER_C3 * new_humidity * new_humidity;

    float temperature = SOIL_TEMPERATURE_MULTIPLIER_D1 
                      + SOIL_TEMPERATURE_MULTIPLIER_D2 * new_temperature;

    float correctedHumidity = (temperature - ROOM_TEMPERATURE) * 
                              (SOIL_HUMIDITY_MULTIPLIER_T1 + SOIL_HUMIDITY_MULTIPLIER_T2 * new_humidity) 
                              + linearHumidity;

    Serial.print("TEMPERATURE: "); Serial.print(temperature);
    Serial.print("C HUMIDITY: "); Serial.print(correctedHumidity);
    Serial.println("%");

    return (uint16_t)correctedHumidity;
*/

/* RAW */
    Serial.print("RAW TEMPERATURE: "); Serial.print(new_temperature);
    Serial.print(" RAW HUMIDITY: "); Serial.println(new_humidity);

    // offload the decimal places
    return new_humidity;
}


