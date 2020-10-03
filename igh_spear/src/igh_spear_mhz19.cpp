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
/* uncomment to enable debug */
// #define LOG_IGH_SPEAR_MHZ19

// hardware baud rate, should not change
#define MHZ19_BAUD      9600 

MHZ19 myMHZ19;

uint16_t mhz19_co2;

unsigned long mhz19_timer = 0;
// TODO: Should come from settings
unsigned long mhz19_read_interval = 2000; // minimum 2 seconds for accuracy

void igh_spear_mhz19_setup(void)
{
    Serial1.begin(MHZ19_BAUD); 
    myMHZ19.begin(Serial1); 

    mhz19_timer = millis();
    
    payload_data_store[SENSOR_CARBON_DIOXIDE].id = CARBON_DIOXIDE;
    payload_data_store[SENSOR_CARBON_DIOXIDE].new_data = false;
}

void igh_spear_mhz19_test_service(void)
{
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
    if( (millis() - mhz19_timer) > mhz19_read_interval )
    {
        mhz19_co2 = 0;
        mhz19_co2 = (uint16_t)myMHZ19.getCO2Raw();

        payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[0] = mhz19_co2 & 0xFF;
        payload_data_store[SENSOR_CARBON_DIOXIDE].bytes[1] = (mhz19_co2 >> 8);
        payload_data_store[SENSOR_CARBON_DIOXIDE].new_data = true;

#ifdef LOG_IGH_SPEAR_MHZ19
        sprintf(debug_buff, "mhz19 raw co2: %d\n", mhz19_co2);
        igh_spear_log(debug_buff);
#endif
        mhz19_timer = millis();
    }
}