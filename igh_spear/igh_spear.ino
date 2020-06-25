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

    // should be done only once
    igh_spear_settings_test_service();
}


/* loop */
void loop()
{
    // main loop here
    igh_spear_hardware_heartbeat();
    igh_spear_hardware_battery_service();
    igh_spear_rfm69_test_service();
}




