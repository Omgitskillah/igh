/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_log.h"

#define IGH_SHIELD_BAUD 115200

char debug_buff[DEBUG_BUFF_SIZE];

void igh_shield_log_setup(void)
{
#ifdef LOG_IGH_SHIELD
    Serial.begin(IGH_SHIELD_BAUD);
    memset(debug_buff, '\0', sizeof(debug_buff));
#endif
}

void igh_shield_log(char * _str)
{
#ifdef LOG_IGH_SHIELD
    for(uint8_t i = 0; i < sizeof(debug_buff); i++ )
    {
        if(_str[i] == '\0') break;
        Serial.print(_str[i]);
    }
    // reset buffer
    memset(debug_buff, '\0', sizeof(debug_buff));
#endif
}