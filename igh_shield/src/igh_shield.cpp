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

#define TEST_MODE
#ifdef TEST_MODE
#include "particle_api/igh_log.h"
#endif

SYSTEM_THREAD(ENABLED);

void setup() 
{
    igh_hardware_setup();
#ifdef TEST_MODE
    // go into test mode
    igh_log_begin();
#endif
    igh_boron_setup();
    igh_sd_log_setup();
}

void loop() 
{
#ifdef TEST_MODE
    // process test commands
    igh_process_serial_cmd(); 
#endif
    igh_boron_service();
    igh_hardware_service();
}