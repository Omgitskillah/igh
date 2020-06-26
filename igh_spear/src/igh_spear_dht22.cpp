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
/* uncomment to enable debug */
#define LOG_IGH_SPEAR_DHT22

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
    if( (millis() - dht22_timer) > dht22_read_interval )
    {
        if( dht.read(false) )
        {
            dht22_timer = millis();
            dht22_temperature = ((uint16_t)(dht.data[2] & 0x7F)) << 8 | dht.data[3];
            dht22_humidity = ((uint16_t)dht.data[0]) << 8 | dht.data[1];
#ifdef LOG_IGH_SPEAR_DHT22
            sprintf(debug_buff, "DHT22 --> temp: %d, hum: %d\n", dht22_temperature, dht22_humidity);
            igh_spear_log(debug_buff);
#endif
        }
    }
}