/*******************************************************************************
 * @file igh_spear_log.h
 * @brief System UART LOG API
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_SHIELD_LOG
#define IGH_SHIELD_LOG

/* uncomment to enable debug */
// #define LOG_IGH_SHIELD

#ifdef LOG_IGH_SHIELD
#define DEBUG_BUFF_SIZE 255
#else
#define DEBUG_BUFF_SIZE 0
#endif

extern char debug_buff[DEBUG_BUFF_SIZE];

void igh_shield_log(char * _str);
void igh_shield_log_setup(void);

#endif
