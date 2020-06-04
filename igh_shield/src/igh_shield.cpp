/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "particle_api/igh_eeprom.h"
#include "particle_api/igh_boron.h"

#define TEST_MODE
#ifdef TEST_MODE
#include "particle_api/igh_log.h"
#endif

SYSTEM_THREAD(ENABLED);

void setup() 
{
#ifdef TEST_MODE
    // go into test mode
    igh_log_begin();
#endif
    igh_boron_setup();
}

void loop() 
{
#ifdef TEST_MODE
    // process test commands
    igh_process_serial_cmd(); 
#endif
    igh_boron_service();
}