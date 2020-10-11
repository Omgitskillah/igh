  /*******************************************************************************
 * @file igh_rfm69.h
 * @brief API to Read and write to the RFM module
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_RFM69
#define IGH_RFM69

#include "RFM69-Particle.h"
#include "RFM69_ATC.h"
#include "RFM69registers.h"

#define FREQUENCY       RF69_915MHZ
#define ENCRYPTKEY      "IGH-RFM69HCW-KEY" 
#define IS_RFM69HCW     true

/// Pin mapping
#define RFM69_CS      A2 
#define RFM69_IRQ     D8
#define RFM69_IRQN    D8 
#define RFM69_RST     D6 

/* Functions */
uint8_t igh_rfm69_test_service(void);
void igh_rfm69_test(void);

uint8_t igh_rfm69_receive_raw_bytes( uint8_t *buffer, uint8_t len );

uint8_t igh_rfm69_process_rx_data(uint8_t * rx_buffer);
void igh_rfm69_send_pkt(uint8_t * pkt, uint8_t len, int16_t rx_id);
void igh_rfm69_reset(void);
void igh_rfm69_set_power_level(uint8_t pwrlvl);
void igh_rfm69_set_bitrate(uint8_t msb, uint8_t lsb);
void igh_rfm69_setup(void);
void igh_rfm69_service( void );

#endif
