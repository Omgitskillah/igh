/*******************************************************************************
 * @file igh_spear_dht22.cpp
 * @brief Application layer for dht22
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_dht22.h"
#include "igh_spear_log.h"
#include "igh_spear_payload.h"

#define DHTTYPE DHT22 

DHT dht(DHT_DATA, DHTTYPE);

uint16_t dht22_temperature;
uint16_t dht22_humidity;

// TODO: This should come from settings
unsigned long dht22_timer = 0;
unsigned long dht22_read_interval = 1000; // every second

void igh_spear_dht22_setup(void)
{
    dht.begin();
    dht22_timer = millis();

    payload_data_store[SENSOR_AIR_HUMIDITY].id = AIR_HUMIDITY;
    payload_data_store[SENSOR_AIR_HUMIDITY].new_data = false;
    payload_data_store[SENSOR_AIR_HUMIDITY].byte_count = 2;

    payload_data_store[SENSOR_AIR_TEMPERATURE].id = AIR_TEMPERATURE;
    payload_data_store[SENSOR_AIR_TEMPERATURE].new_data = false;
    payload_data_store[SENSOR_AIR_TEMPERATURE].byte_count = 2;
}

void igh_spear_dht22_test_service(void)
{
    delay(dht22_read_interval);
    if( dht.read(false) )
    {
#ifdef LOG_IGH_SPEAR_DHT22
        igh_spear_log("DHT22..................OK\n");
#endif      
    }
    else
    {
#ifdef LOG_IGH_SPEAR_DHT22
        igh_spear_log("DHT22..................ERROR\n");
#endif      
    }
}

void igh_spear_dht22_service(void)
{
    if( dht.read(false) )
    {
        dht22_temperature = ((uint16_t)(dht.data[2] & 0x7F)) << 8 | dht.data[3];
        dht22_humidity = ((uint16_t)dht.data[0]) << 8 | dht.data[1];

        payload_data_store[SENSOR_AIR_HUMIDITY].bytes[0] = dht22_humidity & 0xFF;
        payload_data_store[SENSOR_AIR_HUMIDITY].bytes[1] = (dht22_humidity >> 8);
        payload_data_store[SENSOR_AIR_HUMIDITY].new_data = true;

        payload_data_store[SENSOR_AIR_TEMPERATURE].bytes[0] = dht22_temperature & 0xFF;
        payload_data_store[SENSOR_AIR_TEMPERATURE].bytes[1] = (dht22_temperature >> 8);
        payload_data_store[SENSOR_AIR_TEMPERATURE].new_data = true;
#ifdef LOG_IGH_SPEAR_DHT22
        sprintf(debug_buff, "DHT22 --> temp: %d, hum: %d\n", dht22_temperature, dht22_humidity);
        igh_spear_log(debug_buff);
#endif
    }
}