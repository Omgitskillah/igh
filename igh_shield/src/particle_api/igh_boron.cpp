/*******************************************************************************
 * @file igh_boron.cpp
 * @brief API to Boron Device Resoruces
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_boron.h"
#include "include/igh_settings.h"
#include "dct.h"
#include "./simcard.inc"

// Local variable
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
#define KEEP_ALIVE_SECONDS (300) //keep alive connection to the cloud every 5 mins, may change based on sim carrier

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

SYSTEM_MODE(SEMI_AUTOMATIC);

void igh_boron_sim_select( void );

/* Functions */
void igh_boron_sync_time(void)
{
    if((millis() - lastSync > ONE_DAY_MILLIS))
    {
        Particle.syncTime();
        lastSync = millis(); 
    }
}

uint32_t igh_boron_unix_time(void)
{
    return (uint32_t)Time.now();
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
    igh_boron_sim_select(); 

    lastSync = millis();
    // get serial number 
    memcpy( deviceID_string, System.deviceID(), sizeof(deviceID_string) );

    uint8_t k = 0; uint8_t j = 0;
    while( k < sizeof(deviceID_string) )
    {
        boron_serial_number[j] = get_int_from_str(deviceID_string[k]) << 4;
        k++;
        boron_serial_number[j] |= get_int_from_str(deviceID_string[k]);
        k++; j++;
    }
}

uint8_t get_int_from_str( uint8_t num )
{
    unsigned long location = (unsigned long)strchr(hex_digits_LC, num);
    unsigned long origin = (unsigned long)hex_digits_LC;
    return uint8_t(location - origin);
}

void igh_boron_sim_select( void )
{
    if( CURRENT_SIMTYPE == INTERNAL_SIM )
    {
        Cellular.setActiveSim(INTERNAL_SIM);
	    Cellular.clearCredentials();
#ifdef IGH_DEBUG
        Serial.print("USING INTERNAL SIM");
#endif
    }
    else if( CURRENT_SIMTYPE == EXTERNAL_SIM )
    {
        Cellular.setActiveSim(EXTERNAL_SIM);
	    Cellular.setCredentials(APN_CREDENTIALS);
        const uint8_t val = 0x01;
        dct_write_app_data(&val, DCT_SETUP_DONE_OFFSET, 1);
        Particle.keepAlive(KEEP_ALIVE_SECONDS); 
    }
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
#ifdef IGH_DEBUG
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
#endif
}