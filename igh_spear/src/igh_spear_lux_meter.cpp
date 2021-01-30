/*******************************************************************************
 * @file igh_spear_lux_meter.cpp
 * @brief Read raw data from ambient light sensor
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include <Wire.h>
#include "igh_spear_lux_meter.h"
#include "igh_spear_log.h"
#include "igh_spear_payload.h"

BH1750 lux_meter;

uint16_t lux;

unsigned long lux_meter_timer = 0;
// TODO: this value should come from settings
unsigned long lux_meter_read_interval = 200; // every 200 milliseconds, this is the fastest we can sample

void igh_spear_lux_meter_setup(void)
{
  // init I2C
  Wire.begin();
  // Init device, scans both addresses
  lux_meter.begin();
  // init timer
  lux_meter_timer = millis();

  payload_data_store[SENSOR_LIGHT_INTENSITY].id = LIGHT_INTENSITY;
  payload_data_store[SENSOR_LIGHT_INTENSITY].new_data = false;
  payload_data_store[SENSOR_LIGHT_INTENSITY].byte_count = 2;
}

uint16_t igh_spear_lux_meter_read(void)
{
  return lux_meter.readLightLevel();
}

void igh_spear_lux_meter_service(void)
{
  // time to get lux data
  lux = igh_spear_lux_meter_read();
  payload_data_store[SENSOR_LIGHT_INTENSITY].bytes[0] = lux & 0xFF;
  payload_data_store[SENSOR_LIGHT_INTENSITY].bytes[1] = (lux >> 8);
  payload_data_store[SENSOR_LIGHT_INTENSITY].new_data = true;
#ifdef LOG_IGH_SPEAR_LUX_METER
  sprintf(debug_buff, "Light: %d\n", lux);
  igh_spear_log(debug_buff);
#endif
}

void igh_spear_lux_meter_test_service( void )
{
    delay(lux_meter_read_interval); // wait one second then read the sensor, make sure there is light for test to pass
    lux = igh_spear_lux_meter_read();

    if ( 0 != lux )
    {
#ifdef LOG_IGH_SPEAR_LUX_METER
      igh_spear_log("LUX METER..............OK\n");
#endif      
    }
    else
    {
#ifdef LOG_IGH_SPEAR_LUX_METER
      igh_spear_log("LUX METER..............ERROR\n");
#endif      
    }
    
}
