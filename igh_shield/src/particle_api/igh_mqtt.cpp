/*******************************************************************************
 * @file igh_mqtt.cpp
 * @brief Manages the MQTT protocol for the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "MQTT.h"
#include "igh_mqtt.h"
#include "igh_boron.h"

/* MQTT variables */
char domain[] = "broker.hivemq.com";
byte ip_address[] = {138,197,6,61};
uint16_t port = 1883;
uint8_t device_name[] = "home_test_device";
uint8_t inbound_topic[] = "Synnefa_Green_MQTT_SETTINGS_LOCAL";
uint8_t outbound_topic[] = "Synnefa_Green_MQTT_DATA_LOCAT";


unsigned long upload_timer = 0;

/* mqtt functions */
void mqtt_callback(char* topic, byte* payload, unsigned int length);
MQTT client(domain, port, mqtt_callback);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
}

void igh_mqtt_setup( void )
{
    // connect to the server
    client.connect( (const char *)device_name );
    
}

void igh_mqtt_service( void )
{
    if (client.isConnected())
    {
        client.loop();
    }
    else
    {
        /* try to connect */
        Serial.println("Reconnecting");
        client.connect( (const char *)device_name );
    }
}

bool igh_mqtt_publish_data( uint8_t *payload, unsigned int len )
{
    bool ret = false;
    if (client.isConnected()) {
        client.publish( (const char *)outbound_topic, 
                        (const uint8_t*) payload, len);
        client.subscribe( (const char *)inbound_topic );
        ret = true;
    }
    else
    {
        Serial.println("PUBLISH FAILED");
    }
    return ret;
}


