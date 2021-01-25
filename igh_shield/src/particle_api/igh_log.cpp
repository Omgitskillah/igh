/*******************************************************************************
 * @file igh_log.cpp
 * @brief API to test IGH hardware
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include "include/igh_shield.h"
#include "igh_log.h"
#include "igh_eeprom.h"
#include "igh_boron.h"
#include "igh_hardware.h"
#include "igh_sd_log.h"
#include "igh_rfm69.h"
#include "include/igh_button.h"
#include "include/igh_valve.h"


#define IGH_LOG_BAUD (long)19200

const char test_success[] = " OK";
const char test_fail[] = " ERROR";
const char test_header[] = "IGH Shield Test";
const char test_padding[] = "\n***************\n";
const char test_cmd_options[]   = "?. Options\n";
const char test_device_id[]     = "1. Get Device ID\n";
const char test_device_api[]    = "2. Test Device API\n";
const char test_flash_cmd[]     = "3. Test EEPROM\n";
const char test_button_press[]  = "4. Test Irrigation Button\n";
const char test_valve_state[]   = "5. Test Valve Control\n";
const char test_sd_card[]       = "6. Test SD Card\n";
const char test_rfm69_radio[]   = "7. Test Radio\n";

// Valve Test variables
uint8_t test_valve_flag = 0;
unsigned long test_valve_counter = 0;
bool open_close = false;
uint8_t valva_open_pass, valve_closed_pass;
#define valve_transit_time 6000 // set to six seconds, valve takes 5 seconds to change states 

// Button test variables
uint8_t button_test_duration;
uint8_t test_button_flag = 0;
unsigned long test_button_timeout = 0;
unsigned long test_button_timeout_counter = 0;

const char cmd_option       = '?';
const char test_id          = '1';
const char test_device      = '2';
const char test_flash       = '3';
const char test_button      = '4';
const char test_valve       = '5';
const char test_sd          = '6';
const char test_rfm69       = '7';

extern uint32_t button_seconds_counter;

static uint8_t igh_log_read(char * _ch);
static void print_cmd_options(void);
static void igh_append_test_status(uint8_t _status);

uint8_t test_rfm69_flag = 0;

String IGH_ID;

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
#ifdef IGH_DEBUG
    Serial.print(_str);
#endif
}

void igh_log_begin(void)
{
    Serial.begin(IGH_LOG_BAUD);
}

static void print_cmd_options(void)
{
    igh_log_print(test_padding);
    igh_log_print(test_header);
    igh_log_print(test_padding);
    igh_log_print(test_cmd_options);
    igh_log_print(test_device_id);
    igh_log_print(test_device_api);
    igh_log_print(test_flash_cmd);
    igh_log_print(test_button_press);
    igh_log_print(test_valve_state);
    igh_log_print(test_sd_card);
    igh_log_print(test_rfm69_radio);
    // Add more tests here
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
            case cmd_option:
                print_cmd_options();
                ret = 1;
                break;

            case test_id:
                // print out device ID
                igh_log_print(F("\nDevice ID: "));
                IGH_ID = System.deviceID();
                igh_log_print(IGH_ID); igh_log_print("\n"); 
                ret = 1;
                break;
                
            case test_device:
                igh_boron_test_device();
                ret = 1;
                break;

            case test_flash:
                igh_log_print(F("\nTesting EEPROM:"));
                ret = igh_eeproom_test(); // this should be true based on actual test
                igh_append_test_status(ret);  
                break;

            case test_button:
                button_test_duration = random(1, 5); // get a random number to test button press duration
                igh_log_print(F("\nPress button for: ")); igh_log_print(String(button_test_duration)); igh_log_print(F(" Seconds"));
                igh_log_print(F("\nTesting Button:"));
                test_button_flag = 1;
                test_button_timeout = (button_test_duration + 5) * 1000; // + 5 gives you at least 5 seconds to start pressing
                test_button_timeout_counter = millis();
                break;

            case test_valve:
                igh_log_print(F("\nTesting Valve:"));
                test_valve_flag = true;
                test_valve_counter = millis();
                open_close = true;
                break;

            case test_sd:
                igh_log_print(F("\nTesting SD Card:"));
                ret = igh_sd_log_test();
                igh_append_test_status(ret);
                break; 

            case test_rfm69:
                igh_log_print(F("\nTesting Radio:"));
                test_rfm69_flag = 1;
                igh_rfm69_test();
                ret = 1;
            break;

            default:
                igh_log_print(F("\nUnknown command"));
                ret = 0;
                break;
        }
    }
    else
    {
        // Do nothing;
    }

    // Test button routine
    if(test_button_flag)
    {
        if(button_seconds_counter >= button_test_duration)
        {
            ret = 1;
            igh_append_test_status(ret);
            test_button_flag = false;
        }

        if( (millis() - test_button_timeout_counter) >= test_button_timeout )
        {
            ret = 0;
            igh_append_test_status(ret);
            test_button_flag = false;
        }
    }

    // valve test routine
    if(test_valve_flag)
    {
        if(open_close)
        {
#ifdef IGH_DEBUG
            Serial.println("LOG IRRIGATION REQUESTED");
#endif
            igh_valve_change_state( VALVE_OPEN,
                                    valve_transit_time, 
                                    (float)igh_current_system_settings.water_amount_by_button_press );
            open_close = false;
            test_valve_counter = millis();
        }
        
        if( (millis() - test_valve_counter) > (valve_transit_time/2) && 
            (millis() - test_valve_counter) < valve_transit_time )
        {
            if( VALVE_OPEN == current_valve_ctrl.valve_state )
            {
                valve_closed_pass = true;
            }
        }

        if( (millis() - test_valve_counter) >= valve_transit_time )
        {
            if( VALVE_CLOSE == current_valve_ctrl.valve_state )
            {
                valva_open_pass = true;
            }
            
            ret = (valve_closed_pass && valva_open_pass);
            igh_append_test_status(ret);
            test_valve_flag = false;
        }
    }

    if(test_rfm69_flag)
    {
        ret = igh_rfm69_test_service(); // this function is blocking
        igh_append_test_status(ret);
        test_rfm69_flag = false;
    }

    return ret;
}