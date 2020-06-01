/*******************************************************************************
 * @file igh_mqtt.cpp
 * @brief Manages the MQTT protocol for the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "MQTT.h"
#include "igh_mqtt.h"
#include "include/igh_settings.h"

// These must be stored in system settings in eeprom
const char id[]= "test_device"; // * this may be built from device id
char broker[] = "broker.hivemq.com"; // THis should be read straght from storage/ settings struct
uint16_t broker_port = 1883; // SHould be read straight from settings struct

// callback function whenever there is a new message to process
// create downstream message buffer to store multiple messages to process when more than one payloads are present?
void igh_mqtt_callback(char* topic, byte* payload, unsigned int length);
// char p[length + 1];
// memcpy(p, payload, length);

// Setup the active mqtt broker from eeprom settings
void igh_mqtt_setup_broker(void); //client.setBroker(domain1, 1883)

// connect to broker, return true on success
uint8_t igh_mqtt_connect(void); // client.connect("id");

// mqtt publish data, return true on success, unsubscribe, send, then subscribe?
uint8_t igh_mqtt_publish_data(uint8_t * paylaod);

// mqtt subscribe to topic, return true on success
uint8_t igh_mqtt_subscribe(void);

// setup client, use default broke and ort during setup
MQTT client(broker, broker_port, callback);



