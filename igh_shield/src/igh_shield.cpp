/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Illuminum Greenhouses Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_message.h"

extern char igh_msg_buffer[MESSAGE_SIZE];
extern uint8_t size_of_message_id;
extern volatile uint8_t igh_msg_buffer_tracker;

SYSTEM_THREAD(ENABLED);

void setup() 
{
  igh_message_init();

  Serial.begin(115200);
}

void loop() 
{

}
