/*******************************************************************************
 * @file igh_spear_payload.h
 * @brief API for building the igh payload from spear to shield
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SPEAR_PAYLOAD
#define IGH_SPEAR_PAYLOAD

#include "Arduino.h"

/**
 * SPEAR SENSOR LIST *
 * Battery voltage
 * CO2 sensor
 * air humidity (DHT22)
 * air temperature (DHT22)
 * soil humidity (SHT10)
 * soil temperature (SHT10)
 * NPK sensor
 * Leaf wetness sensor
 * Lux meter
 * Soil Moisture
 * */
typedef enum igh_pkt_id
{
    MSG_ACK_TUPLE           = 0x00,
    SPEAR_ID                = 0x01,
    STORE_TIMESTAMP         = 0x02,
    SEND_TIMESTAMP          = 0x03,
    SOIL_MOISTURE           = 0x04,
    AIR_HUMIDITY            = 0x05,
    SOIL_HUMIDITY           = 0x06,
    WATER_DISPENSED         = 0x07,
    CARBON_DIOXIDE          = 0x08,
    AIR_TEMPERATURE         = 0x09,
    SOIL_TEMPERATURE        = 0x0A,
    SOIL_NPK                = 0x0B,
    LIGHT_INTENSITY         = 0x0C,
    SHIELD_BATTERY_LEVEL    = 0x0D,
    SPEAR_BATTERY_LEVEL     = 0x0E,
    VALVE_POSITION          = 0x0F,
    IGH_SEND_SETTINGS       = 0x10,
    IGH_READ_SETTINGS       = 0x11,
    SPEAR_DATA              = 0x12,
    END_OF_PKT_ID           = 0xFF
} pkt_id;

enum sensors
{
    SENSOR_SPEAR_BATTERY_LEVEL,
    SENSOR_LIGHT_INTENSITY,
    SENSOR_SOIL_MOISTURE,
    SENSOR_SOIL_HUMIDITY,
    SENSOR_SOIL_TEMPERATURE,
    SENSOR_AIR_HUMIDITY,
    SENSOR_AIR_TEMPERATURE,
    SENSOR_CARBON_DIOXIDE,

    SENSOR_WATER_DISPENSED,
    SENSOR_SOIL_NPK,
    // number of sensors
    NUM_OF_SENSORS
};

typedef struct sensor_data
{
    pkt_id id;
    uint8_t new_data;
    uint8_t bytes[2]; // bytes in little endian
} igh_spear_sensor_data; 

extern igh_spear_sensor_data  payload_data_store[NUM_OF_SENSORS];


void igh_spear_payload_tick( void );

#endif
