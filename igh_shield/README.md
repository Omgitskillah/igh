# FarmShield Firmware Outline
The spears will run an nRF52840 microprocessor which will require the following hardware drivers to read and control the various peripherals.

## Shield HAL Drivers
- SPI driver
- I2C Driver
- UART
- ADC
- EEPROM

## Shield Peripheral Application Layer
All the peripherals on the shield MCU will require modules atop the hardware abstraction layer. These modules will be made modular so that the code may be reusable regardless of any hardware changes in the future.

The following are the modules required for the Shield. 
- Battery Management stack
- Modem Stack
- Firmware OTA stack
- Configuration storage and retrieval stack
- SD card management stack
- Radio Stack
- Data over radio TX/RX stack
- Message Queue and encryption stack
- Relay Control Stack
- Motor Controller stack
- Water Flow Meter stack
- Valve Control Stack
- Light Level Sensor stack

## Shield System Application Layer
This section covers how all the various stacks come together to follow a routine of how the device works according to a process flow. This includes receiving and bundling together data from the various spears paired to the shield, encrypting and transmitting data to the cloud. The system application layer will also handle how the device interacts with the cloud and users.

# Messaging Protocol

Maximum length of data is 256 bytes over MQTT protocol

| Start | length | Msg type | direction | s/n      | msg id | payload  | end  |  
| :---  | :----: | :----:   | :----:    | :----:   | :----: | :----:   | ---: |  
| 0x3C  | 1 byte | 1 byte   | 0x55/0x44 | 12 bytes | 1 byte | variable | 0x3E |

### *Msg type*
|value | Type            |
| :--- | :---            |
| 0x00 | Sensor data msg | 
| 0x11 | Settings msg    |
| 0x22 | Error msg       |
| 0xAA | Message ACK     |

### *direction*
|value | direction            |
| :--- | :---                 |
| 0x55 | Upload to Server     | 
| 0x44 | Download from server |

if message is of type **Message ACK** , **payload** section will not be available  
  
Example serial number (**s/n**) in hex  
```e00fce689a754705e79a0e37``` split into bytes like so ```0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37```  

**msg id** increments and overflows to start over from zero to prevent multiple processing of the same message

### *payload*
Message payload is packaged in form of a string of tuples in the followinf format  

```tuple id, length, data bytes, tuple id, length, data bytes, ..., ..., ...,tuple id, length, data bytes```  

| tuple member | size                          |
| :---         | :---                          |
| tuple id     | 1 byte                        |
| length       | 1 byte                        |
| data bytes   | Specified by **length** above |

### *Currently supported tuple ids and their length*  

| Tuple Name                 | Tuple ID| Length |
| :---                       | :---    | :---   |
|    SPEAR_ID                | 0x01    | 12     |
|    STORE_TIMESTAMP         | 0x02    | 4      |  
|    SEND_TIMESTAMP          | 0x03    | 4      |  
|    SOIL_MOISTURE           | 0x04    | 2      |  
|    AIR_HUMIDITY            | 0x05    | 2      |  
|    SOIL_HUMIDITY           | 0x06    | 2      |  
|    WATER_DISPENSED         | 0x07    | 4      |  
|    CARBON_DIOXIDE          | 0x08    | 2      |  
|    AIR_TEMPERATURE         | 0x09    | 2      |  
|    SOIL_TEMPERATURE        | 0x0A    | 2      |  
|    SOIL_NPK                | 0x0B    | 2      |  
|    LIGHT_INTENSITY         | 0x0C    | 2      |  
|    SHIELD_BATTERY_LEVEL    | 0x0D    | 2      |  
|    SPEAR_BATTERY_LEVEL     | 0x0E    | 2      |  
|    VALVE_POSITION          | 0x0F    | 1      |  


