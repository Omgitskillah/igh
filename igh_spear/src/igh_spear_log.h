/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SPEAR_LOG
#define IGH_SPEAR_LOG
#include "Arduino.h"
/* uncomment to enable debug */
#define LOG_IGH_SPEAR

#define LOG_IGH_SPEAR_SOIL_MOISTURE_SENSOR
#define LOG_IGH_SPEAR_DHT22
#define LOG_IGH_SPEAR_HARDWARE
#define LOG_IGH_SPEAR_LUX_METER
#define LOG_IGH_SPEAR_MHZ19
#define LOG_IGH_SPEAR_PAYLOAD
#define LOG_IGH_SPEAR_RFM69
#define LOG_IGH_SPEAR_SETTINGS
#define LOG_IGH_SPEAR_SHT10

/* device type */

/* enable this for device to run as spear */
#define IGH_SPEAR 

/* enable this for device to run system tests */
// #define IGH_SPEAR_PERIPHERAL_TEST

/* enable this for feather M0 to receive RF comms data and print over USB serial */
// #define TEST_RF_COMMS

/* end of device types */

#ifdef LOG_IGH_SPEAR
#define DEBUG_BUFF_SIZE 255
#else
#define DEBUG_BUFF_SIZE 0
#endif

extern char debug_buff[DEBUG_BUFF_SIZE];

void igh_spear_log(char * _str);
void igh_spear_log_setup(void);
uint8_t igh_spear_get_serial_hex_data( uint8_t * buffer, uint8_t len );

#endif
