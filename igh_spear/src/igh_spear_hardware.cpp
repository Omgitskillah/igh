/*******************************************************************************
 * @file igh_spear_hardware.c
 * @brief API for native Spear features
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_hardware.h"
#include "igh_spear_log.h"
/* uncomment to enable debug */
// #define LOG_IGH_SPEAR_HARDWARE

/* Onboard led */
#define HEART_PERIOD    1000 // ms
#define HEART_ON_TIME   50 //ms
#define HEART_OFF_TIME  (HEART_PERIOD - HEART_ON_TIME)
unsigned long heartbeat_timer       = 0; // keep track of heartbeat
unsigned long heartbeat_state_time  = 0;

/* battery */
#define BATTERY_REFRESH_FREQUENCY 1000 // check battery voltage every 5 seconds
uint16_t battery_voltage = 0;
unsigned long vbatt_refresh_timer  = 0;


void igh_spear_hardware_setup(void)
{
    pinMode(IGH_SPEAR_LED, OUTPUT);
    
    analogReadResolution(12); // read the full resolution of the ADC on the SAMD M0 MCU

    pinMode(VBAT_SENSE, INPUT);
    heartbeat_state_time = HEART_ON_TIME;
}

uint16_t igh_spear_get_raw_battery_voltage(void)
{
    return (uint16_t)analogRead(VBAT_SENSE);
}

void igh_spear_hardware_heartbeat(void)
{
    if( (millis() - heartbeat_timer) > heartbeat_state_time)
    {
        digitalWrite(IGH_SPEAR_LED, !digitalRead(IGH_SPEAR_LED));
        heartbeat_state_time = ( digitalRead(IGH_SPEAR_LED) ? HEART_ON_TIME : HEART_OFF_TIME );
        heartbeat_timer = millis();
    }
}

void igh_spear_hardware_battery_service(void)
{
    if( (millis() - vbatt_refresh_timer) > BATTERY_REFRESH_FREQUENCY )
    {
        battery_voltage = igh_spear_get_raw_battery_voltage();
#ifdef LOG_IGH_SPEAR_HARDWARE
        int buff = (battery_voltage * 1.6117); // this has a heavy toll on RAM
        uint16_t voltage = (uint16_t)buff; // 1.61 is the scaling factor 
        sprintf(debug_buff, "Raw adc: %d, Batt Voltage: %dmV\n", battery_voltage, voltage );
        igh_spear_log(debug_buff);
#endif
        vbatt_refresh_timer = millis();
    }
}
