/*******************************************************************************
 * @file igh_spear_rfm69.cpp
 * @brief API for IGH spear radio
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_log.h"
#include "igh_spear_rfm69.h"   
#include <SPI.h>

/* uncomment to enable debug */
// #define LOG_IGH_SPEAR_RFM69
   
#define FREQUENCY      RF69_915MHZ  // RF69_868MHZ, RF69_433MHZ
#define ENCRYPTKEY     "IGH-RFM69HCW-KEY" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

//initialize radio with potential custom pin outs; otherwise you may use for default: RFM69 radio;
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW);  

// This should be read from the eeprom and not set as default
uint16_t shield_id = 222;
int16_t NETWORKID = 100;  //the same on all nodes that talk to each other
int16_t NODEID = 1;       //make sure NODEID differs between nodes  
int16_t RECEIVER = 222;     //receiver's node ID

bool rfm69_ok;

void igh_spear_rfm69_setup(void)
{
    rfm69_ok = false;
    // Hard Reset the RFM module - Optional
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, HIGH);
    delay(100);
    digitalWrite(RFM69_RST, LOW);
    delay(100);

    radio.setPowerLevel(14); //  power level

    // Initialize radio
    rfm69_ok = radio.initialize(FREQUENCY,NODEID,NETWORKID);
    if (IS_RFM69HCW) {
        radio.setHighPower();    // Only for RFM69HCW & HW!
    }

    radio.writeReg(0x03,0x0D); //set bit rate to 9k6
    radio.writeReg(0x04,0x05);

    radio.encrypt(ENCRYPTKEY);
}

void igh_spear_rfm69_hw_test_service(void)
{
    if( true == rfm69_ok )
    {
#ifdef LOG_IGH_SPEAR_RFM69
        igh_spear_log("RFM69..................OK\n");
#endif 
    }
    else
    {
#ifdef LOG_IGH_SPEAR_RFM69
        igh_spear_log("RFM69..................ERROR\n");
#endif 
    }
    
}

void igh_spear_rfm69_test_service(void)
{
    uint8_t _buffer[RF69_MAX_DATA_LEN];
    uint8_t valid_msg[] = "<TEST>";
    uint8_t resp[] = "<TEST:OK>";
    uint16_t src_id;

    src_id = igh_spear_rfm69_process_incoming_msg( _buffer ); 

    if(  src_id && (0 == memcmp(valid_msg, (uint8_t *)radio.DATA, sizeof(valid_msg))) )
    {
#ifdef LOG_IGH_SPEAR_RFM69
        sprintf(debug_buff, "DEVICE %d : OK\n", src_id);
        igh_spear_log(debug_buff);
#endif
        igh_spear_rfm69_send_2_shield( src_id, resp, sizeof(resp) );
    }
}

void igh_spear_rfm69_send_2_shield( uint16_t dest_id, uint8_t * ptk, uint8_t pkt_len )
{
    // the shield ID should always be read from EEPROM and must be set during pproduction
    radio.send( dest_id, ptk, pkt_len );
    // listening mode
    radio.receiveDone();
}

uint16_t igh_spear_rfm69_process_incoming_msg( uint8_t * buffer )
{
    uint16_t src_id = 0;

    if ( radio.receiveDone() )
    {
        memcpy( buffer, radio.DATA, radio.DATALEN );
        src_id = radio.SENDERID;
    }
    radio.receiveDone();

    return src_id;
}
