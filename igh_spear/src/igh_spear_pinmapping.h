/*******************************************************************************
 * @file igh_spear_pinmapping.h
 * @brief Pin mapping igh spear on the feather M0 Radio with RFM69
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef IGH_SPEAR_PINMAPPING
#define IGH_SPEAR_PINMAPPING

/* RFM69 pins */
#define RFM69_CS        8 // must be set as input pullup
#define RFM69_IO0       3
#define RFM69_IRQ       3 // should be the same as IO0 coz of library
#define RFM69_RST       4

/* On board led */
#define IGH_SHIELD_LED  13

/* VBAT Sense */
#define VBAT_SENSE      A7 // or pin 9

#endif


#ifdef __cplusplus
} // extern "C" {
#endif