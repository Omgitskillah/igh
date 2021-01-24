/*******************************************************************************
 * @file igh_shield.cpp
 * @brief Main IGH application for the Shield board
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_application.h"

/* Run Application routine */
void setup() 
{
    // setup application
    igh_application_setup();
}

void loop() 
{
    // run main app
    igh_application_churn();
}
