/*******************************************************************************
 * @file igh_spear_rfm69.h
 * @brief API for IGH spear radio
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_SPEAR_RFM69
#define IGH_SPEAR_RFM69

#include "rfm69_hal/RFM69.h"

void igh_spear_rfm69_setup(void);
void igh_spear_rfm69_test_service(void);
void igh_spear_rfm69_hw_test_service(void);

void igh_spear_rfm69_send_2_shield( uint16_t dest_id, uint8_t * ptk, uint8_t pkt_len );
uint16_t igh_spear_rfm69_process_incoming_msg( uint8_t * buffer );
void igh_spear_process_shield_message( void );

#endif
