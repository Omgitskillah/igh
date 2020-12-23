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

uint8_t fw_ver[3] = {0,0,22};

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
void update_fw_version( void );
void igh_app_send_event_pkt( igh_event_id_e event );

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

    // update fw version
    update_fw_version();
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

void update_fw_version( void )
{
    uint8_t read_fw_ver[3];
    EEPROM.get( SYSTEM_FW_VERSION, read_fw_ver );
    if( read_fw_ver[0] != fw_ver[0] ||
        read_fw_ver[1] != fw_ver[1] ||
        read_fw_ver[2] != fw_ver[2] )
    {
        EEPROM.put( SYSTEM_FW_VERSION, fw_ver);
    }
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
            igh_app_send_event_pkt(EVENT_VAVLE_OPENED);
        }
        else
        {
            Serial.println("CLOSED ");
            igh_app_send_event_pkt(EVENT_VAVLE_CLOSED);
        }

        previous_valve_position = current_valve_position;
    }

    if( previous_button_irrigate != button_irrigate ||
        previous_automatic_irrigation_mode != automatic_irrigation_mode )
    {
        if( igh_button_sec_counter_to_send == TWO_SECONDS )
        {
            igh_app_send_event_pkt(EVENT_TWO_SECONDS_BUTTON_PRESS);
        }
        else if( igh_button_sec_counter_to_send == FIVE_SECONDS )
        {
            igh_app_send_event_pkt(EVENT_FIVE_SECONDS_BUTTON_PRESS);
        }

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

        i = igh_app_add_message_header( igh_msg_buffer, i, SENSOR_DATA, IGH_UPLOAD );
        i = igh_app_add_payload( igh_msg_buffer, i, rx_buffer, data_rx_len );

        // Add Frame End 
        igh_msg_buffer[i++] = FRAME_END;

        igh_msg_buffer[1] = i; // add length

        // publish the data or store it if the publish fails
        uint32_t current_time = igh_boron_unix_time();
        
        // Log data only if the time is synced
        if( JAN_01_2020 < current_time )
        {
            if( true == mqtt_connected )
            {
                if( false == igh_mqtt_publish_data(igh_msg_buffer, i) )
                {
                    igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );
                }
                else
                {
                    Serial.print("Uploading: "); Serial.print(current_time, HEX); Serial.println(".LOG success");
                }
            }
            else
            {
                igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );
            }
        }


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
        Serial.println("*************** SYSTEM RESET ***************");
        igh_app_send_event_pkt(EVENT_DEVICE_RESTART);
        device_restart = false;
    }
}

void igh_app_send_event_pkt( igh_event_id_e event )
{
    uint32_t current_time = igh_boron_unix_time();

    uint8_t i = 0; // keep track of pkt data
    uint8_t event_msg[3];

    // first clear the buffer
    memset( igh_msg_buffer, 0, sizeof(igh_msg_buffer) );

    // Add Retart Message
    event_msg[0] = EVENT;
    event_msg[1] = SIZE_OF_ERROR;
    event_msg[2] = (uint8_t)event;

    // Add frame start
    igh_msg_buffer[i++] = FRAME_START;
    i++; // leave room for pkg length

    i = igh_app_add_message_header( igh_msg_buffer, i, ERROR_MSG, IGH_UPLOAD );
    i = igh_app_add_payload( igh_msg_buffer, i, event_msg, sizeof(event_msg) );

    // Add Frame End 
    igh_msg_buffer[i++] = FRAME_END;

    igh_msg_buffer[1] = i; // add length

    // save restart message
    if( true == mqtt_connected )
    {
        if( false == igh_mqtt_publish_data(igh_msg_buffer, i) &&
          event != EVENT_SD_CARD_ERROR )
        {
            igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );
        }
        else
        {
            Serial.print("Uploading: "); Serial.print(current_time, HEX); Serial.println(".LOG success");
        }
    }
    else
    {   
        if( event != EVENT_SD_CARD_ERROR )
        {
            igh_sd_log_save_data_point( (unsigned long)current_time, igh_msg_buffer, i );
        }
    }
}

uint8_t igh_app_add_message_header( uint8_t *_buffer, uint8_t start, igh_msg_type msg_type, igh_msg_dir dir )
{
    uint8_t i = start;

    // Add Message type
    _buffer[i++] = msg_type;

    // Add direction
    _buffer[i++] = dir;

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
    float total_water_dispensed_snapshot = total_water_dispensed_Liters;
    memcpy( &curr_water_L, &total_water_dispensed_snapshot, sizeof curr_water_L );
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
    uint8_t current_fw_ver[3];
    EEPROM.get( SYSTEM_FW_VERSION, current_fw_ver);
    Serial.print("FW VERSION: v"); Serial.print(current_fw_ver[0]); Serial.print("."); Serial.print(current_fw_ver[1]); Serial.print("."); Serial.println(current_fw_ver[2]);
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
    Serial.print("WATER DISPENSER PERIOD: "); Serial.println(igh_current_system_settings.water_dispenser_period);
    Serial.print("AMOUNT BUTTON CAN DISPENSE: "); Serial.print(igh_current_system_settings.water_amount_by_button_press); Serial.println("L");
    Serial.print("SYSTEM SETTINGS CHECKSUM: "); Serial.println(igh_current_system_settings.checksum);

    Serial.print("\n\nSOIL HUMIDITY LOW THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_low);
    Serial.print("SOIL HUMIDITY HIGH THRESHOLD: "); Serial.println(igh_current_threshold_settings.soil_humidity_high);
    Serial.print("MIN WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_low); Serial.println("L");
    Serial.print("MAX WATER TO DISPENSE: "); Serial.print(igh_current_threshold_settings.water_dispensed_period_high); Serial.println("L");
    Serial.print("THRESHOLDS SETTINGS CHECKSUM: "); Serial.println(igh_current_threshold_settings.checksum);

    irrigation_params_str irrigation_parameters;
    EEPROM.get(SYSTEM_IRRIGATION_FLAGS, irrigation_parameters);
    Serial.print("IRRIGATION STATE FLAG: "); Serial.println(irrigation_parameters.irrigation_state, HEX);
    Serial.print("WATER DISPENSED STATE FLAG: "); Serial.println(irrigation_parameters.min_amount_of_water_dispens_status, HEX);
    Serial.print("TOTAL WATER DISPENSED: "); Serial.print(total_water_dispensed_Liters); Serial.println("L");
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
                            igh_app_send_event_pkt(EVENT_SD_CARD_ERROR);
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
            // Serial.println("VALID HUMIDITY DATA");
        }
        else
        {
            refreshed_soil_data = VALID_SOIL_DATA;
            // Serial.println("SATURATED HUMIDITY DATA");
        }
    }
    else
    {
        refreshed_soil_data = INVALID_SOIL_DATA;
        // Serial.println("INVALID HUMIDITY DATA");
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
    Serial.print("\nRAW TEMPERATURE: "); Serial.print(new_temperature);
    Serial.print(" RAW HUMIDITY: "); Serial.println(new_humidity);

    // offload the decimal places
    return new_humidity;
}


