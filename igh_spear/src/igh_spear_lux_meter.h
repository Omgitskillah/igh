/*******************************************************************************
 * @file igh_spear_lux_meter.h
 * @brief Read raw data from ambient light sensor
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SPEAR_LUX_METER
#define IGH_SPEAR_LUX_METER

#include "lux_meter/BH1750.h"

void igh_spear_lux_meter_setup( void );
void igh_spear_lux_meter_test_service( void );
void igh_spear_lux_meter_service( void );

#endif
