/*******************************************************************************
 * @file igh_boron.cpp
 * @brief API to Boron Device Resoruces
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_boron.h"
#include "include/igh_settings.h"

// Local variable
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

unsigned long lastSync;
bool connected_to_cloud;
bool connected_to_network;
CellularSignal signal;
float signal_quality, signal_strength;
FuelGauge fuel;
float battery_voltage, battery_SOC;
unsigned long unix_time;

bool print_serial = true;

char hex_digits_LC[] = "0123456789abcdef";

/* Local function prototypes */
uint8_t get_int_from_str( uint8_t num );

/* Functions */
void igh_boron_sync_time(void)
{
    if((millis() - lastSync > ONE_DAY_MILLIS))
    {
        Particle.syncTime();
        lastSync = millis(); 
    }
}

unsigned long igh_boron_unix_time(void)
{
    return Time.now();
}

float igh_boron_SoC(void)
{
    return fuel.getSoC();
}

float igh_boron_voltage(void)
{
    return fuel.getVCell();
}

void igh_boron_get_celular_data(void)
{
    signal = Cellular.RSSI();
}

float igh_boron_sq(void)
{
    return signal.getQuality();
}

float igh_boron_ss(void)
{
    return signal.getQuality();
}

uint8_t igh_boron_connecetd_to_network(void)
{
    return Cellular.ready();
}

uint8_t igh_boron_connected_to_cloud(void)
{
    return Particle.connected();;
}

void igh_boron_setup(void)
{
    lastSync = millis();
    // get serial number 
    uint8_t deviceID[24];
    memcpy( deviceID, System.deviceID(), sizeof(deviceID) );

    uint8_t k = 0; uint8_t j = 0;
    while( k < sizeof(deviceID) )
    {
        boron_serial_number[j] = get_int_from_str(deviceID[k]) << 4;
        k++;
        boron_serial_number[j] |= get_int_from_str(deviceID[k]);
        k++; j++;
    }
}

uint8_t get_int_from_str( uint8_t num )
{
    unsigned long location = (unsigned long)strchr(hex_digits_LC, num);
    unsigned long origin = (unsigned long)hex_digits_LC;
    return uint8_t(location - origin);
}

void igh_boron_service(void)
{
    igh_boron_sync_time();
#ifdef TEST_MODE
    igh_boron_get_celular_data();
#endif
}

void igh_boron_test_device(void)
{
    Serial.print("\nDevice ID:       "); 
    for( uint8_t i = 0; i < sizeof(boron_serial_number); i++ )
    {
        if( boron_serial_number[i] <= 0x0F ) Serial.print("0");
        Serial.print( boron_serial_number[i], HEX );
    }
    Serial.print("\n");
    Serial.print("Unix Time:        "); Serial.println(igh_boron_unix_time());
    Serial.print("cloud status:     "); Serial.println(igh_boron_connected_to_cloud());
    Serial.print("network status:   "); Serial.println(igh_boron_connecetd_to_network());
    Serial.print("signal strength:  "); Serial.println(igh_boron_ss());
    Serial.print("signal quality:   "); Serial.println(igh_boron_sq());
    Serial.print("Battery Voltage:  "); Serial.println(igh_boron_voltage());
    Serial.print("Battery SOC:      "); Serial.println(igh_boron_SoC());
}