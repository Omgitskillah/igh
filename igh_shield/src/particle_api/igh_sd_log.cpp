  /*******************************************************************************
 * @file igh_sd_log.cpp
 * @brief API to log and retreave data from SD card
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#include "Particle.h"
#include <SPI.h>
#include "SdFat.h"
#include "igh_sd_log.h"

SdFat igh_sd;
File igh_file;
SdFile sd_root;
SdFile next_file;

/* File name format
 * Since we are limited to a 8.3 naming routine,
 * file names are the timestamp in hex to meet 8 characters
 * and '.log' as the extension
 * The data contained will be the entire memory conetnts
 * convertedt to a character string
 * eg: 2E78FA2E.LOG
*/

uint8_t igh_sd_log_setup(void)
{
    if(sd_root.isOpen()) sd_root.close();
    return igh_sd.begin(IGH_SD_CS);
}

void igh_sd_log_rewind_sd(void)
{
    igh_file.rewindDirectory();
}

void igh_sd_log_get_file_name(unsigned long _unix_time, char * file_name)
{
    sprintf(file_name, "%02X%02X%02X%02X.LOG", 
    (uint8_t)(_unix_time >> 24), 
    (uint8_t)(_unix_time >> 16), 
    (uint8_t)(_unix_time >> 8), 
    (uint8_t)(_unix_time));
}

uint8_t igh_sd_log_save_data_point(unsigned long _unix_time, uint8_t * data, uint8_t size)
{
    uint8_t ret = 0;
    char name[FILE_NAME_SIZE]; // include the null terminator?
    igh_sd_log_get_file_name(_unix_time, name);
    igh_file = igh_sd.open((const char *)name, FILE_WRITE);

    // Serial.print("Saving: "); Serial.print(name); Serial.print(" Size: "); Serial.print(size);

    if(igh_file)
    {
        igh_file.write( data, size);
        // Serial.println(" OK");
        ret = 1;
    }
    else
    {
        // Serial.println(" ERROR");
    }

    igh_file.close();
    return ret;
}

uint8_t igh_sd_log_remove_data_point(char * file_name)
{
    return igh_sd.remove(file_name);
}

uint8_t igh_sd_log_clear_sd_data(void)
{
    if(!igh_sd_log_setup())
    {
        return 0;
    }

    if (!sd_root.open("/"))
    {
        return 0;
    }
    else
    {
        while ( next_file.openNext(&sd_root, O_RDONLY) )
        {
            char file_to_delete[FILE_NAME_SIZE];
            memset(file_to_delete, '\0', FILE_NAME_SIZE);

            next_file.getSFN(file_to_delete);
            next_file.close();
            igh_sd.remove(file_to_delete);
        }
        igh_file.rewindDirectory();
        return 1;
    }
}

uint8_t igh_sd_log_read_data_point(char * file_name, uint8_t * buffer, uint8_t data_length)
{
    igh_file = igh_sd.open( (const char *)file_name );
    if(igh_file)
    {
        for(int i = 0; i < data_length; i++)
        {
            buffer[i] = igh_file.read(); // read data_len number of chars
        }
        igh_file.close();
        return 1;
    }
    else
    {
        return 0;
    }   
}

uint8_t igh_sd_log_get_next_file_name(char * next_file_name)
{
    if(!igh_sd_log_setup())
    {
        return 0;
    }

    if (!sd_root.open("/"))
    {
        return 0;
    }
    else
    {
        next_file.openNext(&sd_root, O_RDONLY); // skip one index
        next_file.close(); 
        if( next_file.openNext(&sd_root, O_RDONLY) )
        {
            next_file.getSFN(next_file_name);
            next_file.close(); 
            return 1;
        }
        else
        {
            return 0;
        }    
    }
}

uint8_t igh_sd_log_test(void)
{
    if( !igh_sd_log_clear_sd_data() )
    {
        return 0;
    }

    unsigned long test_timestamp = random(2015707648); 

    uint8_t size = MAX_FILE_SIZE;
    uint8_t data[size];

    memset(data, 't', size);
    if ( !igh_sd_log_save_data_point(test_timestamp, data, size) )
    {
        return 0;
    }

    char test_name[FILE_NAME_SIZE];
    uint8_t test_buffer[size];
    igh_sd_log_get_file_name(test_timestamp, test_name);

    if( !igh_sd_log_read_data_point(test_name, test_buffer, size) )
    {
        return 0;
    }

    if ( 0 != memcmp( data, test_buffer, size) )
    {
        return 0;
    }

    char read_name[FILE_NAME_SIZE];

    if( !igh_sd_log_get_next_file_name(read_name) )
    {
        return 0;
    }

    
    if( 0 != memcmp( test_name, read_name, FILE_NAME_SIZE) )
    {
        return 0;
    }

    if( !igh_sd_log_remove_data_point(read_name) )
    {
        return 0;
    }

    return 1;
}