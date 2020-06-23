/*******************************************************************************
 * @file igh_spear_rfm69.h
 * @brief API for IGH spear radio
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_SPEAR_RFM69
#define IGH_SPEAR_RFM69

#include "SPI.h"
#include "RadioHead/RH_RF69.h"

void radio_setup(void);
void radio_service(void);

#endif
