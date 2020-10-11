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
#include "include/igh_settings.h"

/* MQTT variables */
char domain[] = "broker.hivemq.com";
byte ip_address[] = {138,197,6,61};
uint16_t port = 1883;
uint8_t device_name[] = "home_test_device";
uint8_t inbound_topic[] = "Synnefa_Green_MQTT_SETTINGS_LOCAL";
uint8_t outbound_topic[] = "Synnefa_Green_MQTT_DATA_LOCAT";


unsigned long upload_timer = 0;
unsigned long reconnect_interval = 0;

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
    if( true == mqtt_set_broker )
    {
        igh_current_system_settings.broker;
        uint8_t broker_len = 0;
        while( igh_current_system_settings.broker[broker_len] )
        {
            if( '\0' == igh_current_system_settings.broker[broker_len] ) break;
            broker_len++;
        }
        
        char new_broker[broker_len + 1];
        memcpy( new_broker, igh_current_system_settings.broker, sizeof(new_broker) );

        Serial.print("NEW MQTT BROKER SET: "); Serial.println(new_broker);
        
        client.setBroker(new_broker, igh_current_system_settings.broker_port);

        mqtt_set_broker = false;
    }

    if (client.isConnected())
    {
        client.loop();
    }
    else
    {
        /* try to connect */
        if( (millis() - reconnect_interval) > 10000 )
        {
            // try to reconnect only once every 10 seconds
            Serial.println("Reconnecting to Broker");
            client.connect( (const char *)device_name );
            reconnect_interval = millis();
        }
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


