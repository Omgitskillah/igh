/*******************************************************************************
 * @file igh_mqtt.cpp
 * @brief Manages the MQTT protocol for the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "MQTT.h"
#include "igh_mqtt.h"
#include "include/igh_message.h"
#include "include/igh_settings.h"
#include "include/igh_default_settings.h"

// These must be stored in system settings in eeprom
const char id[]= "test_device"; // * this may be built from device id
char broker[] = "broker.hivemq.com"; // THis should be read straght from storage/ settings struct
uint16_t broker_port = 1883; // SHould be read straight from settings struct

// callback function whenever there is a new message to process
// create downstream message buffer to store multiple messages to process when more than one payloads are present?
void igh_mqtt_callback(char* topic, byte* payload, unsigned int length);
// char p[length + 1];
// memcpy(p, payload, length);


// connect to broker, return true on success
uint8_t igh_mqtt_connect(void); // client.connect("id");

// mqtt publish data, return true on success, unsubscribe, send, then subscribe?
uint8_t igh_mqtt_publish_data(uint8_t * paylaod);





















// setup client, use default broke and ort during setup
MQTT client(DEFAULT_MQTT_BROKER, (uint16_t)DEFAULT_MQTT_BROKER_PORT, igh_mqtt_callback);

/*******************************************************************************
 * igh_mqtt_setup_broker
 *******************************************************************************/
/**
 * \brief setup igh broker from the prevailing system settings
 * \param void
 * \return void
 */
void igh_mqtt_setup_broker(void)
{
    client.setBroker(
        igh_current_system_settings.broker, 
        igh_current_system_settings.broker_port); 
        // can only be called after stored settings are updated from eeprom
}

/*******************************************************************************
 * igh_mqtt_subscribe
 *******************************************************************************/
/**
 * \brief subscribe to the download topic using the serial number and the download byte in the string
 * \param void
 * \return true or false
 */
uint8_t igh_mqtt_subscribe(void)
{
    // multiply by two, should still be within a uint8
    uint8_t sub_topic_len = ((1 + sizeof(igh_current_system_settings.serial_number)) << 1) + 1; // + 1 for the null terminator 
    uint8_t sub_topic[sub_topic_len];
    
    sprintf(&sub_topic[0], "%02X", IGH_DOWNLOAD);

    // start from 2 to avoid the first two character
    for( uint8_t i = 2; i < sizeof(igh_current_system_settings.serial_number); i++ )
    {
        sprintf(&sub_topic[2*i], "%02X", igh_current_system_settings.serial_number[i]);
    }

    return client.subscribe((const char*)sub_topic);
}
