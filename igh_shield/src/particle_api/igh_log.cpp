/*******************************************************************************
 * @file igh_log.cpp
 * @brief API to test IGH hardware
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "igh_eeprom.h"

#define IGH_LOG_BAUD (long)19200

const char test_success[] = "SUCCESS";
const char test_fail[] = "FAIL";
const char test_padding[] = "**********\n";
const char test_cmd_options[]   = "?. Options\n";
const char test_flash_cmd[]     = "1. Test EEPROM\n";

const char cmd_option = '?';
const char test_flash = '1';

static uint8_t igh_log_read(char * _ch);
static void print_cmd_options(void);
static void igh_append_test_status(uint8_t _status);

static uint8_t igh_log_read(char * _ch)
{
    if( Serial.available() )
    {
        *_ch = Serial.read();
        while (Serial.available()) Serial.read(); // remove any other data
        return 1;
    }
    return 0;
}

void igh_log_print(String _str)
{
    Serial.print(_str);
}

void igh_log_begin(void)
{
    Serial.begin(IGH_LOG_BAUD);
}

static void print_cmd_options(void)
{
    igh_log_print(test_padding);
    igh_log_print(test_cmd_options);
    igh_log_print(test_flash_cmd);
    igh_log_print(test_padding);
}

static void igh_append_test_status(uint8_t _status)
{
    if(_status)
    {
        igh_log_print(test_success); igh_log_print("\n");
    }
    else
    {
        igh_log_print(test_fail); igh_log_print("\n");
    }   
}

uint8_t igh_process_serial_cmd(void)
{
    uint8_t ret = 0;
    char resp[10];
    memset(resp, '\0', sizeof(resp));

    char serial_cmd = '\0';

    if( igh_log_read(&serial_cmd) )
    {
        switch(serial_cmd)
        {
            case test_flash:
                igh_log_print(F("\nTesting flash:"));
                ret = igh_eeproom_test(); // this should be true based on actual test
                igh_append_test_status(ret);  
                break;
            
            case cmd_option:
                print_cmd_options();
                ret = 1;
                break;

            default:
                // do nothing
                break;
        }
    }
    else
    {
        // Do nothing;
    }

    return ret;
}