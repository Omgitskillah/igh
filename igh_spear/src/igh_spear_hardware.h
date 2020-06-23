/*******************************************************************************
 * @file igh_spear_hardware.h
 * @brief API for native Spear features
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_SPEAR_HARDWARE
#define IGH_SPEAR_HARDWARE

extern uint16_t battery_voltage;

uint16_t igh_spear_get_raw_battery_voltage(void);
void igh_spear_hardware_battery_service(void);
void igh_spear_hardware_heartbeat(void);
void igh_spear_hardware_setup(void);


#endif

