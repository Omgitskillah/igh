/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "igh_spear_log.h"

#define IGH_SPEAR_BAUD 115200

char test_header[] = "  *** TEST IGH SPEAR ***    \n";
char test_header_line[] = "******************************\n";
char hex_digits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
char debug_buff[DEBUG_BUFF_SIZE];

void igh_spear_log_setup(void)
{
#ifdef LOG_IGH_SPEAR
    Serial.begin(IGH_SPEAR_BAUD);
    // while (!SERIAL_PORT_MONITOR) { }
    delay(1000);
    memset(debug_buff, '\0', sizeof(debug_buff));
    igh_spear_log(test_header);
    igh_spear_log(test_header_line);
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

uint8_t igh_spear_get_serial_hex_data( uint8_t * buffer, uint8_t len )
{
    uint8_t ret = false; 
    if( Serial.available() > 0 && 
        Serial.available() <= len )
    {
        Serial.readBytes( (char *)buffer, len );
        ret = true;
    }
    else if( Serial.available() > len )
    {
        while( Serial.available() )
        {
            Serial.read(); // clear out the buffer but still return false
        }
    }

    return ret;
}