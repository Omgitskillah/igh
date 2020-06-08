/*******************************************************************************
 * @file igh_shield.cpp
 * @brief Main IGH application for the Shield board
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "particle_api/igh_eeprom.h"
#include "particle_api/igh_boron.h"
#include "particle_api/igh_hardware.h"
#include "particle_api/igh_sd_log.h"
#include "particle_api/igh_rfm69.h"

#define TEST_MODE

#ifdef TEST_MODE
#include "particle_api/igh_log.h"
#endif

SYSTEM_THREAD(ENABLED);

void igh_shield_test_loop(void);
void igh_shield_test_setup(void);

void setup() 
{
#ifdef TEST_MODE
    igh_shield_test_setup();
#endif

}

void loop() 
{
#ifdef TEST_MODE
    igh_shield_test_loop();
#endif

}

void igh_shield_test_setup(void)
{
    igh_hardware_setup();
    igh_boron_setup();
    igh_sd_log_setup();
    igh_rfm69_setup();
    igh_log_begin();
}

void igh_shield_test_loop(void)
{
    // process test commands
    igh_process_serial_cmd(); 
    // Service platform and hardawre
    igh_boron_service();
    igh_hardware_service();
}