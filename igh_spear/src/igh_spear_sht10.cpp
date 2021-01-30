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
#include "igh_spear_payload.h"

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

    payload_data_store[SENSOR_SOIL_HUMIDITY].id = SOIL_HUMIDITY;
    payload_data_store[SENSOR_SOIL_HUMIDITY].new_data = false;
    payload_data_store[SENSOR_SOIL_HUMIDITY].byte_count = 2;

    payload_data_store[SENSOR_SOIL_TEMPERATURE].id = SOIL_TEMPERATURE;
    payload_data_store[SENSOR_SOIL_TEMPERATURE].new_data = false;
    payload_data_store[SENSOR_SOIL_TEMPERATURE].byte_count = 2;
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
    sht10_humidity = 0;
    sht10_temperature = 0;
    sht10_humidity = sht1x.readRawData( CMD_MEASURERELATIVEHUMIDITY, SHT_PINS );
    sht10_temperature = sht1x.readRawData( CMD_MEASURETEMPERATURE, SHT_PINS );

    payload_data_store[SENSOR_SOIL_HUMIDITY].bytes[0] = sht10_humidity & 0xFF;
    payload_data_store[SENSOR_SOIL_HUMIDITY].bytes[1] = (sht10_humidity >> 8);
    payload_data_store[SENSOR_SOIL_HUMIDITY].new_data = true;

    payload_data_store[SENSOR_SOIL_TEMPERATURE].bytes[0] = sht10_temperature & 0xFF;
    payload_data_store[SENSOR_SOIL_TEMPERATURE].bytes[1] = (sht10_temperature >> 8);
    payload_data_store[SENSOR_SOIL_TEMPERATURE].new_data = true;
#ifdef LOG_IGH_SPEAR_SHT10
    sprintf(debug_buff, "SHT10 --> temp: %d, hum: %d\n", sht10_temperature, sht10_humidity);
    igh_spear_log(debug_buff);
#endif
}
