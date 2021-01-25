/*******************************************************************************
 * @file igh_irrigation.h
 * @brief Manages the irrigation function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_IRRIGATION
#define IGH_IRRIGATION

extern bool igh_irrigation_enabled;
extern bool irrigation_suspended;
extern bool remote_valve_command;
extern valve_position_e remote_valve_state;

void igh_irrigation_by_button( void );
void igh_irrigation_mngr( void );
void igh_irrigation_update_sensor_data( uint16_t sensor_data );
void igh_irrigation_toggle( void );

#endif