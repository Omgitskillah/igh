/*******************************************************************************
 * @file igh_spear.ino
 * @brief Main IGH Spear script based on Arduino for Feather M0 EVM
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

/* Includes */
#include "src/igh_spear_hardware.h"
#include "src/igh_spear_log.h"
unsigned long log_timer = 0;
#define LOG_PERIOD 1000

/* defines */



/* setup */
void setup()
{
    // System init functions
    igh_shield_hardware_setup();
    igh_shield_log_setup();
}


/* loop */
void loop()
{
    // main loop here
    igh_shield_hardware_heartbeat();
    igh_shield_hardware_battery_service();
}




