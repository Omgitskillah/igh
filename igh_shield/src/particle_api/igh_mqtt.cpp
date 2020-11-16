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
#include "include/igh_default_settings.h"
#include "include/igh_message.h"

/* MQTT variables */
String device_name;
String inbound_topic;
String outbound_topic;

unsigned long upload_timer = 0;
unsigned long reconnect_interval = 0;

bool mqtt_subscribed = false;
uint8_t mqtt_connected = 0;

/* mqtt functions */
void mqtt_callback(char* topic, byte* payload, unsigned int length);
MQTT client(DEFAULT_MQTT_BROKER, DEFAULT_MQTT_BROKER_PORT, mqtt_callback);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    igh_msg_type msg_type = UNKNOWN_MSG;
    uint8_t p[length];
    memcpy(p, payload, length);

    Serial.print("RECEIVED MSG: ");
    for( uint8_t i = 0; i < length; i++ )
    {
        if( p[i] <= 0x0F ) Serial.print("0");
        Serial.print(p[i], HEX);
    }

    msg_type = (igh_msg_type)igh_message_process_incoming_msg( p );

    Serial.print("\nMESSAGE TYPE: ");
    Serial.println((int)msg_type);
}

void igh_mqtt_setup( void )
{
    // set mqtt device name
    device_name = System.deviceID();
    inbound_topic = "44" + System.deviceID();
    outbound_topic = "55" + System.deviceID();
    client.connect( (const char *)device_name );

}

void igh_mqtt_service( void )
{
    if( true == mqtt_set_broker )
    {
        uint8_t broker_len = 0;

        while( igh_current_system_settings.broker[broker_len] )
        {
            if( '\0' == igh_current_system_settings.broker[broker_len] ) break;
            broker_len++;
        }
        
        char new_broker[broker_len + 1];
        
        memcpy( new_broker, igh_current_system_settings.broker, sizeof(new_broker) );

        Serial.print("BROKER: "); Serial.print(new_broker);
        Serial.print(" PORT: "); Serial.println(igh_current_system_settings.broker_port);
        
        client.setBroker(new_broker, igh_current_system_settings.broker_port);

        mqtt_set_broker = false;
    }

    if (client.isConnected())
    {
        client.loop();
        mqtt_connected = 1;

        if( false == mqtt_subscribed )
        {
            client.subscribe( (const char *)inbound_topic );
            mqtt_subscribed = true;
        }
    }
    else
    {
        /* try to connect */
        mqtt_connected = 0;

        if( (millis() - reconnect_interval) > 10000 )
        {
            // try to reconnect only once every 10 seconds
            uint8_t broker_uname_len = 0;
            uint8_t broker_password_len = 0;

            while( igh_current_system_settings.mqtt_username[broker_uname_len] )
            {
                if( '\0' == igh_current_system_settings.mqtt_username[broker_uname_len] ) break;
                broker_uname_len++;
            }

            while( igh_current_system_settings.mqtt_password[broker_password_len] )
            {
                if( '\0' == igh_current_system_settings.mqtt_password[broker_password_len] ) break;
                broker_password_len++;
            }

            char new_uname[broker_uname_len + 1];
            char new_password[broker_password_len + 1];
            
            memcpy( new_uname, igh_current_system_settings.mqtt_username, sizeof(new_uname) );
            memcpy( new_password, igh_current_system_settings.mqtt_password, sizeof(new_password) );
            
            // Serial.print("CLIENT NAME: "); Serial.print((const char *)device_name);
            // Serial.print(" USERNAME: "); Serial.print(new_uname);
            // Serial.print(" PASSWORD: "); Serial.println(new_password);
            
            client.connect( (const char *)device_name, (const char *)new_uname, (const char *)new_password );
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


