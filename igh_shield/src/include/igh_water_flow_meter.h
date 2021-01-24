/*******************************************************************************
 * @file igh_water_flow_meter.h
 * @brief Manages the flow meter
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_WATER_FLOW_METER
#define IGH_WATER_FLOW_METER

extern float total_water_dispensed_Liters;

void igh_water_flow_meter_reset_nv( void );
void igh_water_flow_meter_setup( void );

#endif