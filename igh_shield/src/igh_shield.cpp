/*******************************************************************************
 * @file igh_shield.cpp
 * @brief Main IGH application for the Shield board
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"


// #define TEST_MODE

#ifdef TEST_MODE
#include "particle_api/igh_eeprom.h"
#include "particle_api/igh_boron.h"
#include "particle_api/igh_hardware.h"
#include "particle_api/igh_sd_log.h"
#include "particle_api/igh_rfm69.h"
#include "particle_api/igh_mqtt.h"
#include "include/igh_message.h"
#include "particle_api/igh_log.h"
#endif

#include "include/igh_app.h"

// SYSTEM_THREAD(ENABLED);

#ifdef TEST_MODE
void igh_shield_test_loop(void);
void igh_shield_test_setup(void);

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

/* Run System test routine */
void setup() 
{
    igh_shield_test_setup();
}

void loop() 
{
    igh_shield_test_loop();
}

#else

/* Run Application routine */
void setup() 
{
    // setup application
    igh_app_setup();
}

void loop() 
{
    // run main app
    igh_main_application();
}

#endif
