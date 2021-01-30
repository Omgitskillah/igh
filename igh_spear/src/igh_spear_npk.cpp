/*******************************************************************************
 * @file igh_spear_npk.cpp
 * @brief Application code for NPK sensor
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_payload.h"
#include "igh_spear_log.h"
#include "igh_spear_settings.h"

// hardware baud rate, should not change
#define NPK_BAUD      (9600)

const uint8_t nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const uint8_t phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const uint8_t pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

uint8_t values[11];

void igh_spear_npk_setup( void )
{
    if( (SERIAL_SENSOR_NPK != active_system_setting.serial_sensor_type) || (true == spear_serial_sensor_type_updated) )
    { return; } // do nothing if this sensor is not selected

    payload_data_store[SENSOR_SOIL_NITROGEN].id = SOIL_NITROGEN;
    payload_data_store[SENSOR_SOIL_NITROGEN].new_data = false;
    payload_data_store[SENSOR_SOIL_NITROGEN].byte_count = 2;

    payload_data_store[SENSOR_SOIL_POTASSIUM].id = SOIL_POTASSIUM;
    payload_data_store[SENSOR_SOIL_POTASSIUM].new_data = false;
    payload_data_store[SENSOR_SOIL_POTASSIUM].byte_count = 2;

    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].id = SOIL_PHOSPHOROUS;
    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].new_data = false;
    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].byte_count = 2;

    Serial1.begin(NPK_BAUD); 
}

uint8_t nitrogen()
{
    if(Serial1.write(nitro,sizeof(nitro))==8)
    {
        for(uint8_t i=0;i<7;i++)
        {
            values[i] = Serial1.read();
            // Serial.print(values[i],HEX);
        }
        // Serial.println();
    }
    return values[4];
}
 
uint8_t phosphorous()
{
    if(Serial1.write(phos,sizeof(phos))==8)
    {
        for(uint8_t i=0;i<7;i++)
        {
            values[i] = Serial1.read();
            // Serial.print(values[i],HEX);
        }
        // Serial.println();
    }
    return values[4];
}
 
uint8_t potassium()
{
    if(Serial1.write(pota,sizeof(pota))==8)
    {
        for(uint8_t i=0;i<7;i++)
        {
            values[i] = Serial1.read();
            // Serial.print(values[i],HEX);
        }
        // Serial.println();
    }
    return values[4];
}

void igh_spear_npk_service( void ) 
{
    if( (SERIAL_SENSOR_NPK != active_system_setting.serial_sensor_type) || (true == spear_serial_sensor_type_updated) )
    { return; } // do nothing if this sensor is not selected

    uint8_t n, p, k;
    delay(250);
    n = nitrogen();
    delay(250);
    p = phosphorous();
    delay(250);
    k = potassium();
    delay(250);

    payload_data_store[SENSOR_SOIL_NITROGEN].bytes[0] = n;
    payload_data_store[SENSOR_SOIL_NITROGEN].bytes[1] = 0;
    payload_data_store[SENSOR_SOIL_NITROGEN].new_data = true;

    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].bytes[0] = p;
    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].bytes[1] = 0;
    payload_data_store[SENSOR_SOIL_PHOSPHOROUS].new_data = true;

    payload_data_store[SENSOR_SOIL_POTASSIUM].bytes[0] = k;
    payload_data_store[SENSOR_SOIL_POTASSIUM].bytes[1] = 0;
    payload_data_store[SENSOR_SOIL_POTASSIUM].new_data = true;

#ifdef LOG_IGH_SPEAR_NPK
    sprintf(debug_buff, "nitrogen: %d, phosphorous: %d, potassium: %d\n", n, p, k);
    igh_spear_log(debug_buff);
#endif
}
