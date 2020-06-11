/*******************************************************************************
 * @file igh_spear_hardware.h
 * @brief API for native Shield features
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_SHIELD_HARDWARE
#define IGH_SHIELD_HARDWARE

extern uint16_t battery_voltage;

uint16_t igh_shield_get_raw_battery_voltage(void);
void igh_shield_hardware_battery_service(void);
void igh_shield_hardware_heartbeat(void);
void igh_shield_hardware_setup(void);


#endif

