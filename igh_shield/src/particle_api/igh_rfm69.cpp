  /*******************************************************************************
 * @file igh_rfm69.cpp
 * @brief API to Read and write to the RFM module
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "igh_rfm69.h"
#include "include/igh_settings.h"
#include "particle_api/igh_hardware.h"
#include "include/igh_shield.h"

int16_t NETWORKID   = 100;
int16_t NODEID      = 7;
int16_t TEST_NODE   = 4;

// derived settings
#define RFM69_NETWORK_ID igh_current_system_settings.serial_number[INDEX_RFM69_NETWORK_ID]
#define RFM69_NODE_ID    igh_current_system_settings.serial_number[INDEX_RFM69_NODE_ID]

#define START_BYTE  0x3C
#define END_BYTE    0x3E

byte ackCount = 0;

uint8_t rfm69_powerlevel  = 10; // default power level, about 100M line of site

RFM69 igh_radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);

void igh_rfm69_ack_service( void );

void igh_rfm69_setup(void)
{
    // first reset the module
    pinMode(RFM69_RST, OUTPUT);

    igh_rfm69_reset();
    
    if( false == igh_radio.initialize( FREQUENCY, RFM69_NODE_ID, RFM69_NETWORK_ID) )
#ifdef IGH_DEBUG
    Serial.println("RFM69 INIT ERROR");
    Serial.print( "NEW SHIELD RF ID: " ); Serial.println( RFM69_NODE_ID);
    Serial.print( "NEW NETWORK ID: " ); Serial.println( RFM69_NETWORK_ID);
#endif

    igh_radio.setHighPower(); // This should only be called for RFM69HCW & HW

    igh_rfm69_set_bitrate(RF_BITRATEMSB_9600, RF_BITRATELSB_9600); // set to 9.6Kbps

    igh_rfm69_set_power_level(rfm69_powerlevel);

    // configure the encrytion key, should be standard for all devices in the fleet
    igh_radio.encrypt(ENCRYPTKEY);
} 

void igh_rfm69_set_bitrate(uint8_t msb, uint8_t lsb)
{
    igh_radio.writeReg(REG_BITRATEMSB, msb); 
    igh_radio.writeReg(REG_BITRATELSB, lsb);
}

void igh_rfm69_set_power_level(uint8_t pwrlvl)
{
    igh_radio.setPowerLevel(pwrlvl);
}

void igh_rfm69_reset(void)
{
    digitalWrite(RFM69_RST, HIGH);
    delay(100);
    digitalWrite(RFM69_RST, LOW);
    delay(100);
}

void igh_rfm69_send_pkt(uint8_t * pkt, uint8_t len, int16_t rx_id)
{
    // send but dont ask for ACK, if ACK is required, use (rx_id, pkt, len, true) as arg
    igh_radio.send(rx_id, pkt, len);
    // listen mode
    igh_radio.receiveDone();
}

uint8_t igh_rfm69_process_rx_data(uint8_t * rx_buffer)
{
    if ( igh_radio.receiveDone() )
    {
        uint8_t fist_byte = 0;
        uint8_t last_byte = igh_radio.DATALEN - 2; // reduce index by one then move one byte back behind the null terminator
        // This forces us to add an extra byte at the end of every transmission to hold the null terminator
        char rx[igh_radio.DATALEN];
        memcpy(rx, (char *)igh_radio.DATA, igh_radio.DATALEN);

        if ( (rx[fist_byte] == START_BYTE) && (rx[last_byte] == END_BYTE))
        {
            // only copy over valid data
            memcpy(rx_buffer, (uint8_t *)igh_radio.DATA, igh_radio.DATALEN);
            // this should never happen as we will never request for an ACK
            if( igh_radio.ACKRequested() )
            {
                igh_radio.sendACK();
            }
            // continue listening
            igh_radio.receiveDone();
            return 1;
        }
        else
        {
            igh_radio.receiveDone();
            return 0;
        }
        
    }
    else
    {
        // nothing to do
        igh_radio.receiveDone();
        return 0;
    } 
}

void igh_rfm69_test(void)
{
    uint8_t test_data[] = "<TEST>";
    igh_rfm69_send_pkt(test_data, sizeof(test_data), TEST_NODE);
    
}

uint8_t igh_rfm69_test_service(void)
{
    uint8_t expected_resp[] = "<TEST:OK>";
    uint8_t resp_buffer[10]; // expect a <TEST:OK>
    memset(resp_buffer, '\0', sizeof(resp_buffer));
    
    for(int i = 0; i < 5; i++)
    {
        if( igh_rfm69_process_rx_data(resp_buffer) ) break;
        delay(1000); // wait a total of 5 seconds
    }

    if(0 == memcmp(expected_resp, resp_buffer, sizeof(expected_resp)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

uint8_t igh_rfm69_receive_raw_bytes( uint8_t *buffer, uint8_t len )
{
    uint8_t rx_len = 0;
    if ( igh_radio.receiveDone() )
    {
#ifdef IGH_DEBUG
        byte temperature =  igh_radio.readTemperature(-1);// -1 = user cal factor, adjust for correct ambient
        Serial.print("NEW DATA FROM: "); Serial.print(igh_radio.SENDERID); Serial.print(" RFM TEMP: "); Serial.print(temperature); Serial.println("C");
#endif
        if( igh_radio.DATALEN <= len )
        {
            memcpy( buffer, (uint8_t *)igh_radio.DATA, igh_radio.DATALEN);
            rx_len = igh_radio.DATALEN;
        }
        // service the ACK
        igh_rfm69_ack_service();

        igh_radio.receiveDone();
    }
    return rx_len;
}

void igh_rfm69_ack_service( void )
{
    if (igh_radio.ACKRequested())
    {
        igh_radio.sendACK();
    }
}

void igh_rfm69_service( void )
{
    // if new settings have been sent, re initialize the module
    if( true == initialize_rfm69 )
    {
        igh_rfm69_setup();
        initialize_rfm69 = false;
    }
}