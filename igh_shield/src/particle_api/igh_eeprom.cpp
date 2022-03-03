/*******************************************************************************
 * @file igh_eeprom.cpp
 * @brief Save and retreave persistent settings from particle eeprom emulator
 * @auther Alucho C. Ayisi
 * Copyright (C), Synnefa Green Ltd. All rights reserved.
 *******************************************************************************/

#include "Particle.h"
#include "include/igh_shield.h"
#include "include/igh_button.h"
#include "include/igh_message.h"
#include "igh_eeprom.h"

// We only have 4KB for the Boron
#define SYSTEM_SETTINGS_ADDRESS           (int)0x0000
#define SYSTEM_THRESHOLDS_ADDRESS         (int)0x0200
#define SYSTEM_ERRORS_ADDRESS             (int)0x0400
#define SYSTEM_NV_WATER_STORE_ADDRESS     (int)0x0600
#define SYSTEM_NV_DAY                     (int)0x0800

#define ONE_SECOND_MS 1000 // one second in ms
#define THREE_SECONDS 3 // 3 second in s

void igh_eeprom_update_errors(uint32_t error_bit_field);
uint8_t igh_eeprom_read_system_settings(system_settings * running_system_settings_buffer);
uint8_t igh_eeprom_read_threshold_settings(thresholds * running_thresholds);
void igh_eeprom_get_errors(uint32_t * error_bit_field);
void igh_eeprom_check_to_clear( void );


void igh_eeprom_init( void )
{
  system_settings settings_in_memory;
  thresholds thresholds_in_memory;

  igh_eeprom_check_to_clear();

  bool system_settings_read_successfully = igh_eeprom_read_system_settings(&settings_in_memory);
  uint8_t valid_system_checksum = igh_settings_calculate_checksum( &settings_in_memory, sizeof(settings_in_memory) );

  if( false == system_settings_read_successfully ||
      (settings_in_memory.checksum != valid_system_checksum) ||
      ( 0 == settings_in_memory.checksum) )
  {
    // if we can't get valid settings from memory at all, use default settings
#ifdef IGH_DEBUG
    Serial.println("USING DEFAULT SETTINGS");
#endif
    igh_settings_reset_system_to_default();
    igh_eeprom_update_water_flow_in_nv(0); // reset water flow to zero
    new_settings_available = 1;
  }
  else
  {
#ifdef IGH_DEBUG
    Serial.println("USING SETTINGS FROM MEMORY");
#endif
    igh_current_system_settings = settings_in_memory;
  }

  bool threshold_settings_read_successfully = igh_eeprom_read_threshold_settings(&thresholds_in_memory);
  uint8_t valid_threshold_checksum = igh_settings_calculate_checksum( &thresholds_in_memory, sizeof(thresholds_in_memory) );

  if( false == threshold_settings_read_successfully ||
      (thresholds_in_memory.checksum != valid_threshold_checksum) ||
      ( 0 == thresholds_in_memory.checksum) )
  {
    // if we can't get valid settings from memory at all, use default settings
#ifdef IGH_DEBUG
    Serial.println("USING DEFAULT THRESHOLDS");
#endif
    igh_settings_reset_system_to_default();
    new_settings_available = 1;
  }
  else
  {
#ifdef IGH_DEBUG
    Serial.println("USING THRESHOLDS FROM MEMORY");
#endif
    igh_current_threshold_settings = thresholds_in_memory;
  }
}

void igh_eeprom_check_to_clear( void )
{
  if( true == igh_button_pressed() )
  {
#ifdef IGH_DEBUG
    Serial.println("HOLD DOWN BUTTON FOR 5 MORE SECONDS TO CLEAR EEPROM");
#endif
  }
  unsigned long eeprom_reset_timer = millis();
  uint8_t eeprom_reset_seconds_counter = 0;
  while( true == igh_button_pressed() )
  {
    if( (millis() - eeprom_reset_timer) >= ONE_SECOND_MS )
    {
#ifdef IGH_DEBUG
      Serial.print(".");
#endif
      eeprom_reset_seconds_counter++;
      eeprom_reset_timer = millis();
    }
  }
  if( eeprom_reset_seconds_counter >= THREE_SECONDS )
  {
    EEPROM.clear();
#ifdef IGH_DEBUG
    Serial.println("\nEEPROM CLEARED\n");
#endif
  }
}

/*******************************************************************************
 * igh_eeprom_save_system_settings
 *******************************************************************************/
/**
 * \brief Save system settings  to eeprom emulator
 * \param settings_to_save: pointer to struct to save
 * \return true or false based on write success
 */
uint8_t igh_eeprom_save_system_settings(system_settings * settings_to_save)
{
  system_settings local_buffer;
  // save the data
  EEPROM.put(SYSTEM_SETTINGS_ADDRESS, *settings_to_save);
  // check if data is saved
  igh_eeprom_read_system_settings(&local_buffer);

  if( local_buffer.checksum != (settings_to_save->checksum))
  {
    return 0;
  }

  return 1;
}

/*******************************************************************************
 * igh_eeprom_save_threshold_settings
 *******************************************************************************/
/**
 * \brief Save threshold settings  to eeprom emulator
 * \param settings_to_save: pointer to struct to save
 * \return true or false based on write success
 */
uint8_t igh_eeprom_save_threshold_settings(thresholds * settings_to_save)
{
    thresholds local_buffer;
    // save the data
    EEPROM.put(SYSTEM_THRESHOLDS_ADDRESS, *settings_to_save);
    // check if data is saved
    igh_eeprom_read_threshold_settings(&local_buffer);

    if( local_buffer.checksum != (settings_to_save->checksum))
    {
        return 0;
    }

    return 1;
}

/*******************************************************************************
 * igh_eeprom_update_errors
 *******************************************************************************/
/**
 * \brief Save errors bit field
 * \param error_bit_field: error bit field
 * \return void
 */
void igh_eeprom_update_errors(uint32_t error_bit_field)
{
    EEPROM.put(SYSTEM_ERRORS_ADDRESS, error_bit_field);
}



/*******************************************************************************
 * igh_eeprom_read_system_settings
 *******************************************************************************/
/**
 * \brief Gets system settings from eeprom emulator
 * \param running_system_settings_buffer: buffer to store settings in
 * \return true or false based on success of read
 */
uint8_t igh_eeprom_read_system_settings(system_settings * running_system_settings_buffer)
{
  system_settings local_buffer;
  memset(&local_buffer, 0, sizeof(local_buffer));
  EEPROM.get(SYSTEM_SETTINGS_ADDRESS, local_buffer);

  uint8_t checksum = igh_settings_calculate_checksum(&local_buffer, sizeof(local_buffer));

  if( checksum != local_buffer.checksum)
  {
    return 0;
  }

  *running_system_settings_buffer = local_buffer;
  return 1;

}


/*******************************************************************************
 * igh_eeprom_read_threshold_settings
 *******************************************************************************/
/**
 * \brief Gets threshold settings from eeprom emulator
 * \param running_thresholds: buffer to store settings in
 * \return true or false based on success of read
 */
uint8_t igh_eeprom_read_threshold_settings(thresholds * running_thresholds)
{
  thresholds local_buffer;
  memset(&local_buffer, 0, sizeof(local_buffer));
  EEPROM.get(SYSTEM_THRESHOLDS_ADDRESS, local_buffer);

  uint8_t checksum = igh_settings_calculate_checksum(&local_buffer, sizeof(local_buffer));
  
  if( checksum != local_buffer.checksum)
  {
    return 0;
  }
  
  *running_thresholds = local_buffer;
  return 1;
}

/*******************************************************************************
 * igh_eeprom_get_errors
 *******************************************************************************/
/**
 * \brief Gets errors stored in eeprom
 * \param error_bit_field; uint32 bit fiel to populate
 * \return void
 */
void igh_eeprom_get_errors(uint32_t * error_bit_field)
{
    uint32_t _error = 0;
    EEPROM.get(SYSTEM_ERRORS_ADDRESS, _error);
    *error_bit_field = _error;
}

/*******************************************************************************
 * igh_eeproom_test
 *******************************************************************************/
/**
 * \brief Tests that we can read and write to the EEPROM just fine
 * \param void
 * \return true or false
 */
uint8_t igh_eeproom_test(void)
{
    thresholds test_thresholds;
    system_settings test_system_settings;

    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    uint8_t test_broker[] = "test.broker.com";

    memset(&test_system_settings, 0, sizeof(test_system_settings));
    memset(&test_thresholds, 0, sizeof(test_thresholds));

    memcpy(test_system_settings.serial_number, test_shield_id, 12);
    memcpy(test_system_settings.broker, test_broker, sizeof(test_broker));
    test_system_settings.op_state = OP_INACTIVE;
    test_system_settings.reporting_interval = 0x11111111;
    test_system_settings.data_resolution = 0x11111111;
    test_system_settings.broker_port = 0x1111;
    test_system_settings.checksum = igh_settings_calculate_checksum(&test_system_settings, sizeof(test_system_settings));

    test_thresholds.soil_moisture_low = 0x1111;
    test_thresholds.soil_moisture_high = 0x1111;
    test_thresholds.air_humidity_low = 0x1111;
    test_thresholds.air_humidity_high = 0x1111;
    test_thresholds.soil_humidity_low = 0x1111;
    test_thresholds.soil_humidity_high = 0x1111;
    test_thresholds.carbon_dioxide_low = 0x1111;
    test_thresholds.carbon_dioxide_high = 0x1111;
    test_thresholds.air_temperature_low = 0x1111;
    test_thresholds.air_temperature_high = 0x1111;
    test_thresholds.soil_temperature_low = 0x1111;
    test_thresholds.soil_temperature_high = 0x1111;
    test_thresholds.soil_npk_low = 0x1111;
    test_thresholds.soil_npk_high = 0x1111;
    test_thresholds.light_intensity_high = 0x1111;
    test_thresholds.light_intensity_low = 0x1111;
    test_thresholds.shield_battery_level_low = 0x1111;
    test_thresholds.shield_battery_level_high = 0x1111;
    test_thresholds.spear_battery_level_low = 0x1111;
    test_thresholds.spear_battery_level_high = 0x1111;
    test_thresholds.water_dispensed_period_high = 0x11111111;
    test_thresholds.water_dispensed_period_low = 0x11111111;
    test_thresholds.checksum = igh_settings_calculate_checksum(&test_thresholds, sizeof(test_thresholds));

    uint32_t test_error = 0xAAAAAAAA;
    uint32_t ret_test_error = 0;
    igh_eeprom_update_errors(test_error);
    igh_eeprom_get_errors(&ret_test_error);

    return igh_eeprom_save_system_settings(&test_system_settings)
           && igh_eeprom_save_threshold_settings(&test_thresholds)
           && (ret_test_error == test_error);
}

void igh_eeprom_update_water_flow_in_nv( float nv_water )
{
  EEPROM.put(SYSTEM_NV_WATER_STORE_ADDRESS, nv_water);
}

float igh_eeprom_get_water_flow_in_nv( void )
{
  float nv_water = 0;
  EEPROM.get(SYSTEM_NV_WATER_STORE_ADDRESS, nv_water);
  return nv_water;
}

uint32_t igh_eeprom_get_time_in_nv( void )
{
  uint32_t time_in_nv;
  EEPROM.get(SYSTEM_NV_DAY, time_in_nv);
  return time_in_nv;
}

void igh_eeprom_update_time_in_nv( uint32_t new_unix_time )
{
  EEPROM.put(SYSTEM_NV_DAY, new_unix_time);
}