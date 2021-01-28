/*******************************************************************************
 * @file igh_valve.h
 * @brief Manages the valve function on the IGH project
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_VALVE
#define IGH_VALVE

typedef enum _valve_position_e
{
    VALVE_CLOSE,
    VALVE_OPEN
}valve_position_e;

typedef struct _valve_ctrl_str
{
    valve_position_e valve_state;
    uint32_t open_duration;
    float open_litres;
} valve_ctrl_str;

extern volatile float current_valve_open_water_flow;
extern valve_ctrl_str current_valve_ctrl;

void init_valve( void );
void igh_valve_change_state( valve_position_e _valve_state, uint32_t _open_duration, float _open_flow );
void igh_valve_mngr( void );

#endif