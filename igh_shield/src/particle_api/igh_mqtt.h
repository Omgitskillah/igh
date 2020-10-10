/*******************************************************************************
 * @file igh_mqtt.h
 * @brief Manages the MQTT protocol for the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_MQTT
#define IGH_MQTT

void igh_mqtt_setup( void );
void igh_mqtt_service( void );
bool igh_mqtt_publish_data( uint8_t *payload, unsigned int len );
// bool igh_mqtt_publish_data( uint8_t *payload );

#endif