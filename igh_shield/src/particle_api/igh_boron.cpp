/*******************************************************************************
 * @file igh_boron.cpp
 * @brief API to Boron Device Resoruces
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "igh_boron.h"

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
}

void igh_boron_service(void)
{
    igh_boron_sync_time();
    igh_boron_get_celular_data();
}

void igh_boron_test_device(void)
{
    Serial.print("\nUnix Time:      "); Serial.println(igh_boron_unix_time());
    Serial.print("cloud status:     "); Serial.println(igh_boron_connected_to_cloud());
    Serial.print("network status:   "); Serial.println(igh_boron_connecetd_to_network());
    Serial.print("signal strength:  "); Serial.println(igh_boron_ss());
    Serial.print("signal quality:   "); Serial.println(igh_boron_sq());
    Serial.print("Battery Voltage:  "); Serial.println(igh_boron_voltage());
    Serial.print("Battery SOC:      "); Serial.println(igh_boron_SoC());
}
