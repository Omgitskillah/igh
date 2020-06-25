/*******************************************************************************
 * @file igh_spear_soil_moisture_sensor.cpp
 * @brief Simple abstraction to analog soil moisture sensor
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_soil_moisture_sensor.h"
#include "igh_spear_log.h"

/* uncomment to enable debug */
#define LOG_IGH_SPEAR_SOIL_MOISTURE_SENSOR

uint16_t soil_moisture;

unsigned long soil_moisture_sensor_timer = 0;
// TODO: should come from settings
unsigned long soil_moisture_sensor_inverval = 500; // 500 mS

void igh_spear_soil_moisture_sensor_setup(void)
{
    pinMode(ANALOG_SOIL_MOISTURE_SENSOR, INPUT);

    soil_moisture_sensor_timer = millis();
}

void igh_spear_soil_mousture_test_service(void)
{
    soil_moisture = 0;
    delay(soil_moisture_sensor_inverval); // wait for a while

    soil_moisture = analogRead(ANALOG_SOIL_MOISTURE_SENSOR);

    if( 0 != soil_moisture )
    {
#ifdef LOG_IGH_SPEAR_SOIL_MOISTURE_SENSOR
        igh_spear_log("SOIL MOISTURE SENSOR...OK\n");
#endif      
    }
    else
    {
#ifdef LOG_IGH_SPEAR_SOIL_MOISTURE_SENSOR
        igh_spear_log("SOIL MOISTURE SENSOR...ERROR\n");
#endif      
    }
}

void igh_spear_soil_mousture_service(void)
{
    if ( (millis() - soil_moisture_sensor_timer) > soil_moisture_sensor_inverval )
    {
        soil_moisture = 0;
        soil_moisture = analogRead(ANALOG_SOIL_MOISTURE_SENSOR);

#ifdef LOG_IGH_SPEAR_SOIL_MOISTURE_SENSOR
        sprintf(debug_buff, "Soil Moisture: %d\n", soil_moisture);
        igh_spear_log(debug_buff);
#endif
        soil_moisture_sensor_timer = millis();
    }
}