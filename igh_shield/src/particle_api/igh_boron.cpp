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
#define ON        1
#define OFF       0
#define BORON_LED               D7
#define IGH_IRRIGATION_BUTTON   D5

unsigned long lastSync;
bool connected_to_cloud;
bool connected_to_network;
CellularSignal signal;
float signal_quality, signal_strength;
FuelGauge fuel;
float battery_voltage, battery_SOC;
unsigned long unix_time;

uint8_t igh_button_sec_counter;
Timer igh_button_timer(1000, igh_boron_button_press_duration);

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
    pinMode(BORON_LED, OUTPUT);
    pinMode(IGH_IRRIGATION_BUTTON, INPUT_PULLUP);
    igh_boron_toggle_boron_led(OFF);
    igh_button_timer.start();
    igh_button_sec_counter = 0;
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

void igh_boron_toggle_boron_led(uint8_t _state)
{
    digitalWrite(BORON_LED, _state);
}

uint8_t igh_boron_read_button(void)
{
    return digitalRead(IGH_IRRIGATION_BUTTON);
}

void igh_boron_button_press_duration(void)
{
    if(!igh_boron_read_button())
    {
        igh_button_sec_counter++;
        // Serial.print("button timer: "); Serial.println(igh_button_sec_counter);
    }
    else
    {
        igh_button_sec_counter = 0;
    } 
}
