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
#include "src/igh_spear_payload.h"

unsigned long log_timer = 0;
#define LOG_PERIOD 1000

/* defines */

void igh_spear_service( void );
void igh_spear_peripheral_test( void );
void init_igh_spear( void );

void setup()
{
    /* System init functions */
    igh_spear_log_setup();

#if defined(IGH_SPEAR)
    init_igh_spear();
#endif
    /* All states require the RF */
    igh_spear_rfm69_setup();

#if defined(IGH_SPEAR_PERIPHERAL_TEST)
    /* One shot test of all peripherals */
    igh_spear_peripheral_test();
#endif

}

void loop()
{
    /* Main Loop */
    igh_spear_hardware_heartbeat();

#if defined(IGH_SPEAR)
    igh_spear_service();
#elif defined(TEST_RF_COMMS)
    igh_spear_rfm69_test_service();
#endif

}

void init_igh_spear( void )
{
    igh_spear_settings_init();
    igh_spear_lux_meter_setup();
    igh_spear_soil_moisture_sensor_setup();
    igh_spear_sht10_setup();
    igh_spear_dht22_setup();
    igh_spear_mhz19_setup();
    igh_spear_rfm69_setup();
}

void igh_spear_peripheral_test( void )
{
    /* Test Routines */
    igh_spear_hardware_battery_test_service();
    igh_spear_rfm69_hw_test_service();
    igh_spear_settings_test_service();
    igh_spear_lux_meter_test_service();
    igh_spear_soil_mousture_test_service();
    igh_spear_sht10_test_service();
    igh_spear_dht22_test_service();
    igh_spear_mhz19_test_service();
}

void igh_spear_service( void )
{
    igh_spear_payload_tick();
}




