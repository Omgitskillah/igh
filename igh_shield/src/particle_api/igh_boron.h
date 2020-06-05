/*******************************************************************************
 * @file igh_boron.cpp
 * @brief API to Boron Device Resoruces
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_BORON
#define IGH_BORON

// Extern variables
extern unsigned long lastSync;
extern bool connected_to_cloud;
extern bool connected_to_network;
extern CellularSignal signal;
extern float signal_quality, signal_strength;
extern FuelGauge fuel;
extern float battery_voltage, battery_SOC;
extern unsigned long unix_time;

/* Function APIs */
void igh_boron_sync_time(void);
uint8_t igh_boron_connected_to_cloud(void);
uint8_t igh_boron_connecetd_to_network(void);
float igh_boron_ss(void);
float igh_boron_sq(void);
void igh_boron_get_celular_data(void);
float igh_boron_voltage(void);
float igh_boron_SoC(void);
unsigned long igh_boron_unix_time(void);
void igh_boron_sync_time(void);
void igh_boron_test_device(void);
void igh_boron_service(void);
void igh_boron_setup(void);

void igh_boron_toggle_boron_led(uint8_t _state);
void igh_boron_button_press_duration(void);
uint8_t igh_boron_read_button(void);

#endif