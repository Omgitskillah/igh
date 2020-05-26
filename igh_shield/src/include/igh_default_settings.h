/*******************************************************************************
 * @file igh_air.h
 * @brief manage air sensor data
 * @auther Alucho C. Ayisi
 * Copyright (C), Illuminum Greenhouses Ltd. All rights reserved.
 *******************************************************************************/

#ifndef IGH_DEFAULT_SETTINGS
#define IGH_DEFAULT_SETTINGS

#ifdef __cplusplus
extern "C" {
#endif

// Sysem default settings for both shield and spear

// System settings
#define DEFAULT_NEW_OPSTATE                     OP_INACTIVE
#define DEFAULT_REPORTING_INTERVAL              255   
#define DEFAULT_DATA_RESOLUTION                 255
//High Threshold tirggers       
#define DEFAULT_SOIL_MOISTURE_LOW               0            
#define DEFAULT_AIR_HUMIDITY_LOW                0            
#define DEFAULT_SOIL_HUMIDITY_LOW               0                 
#define DEFAULT_CARBON_DIOXIDE_LOW              0   
#define DEFAULT_AIR_TEMPERATURE_LOW             0            
#define DEFAULT_SOIL_TEMPERATURE_LOW            0            
#define DEFAULT_SOIL_NPK_LOW                    0            
#define DEFAULT_LIGHT_INTENSITY_LOW             0            
#define DEFAULT_SHIELD_BATTERY_LEVEL_LOW        0            
#define DEFAULT_SPEAR_BATTERY_LEVEL_LOW         0           
#define DEFAULT_WATER_DISPENSED_PERIOD_LOW      0                 
// Low Threshold Trigger        
#define DEFAULT_SOIL_MOISTURE_HIGH              255             
#define DEFAULT_AIR_HUMIDITY_HIGH               255          
#define DEFAULT_SOIL_HUMIDITY_HIGH              255                  
#define DEFAULT_CARBON_DIOXIDE_HIGH             255   
#define DEFAULT_AIR_TEMPERATURE_HIGH            255            
#define DEFAULT_SOIL_TEMPERATURE_HIGH           255            
#define DEFAULT_SOIL_NPK_HIGH                   255            
#define DEFAULT_LIGHT_INTENSITY_HIGH            255            
#define DEFAULT_SHIELD_BATTERY_LEVEL_HIGH       255            
#define DEFAULT_SPEAR_BATTERY_LEVEL_HIGH        255                     
#define DEFAULT_WATER_DISPENSED_PERIOD_HIGH     255
#define DEFAULT_SERIAL_NUMBER                   {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}

#ifdef __cplusplus
}
#endif

#endif