/*******************************************************************************
 * @file igh_log.cpp
 * @brief API to test IGH hardware
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/
#ifndef IGH_LOG
#define IGH_LOG

void igh_log_begin(void);
void igh_log_print(String _str);
uint8_t igh_process_serial_cmd(void);

#endif