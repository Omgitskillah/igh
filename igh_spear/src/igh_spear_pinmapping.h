/*******************************************************************************
 * @file igh_spear_pinmapping.h
 * @brief Pin mapping igh spear on the feather M0 Radio with RFM69
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_SPEAR_PINMAPPING
#define IGH_SPEAR_PINMAPPING

/* RFM69 pins */
#define RFM69_CS        8 // must be set as input pullup
#define RFM69_IO0       3
#define RFM69_IRQ       3 // should be the same as IO0 coz of library
#define RFM69_RST       4

/* On board led */
#define IGH_SPEAR_LED  13

/* VBAT Sense */
#define VBAT_SENSE      A7 // or pin 9

/* Analog Soil Moisture sensor */
#define ANALOG_SOIL_MOISTURE_SENSOR     A2

/* SHT10 Soil temp/hum sensor */
#define SHT10_DATA      10
#define SHT10_CLOCK     12

/* DHT22 Air Temperature and Humidity */
#define DHT_DATA        5 

#endif
