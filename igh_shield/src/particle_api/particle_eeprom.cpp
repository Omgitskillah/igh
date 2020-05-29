/*******************************************************************************
 * @file particle_eeprom.cpp
 * @brief Save and retreave persistent settings from particle eeprom emulator
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_settings.h"

#define SYSTEM_SETTINGS_ADDRESS 0x0000
#define SYSTEM_THRESHOLDS_ADDRESS 0x0000
#define SYSTEM_ERRORS_ADDRESS 0x0000

/* save data */

/*******************************************************************************
 * particle_eeprom_save_system_settings
 *******************************************************************************/
/**
 * \brief Save system settings  to eeprom emulator
 * \param settings_to_save: pointer to struct to save
 * \return true or false based on write success
 */
uint8_t particle_eeprom_save_system_settings(struct system_settings * settings_to_save)
{
  system_settings local_buffer;
  // save the data
  EEPROM.put(SYSTEM_SETTINGS_ADDRESS, &settings_to_save);
  // check if data is saved
  EEPROM.get(SYSTEM_THRESHOLDS_ADDRESS, local_buffer);

  if( local_buffer.checksum != (settings_to_save->checksum))
  {
    return 0;
  }

  return 1;
}

/*******************************************************************************
 * particle_eeprom_save_threshold_settings
 *******************************************************************************/
/**
 * \brief Save threshold settings  to eeprom emulator
 * \param settings_to_save: pointer to struct to save
 * \return true or false based on write success
 */
uint8_t particle_eeprom_save_threshold_settings(thresholds * settings_to_save)
{
  thresholds local_buffer;
  // save the data
  EEPROM.put(SYSTEM_THRESHOLDS_ADDRESS, &settings_to_save);
  // check if data is saved
  EEPROM.get(SYSTEM_THRESHOLDS_ADDRESS, local_buffer);

  if( local_buffer.checksum != (settings_to_save->checksum))
  {
    return 0;
  }

  return 1;
}

/*******************************************************************************
 * particle_eeprom_update_errors
 *******************************************************************************/
/**
 * \brief Save errors bit field
 * \param error_bit_field: error bit field
 * \return void
 */
void particle_eeprom_update_errors(uint32_t error_bit_field)
{
  EEPROM.put(SYSTEM_ERRORS_ADDRESS, error_bit_field);
}



/*******************************************************************************
 * particle_eeprom_read_system_settings
 *******************************************************************************/
/**
 * \brief Gets system settings from eeprom emulator
 * \param running_system_settings_buffer: buffer to store settings in
 * \return true or false based on success of read
 */
uint8_t particle_eeprom_read_system_settings(struct system_settings running_system_settings_buffer)
{
  system_settings local_buffer;

  EEPROM.get(SYSTEM_SETTINGS_ADDRESS, local_buffer);

  uint8_t checksum = igh_settings_calculate_system_settings_checksum(local_buffer);
  
  if( checksum != local_buffer.checksum)
  {
    return 0;
  }

  running_system_settings_buffer = local_buffer;
  return 1;

}

uint8_t particle_eeprom_read_threshold_settings(struct thresholds running_thresholds)
{
  thresholds local_buffer;

  EEPROM.get(SYSTEM_THRESHOLDS_ADDRESS, local_buffer);

  uint8_t checksum = igh_settings_calculate_threshold_settings_checksum(local_buffer);
  
  if( checksum != local_buffer.checksum)
  {
    return 0;
  }
  
  running_thresholds = local_buffer;
  return 1;
}

void particle_eeprom_get_errors(uint32_t error_bit_field)
{
  EEPROM.get(SYSTEM_ERRORS_ADDRESS, error_bit_field);
}
