/*******************************************************************************
 * @file igh_spear.ino
 * @brief Main IGH Spear script based on Arduino for Feather M0 EVM
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

/* Includes */
#include "src/igh_spear_hardware.h"
#include "src/igh_spear_log.h"
#include "src/igh_spear_rfm69.h"
#include "src/igh_spear_settings.h"
#include "src/igh_spear_soil_moisture_sensor.h"
#include "src/igh_spear_lux_meter.h"
#include "src/igh_spear_sht10.h"
#include "src/igh_spear_dht22.h"
#include "src/igh_spear_mhz19.h"

unsigned long log_timer = 0;
#define LOG_PERIOD 1000

/* defines */



/* setup */
void setup()
{
    // System init functions
    igh_spear_hardware_setup();
    igh_spear_log_setup();
    igh_spear_rfm69_setup();
    igh_spear_lux_meter_setup();
    igh_spear_soil_moisture_sensor_setup();
    igh_spear_sht10_setup();
    igh_spear_dht22_setup();
    igh_spear_mhz19_setup();

    // should be done only once
    igh_spear_hardware_battery_test_service();
    igh_spear_rfm69_hw_test_service();
    igh_spear_settings_test_service(); // TODO: uncomment this last, using up flash writes
    igh_spear_lux_meter_test_service();
    igh_spear_soil_mousture_test_service();
    igh_spear_sht10_test_service();
    igh_spear_dht22_test_service();
    igh_spear_mhz19_test_service();
}


/* loop */
void loop()
{
    // main loop here
    
    igh_spear_hardware_heartbeat();
    // igh_spear_hardware_battery_service();
    // igh_spear_rfm69_test_service();
    // igh_spear_lux_meter_service();
    // igh_spear_soil_mousture_service();
    // igh_spear_sht10_service();
    // igh_spear_dht22_service();
    // igh_spear_mhz19_service();
}




