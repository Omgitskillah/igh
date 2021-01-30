/*******************************************************************************
 * @file igh_spear_mhz19.cpp
 * @brief Application layer carbon dioxide sensor mhz19
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_mhz19.h"
#include "igh_spear_log.h"
#include "igh_spear_payload.h"
#include "igh_spear_settings.h"


// hardware baud rate, should not change
#define MHZ19_BAUD      9600 

MHZ19 myMHZ19;

uint16_t mhz19_co2;

unsigned long mhz19_timer = 0;
// TODO: Should come from settings
unsigned long mhz19_read_interval = 2000; // minimum 2 seconds for accuracy

void igh_spear_mhz19_setup(void)
{
    if( (SERIAL_SENSOR_CO2 != active_system_setting.serial_sensor_type) || (true == spear_serial_sensor_type_updated)  )
    { return; } // do nothing if this sensor is not selected

    payload_data_store[SENSOR_CARBON_DIOXIDE].id = CARBON_DIOXIDE;
    payload_data_store[SENSOR_CARBON_DIOXIDE].new_data = false;
    payload_data_store[SENSOR_CARBON_DIOXIDE].byte_count = 4;

    Serial1.begin(MHZ19_BAUD); 
    myMHZ19.begin(Serial1); 

    mhz19_timer = millis();
}

void igh_spear_mhz19_test_service(void)
{
    if( (SERIAL_SENSOR_CO2 != active_system_setting.serial_sensor_type) || (true == spear_serial_sensor_type_updated) )
    { return; } // do nothing if this sensor is not selected

    delay(mhz19_read_interval);

    mhz19_co2 = 0;
    mhz19_co2 = (uint16_t)myMHZ19.getCO2Raw();

    if( 0 != mhz19_co2 )
    {
#ifdef LOG_IGH_SPEAR_MHZ19
        igh_spear_log("MHZ19..................OK\n");
#endif      
    }
    else
    {
#ifdef LOG_IGH_SPEAR_MHZ19
        igh_spear_log("MHZ19..................ERROR\n");
#endif      
    }
}

void igh_spear_mhz19_service(void)
{

    if( (SERIAL_SENSOR_CO2 != active_system_setting.serial_sensor_type) || (true == spear_serial_sensor_type_updated) )
    { return; } // do nothing if this sensor is not selected
    
    uint32_t mhz19_co2_ppm = 0;
    mhz19_co2_ppm = (uint32_t)myMHZ19.getCO2();

    payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[0] = mhz19_co2_ppm & 0xFF;
    payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[1] = (mhz19_co2_ppm >> 8);
    payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[2] = (mhz19_co2_ppm >> 16);
    payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[3] = (mhz19_co2_ppm >> 24);
    payload_data_store[SENSOR_CARBON_DIOXIDE].new_data = true;

#ifdef LOG_IGH_SPEAR_MHZ19
    sprintf(debug_buff, "mhz19 raw co2: %d ppm\n", mhz19_co2_ppm);
    igh_spear_log(debug_buff);
#endif
    mhz19_timer = millis();
}