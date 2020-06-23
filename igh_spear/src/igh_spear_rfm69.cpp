/*******************************************************************************
 * @file igh_spear_rfm69.cpp
 * @brief API for IGH spear radio
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Arduino.h"
#include "igh_spear_pinmapping.h"
#include "igh_spear_rfm69.h"
#include "igh_spear_log.h"

/* uncomment to enable debug */
// #define LOG_IGH_SPEAR_RFM69
#define RF69_FREQ 915.0

RH_RF69 rf69(RFM69_CS, RFM69_IRQ);

int16_t packetnum = 0; 

void Blink(byte PIN, byte DELAY_MS, byte loops); 

void radio_setup(void)
{
    Serial.begin(115200);

    pinMode(IGH_SPEAR_LED, OUTPUT);     
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);

    Serial.println("Feather RFM69 TX Test!");
    Serial.println();

    // manual reset
    digitalWrite(RFM69_RST, HIGH);
    delay(10);
    digitalWrite(RFM69_RST, LOW);
    delay(10);

    if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
    }
    Serial.println("RFM69 radio init OK!");
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
    }

    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

    // The encryption key has to be the same as the one in the server
    uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);

    pinMode(IGH_SPEAR_LED, OUTPUT);

    Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void radio_service(void)
{
    delay(1000);  // Wait 1 second between transmits, could also 'sleep' here!

    char radiopacket[20] = "Hello World #";
    itoa(packetnum++, radiopacket+13, 10);
    Serial.print("Sending "); Serial.println(radiopacket);

    // Send a message!
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();

    // Now wait for a reply
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf69.waitAvailableTimeout(500))  { 
    // Should be a reply message for us now   
    if (rf69.recv(buf, &len)) {
        Serial.print("Got a reply: ");
        Serial.println((char*)buf);
        Blink(IGH_SPEAR_LED, 50, 3); //blink LED 3 times, 50ms between blinks
    } else {
        Serial.println("Receive failed");
    }
    } else {
    Serial.println("No reply, is another RFM69 listening?");
    }   
}

void Blink(byte PIN, byte DELAY_MS, byte loops) 
{
    for (byte i=0; i<loops; i++)  
    {
        digitalWrite(PIN,HIGH);
        delay(DELAY_MS);
        digitalWrite(PIN,LOW);
        delay(DELAY_MS);
    }
}




