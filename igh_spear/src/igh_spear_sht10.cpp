/*******************************************************************************
 * @file igh_spear_sht10.cpp
 * @brief Application code for SHT10
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_sht10.h"
#include "igh_spear_log.h"

/* uncomment to enable debug */
#define LOG_IGH_SPEAR_SHT10

#define CMD_MEASURETEMPERATURE          sht1x.ShtCommand::MeasureTemperature
#define CMD_MEASURERELATIVEHUMIDITY     sht1x.ShtCommand::MeasureRelativeHumidity
#define SHT_PINS                        SHT10_DATA, SHT10_CLOCK

uint16_t sht10_temperature;
uint16_t sht10_humidity;

unsigned long sht10_read_timer;
// TODO: Should come from settings
unsigned long sht10_read_interval = 1000;

SHT1x sht1x(SHT_PINS);

void igh_spear_sht10_setup(void)
{
    // init variables
    sht10_read_timer = millis();
}

void igh_spear_sht10_test_service(void)
{
    delay(sht10_read_interval); // wait a while
    sht10_humidity = 0;
    sht10_temperature = 0;
    sht10_humidity = sht1x.readRawData( CMD_MEASURERELATIVEHUMIDITY, SHT_PINS );
    sht10_temperature = sht1x.readRawData( CMD_MEASURETEMPERATURE, SHT_PINS );

    if( (0xFF00 == sht10_humidity) || (0xFF00 == sht10_humidity) )
    {
#ifdef LOG_IGH_SPEAR_SHT10
        igh_spear_log("SHT10..................ERROR\n");
#endif      
    }
    else
    {
#ifdef LOG_IGH_SPEAR_SHT10
        igh_spear_log("SHT10..................OK\n");
#endif      
    }
}

void igh_spear_sht10_service(void)
{
    if( (millis() - sht10_read_timer) > sht10_read_interval )
    {
        sht10_humidity = 0;
        sht10_temperature = 0;

        sht10_humidity = sht1x.readRawData( CMD_MEASURERELATIVEHUMIDITY, SHT_PINS );
        sht10_temperature = sht1x.readRawData( CMD_MEASURETEMPERATURE, SHT_PINS );

#ifdef LOG_IGH_SPEAR_SHT10
        sprintf(debug_buff, "SHT10 --> temp: %d, hum: %d\n", sht10_temperature, sht10_humidity);
        igh_spear_log(debug_buff);
#endif
        sht10_read_timer = millis();
    }
}
