/*******************************************************************************
 * @file igh_hardware.cpp
 * @brief API to IGH hardware peripherals
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_HARDWARE
#define IGH_HARDWARE

#define ON        1
#define OFF       0

// Pin Mapping
#define BORON_LED               D7
#define IGH_IRRIGATION_BUTTON   D5
#define IGH_IRRIGATION_VALVE    D7 //TODO replace with actual pin, route to on board LED in the mean time 

enum valve_position
{
    VALVE_CLOSE,
    VALVE_OPEN
};
typedef enum valve_position valve_position;

// extern variables
extern uint8_t igh_button_sec_counter;
extern volatile valve_position current_valve_position;
 
void igh_hardware_setup(void);
void igh_hardware_service(void);
void igh_boron_toggle_boron_led(uint8_t _state);
void igh_boron_button_press_duration(void);
uint8_t igh_boron_read_button(void);

uint8_t igh_hardware_test_valve_state(void);
void igh_hardware_service_valve_state(void);

#endif
