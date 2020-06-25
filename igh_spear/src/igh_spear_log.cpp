/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_log.h"

#define IGH_SPEAR_BAUD 115200

char debug_buff[DEBUG_BUFF_SIZE];

void igh_spear_log_setup(void)
{
#ifdef LOG_IGH_SPEAR
    Serial.begin(IGH_SPEAR_BAUD);
    while (!SERIAL_PORT_MONITOR) { }
    memset(debug_buff, '\0', sizeof(debug_buff));
    igh_spear_log("  *** TEST IGH SPEAR ***    \n");
    igh_spear_log("******************************\n");
#endif
}

void igh_spear_log(char * _str)
{
#ifdef LOG_IGH_SPEAR
    for(uint8_t i = 0; i < sizeof(debug_buff); i++ )
    {
        if(_str[i] == '\0') break;
        Serial.print(_str[i]);
    }
    // reset buffer
    memset(debug_buff, '\0', sizeof(debug_buff));
#endif
}