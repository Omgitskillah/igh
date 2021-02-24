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

#define ONE_SECOND  1000

// Pin Mapping
#define BORON_LED               D7
#define IGH_IRRIGATION_BUTTON   A4

#define IGH_VALVE_YELLOW        D0
#define IGH_VALVE_BLUE          D1
#define IGH_VALVE_CLOSE         A1
#define IGH_VALVE_OPEN          A0
#define IGH_VALVE_EN            A3

#define IGH_WATER_FLOW_PIN      D5

#define RFM69_CS                A2 
#define RFM69_IRQ               D8
#define RFM69_IRQN              D8 
#define RFM69_RST               D6 

// time constants
#define THREE_SECONDS          (3)
#define TEN_SECONDS            (10)

#define BUTTON_PRESS_TOGGLE_VALVE THREE_SECONDS
#define BUTTON_PRESS_TOGGLE_SYS   TEN_SECONDS

// extern variables

void igh_hardware_setup(void);
void igh_boron_toggle_boron_led(uint8_t _state);

#endif
