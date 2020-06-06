  /*******************************************************************************
 * @file igh_sd_log.h
 * @brief API to log and retreave data from SD card
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SD_LOG
#define IGH_SD_LOG

#define IGH_SD_CS   D14 // review this
//MISO, MOSI, SCK: D11, D12, D13 

uint8_t igh_sd_log_setup(void);
void igh_sd_log_get_file_name(unsigned long _unix_time, char * file_name);
uint8_t igh_sd_log_save_data_point(unsigned long _unix_time, uint8_t * data, uint8_t size);
uint8_t igh_sd_log_remove_data_point(char * file_name);
uint8_t igh_sd_log_read_data_point(char * file_name, uint8_t * buffer, uint8_t data_length);
uint8_t igh_sd_log_get_next_file_name(char * next_file_name);
uint8_t igh_sd_log_test(void);

#endif
