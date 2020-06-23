/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SPEAR_LOG
#define IGH_SPEAR_LOG

/* uncomment to enable debug */
// #define LOG_IGH_SPEAR

#ifdef LOG_IGH_SPEAR
#define DEBUG_BUFF_SIZE 255
#else
#define DEBUG_BUFF_SIZE 0
#endif

extern char debug_buff[DEBUG_BUFF_SIZE];

void igh_spear_log(char * _str);
void igh_spear_log_setup(void);

#endif
