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

#define ONE_SECOND 1000

// Pin Mapping
#define BORON_LED               D7
#define IGH_IRRIGATION_BUTTON   A4

#define IGH_VALVE_YELLOW        D0
#define IGH_VALVE_BLUE          D1
#define IGH_VALVE_CLOSE         A1
#define IGH_VALVE_OPEN          A0

#define IGH_WATER_FLOW_PIN      D5

// time constants
#define TWENTY_FOUR_HOURS      (24)
#define MIDNIGHT               (0)
#define TWO_SECONDS            (2)
#define THREE_SECONDS          (3)
#define FIVE_SECONDS           (5)

typedef struct _irrigation_params_str
{
    uint32_t irrigation_state;
    uint32_t min_amount_of_water_dispens_status;
    uint32_t max_amount_of_water_dispens_status;
    float water_dispensed_store;
}irrigation_params_str;

// extern variables
extern uint8_t igh_button_sec_counter;

#define INVALID_SOIL_DATA      0x00
#define VALID_SOIL_DATA        0xFF
extern float total_water_dispensed_Liters;
extern uint16_t soil_humidity;
extern uint16_t soil_temperature;
extern uint8_t refreshed_soil_data;

void igh_hardware_setup(void);
void igh_hardware_service(void);
void igh_boron_toggle_boron_led(uint8_t _state);
void igh_boron_button_press_duration(void);
uint8_t igh_boron_read_button(void);
void reset_irrigation_params(void);

uint8_t igh_hardware_test_valve_state(void);
void igh_hardware_service_valve_state(void);

#endif
