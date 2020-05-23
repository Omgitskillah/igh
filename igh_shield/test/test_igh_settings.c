#include "unity.h"

#include "string.h" 
#include "igh_settings.h"
#include "igh_default_settings.h"
#include "igh_message.h"

extern thresholds igh_default_thresholds;
extern system_settings igh_default_system_settings;
extern system_settings igh_current_system_settings;

extern uint8_t default_serial_number[];

uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};

void setUp(void)
{
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    igh_current_system_settings.op_state = OP_INACTIVE;
}

void tearDown(void)
{
}

void test_igh_settings_get_defaults(void)
{
    igh_settings_get_defaults();
    // test that all settings are set to the defaults
    TEST_ASSERT_EQUAL_UINT8_ARRAY(igh_default_system_settings.serial_number, default_serial_number, 12);
    TEST_ASSERT(igh_default_system_settings.op_state                    == DEFAULT_NEW_OPSTATE);
    TEST_ASSERT(igh_default_system_settings.reporting_interval          == DEFAULT_REPORTING_INTERVAL);
    TEST_ASSERT(igh_default_system_settings.data_resolution             == DEFAULT_DATA_RESOLUTION);
    //High Threshold tirggers
    TEST_ASSERT(igh_default_thresholds.soil_moisture_low                == DEFAULT_SOIL_MOISTURE_LOW);                   
    TEST_ASSERT(igh_default_thresholds.air_humidity_low                 == DEFAULT_AIR_HUMIDITY_LOW);                     
    TEST_ASSERT(igh_default_thresholds.soil_humidity_low                == DEFAULT_SOIL_HUMIDITY_LOW);                   
    TEST_ASSERT(igh_default_thresholds.carbon_dioxide_low               == DEFAULT_CARBON_DIOXIDE_LOW);        
    TEST_ASSERT(igh_default_thresholds.air_temperature_low              == DEFAULT_AIR_TEMPERATURE_LOW);               
    TEST_ASSERT(igh_default_thresholds.soil_temperature_low             == DEFAULT_SOIL_TEMPERATURE_LOW);             
    TEST_ASSERT(igh_default_thresholds.soil_npk_low                     == DEFAULT_SOIL_NPK_LOW);                             
    TEST_ASSERT(igh_default_thresholds.light_intensity_low              == DEFAULT_LIGHT_INTENSITY_LOW);               
    TEST_ASSERT(igh_default_thresholds.shield_battery_level_low         == DEFAULT_SHIELD_BATTERY_LEVEL_LOW);     
    TEST_ASSERT(igh_default_thresholds.spear_battery_level_low          == DEFAULT_SPEAR_BATTERY_LEVEL_LOW);      
    TEST_ASSERT(igh_default_thresholds.water_dispensed_period_low       == DEFAULT_WATER_DISPENSED_PERIOD_LOW);
    // Low Threshold Trigger
    TEST_ASSERT(igh_default_thresholds.soil_moisture_high               == DEFAULT_SOIL_MOISTURE_HIGH);                 
    TEST_ASSERT(igh_default_thresholds.air_humidity_high                == DEFAULT_AIR_HUMIDITY_HIGH);                   
    TEST_ASSERT(igh_default_thresholds.soil_humidity_high               == DEFAULT_SOIL_HUMIDITY_HIGH);                 
    TEST_ASSERT(igh_default_thresholds.carbon_dioxide_high              == DEFAULT_CARBON_DIOXIDE_HIGH);        
    TEST_ASSERT(igh_default_thresholds.air_temperature_high             == DEFAULT_AIR_TEMPERATURE_HIGH);             
    TEST_ASSERT(igh_default_thresholds.soil_temperature_high            == DEFAULT_SOIL_TEMPERATURE_HIGH);           
    TEST_ASSERT(igh_default_thresholds.soil_npk_high                    == DEFAULT_SOIL_NPK_HIGH);                           
    TEST_ASSERT(igh_default_thresholds.light_intensity_high             == DEFAULT_LIGHT_INTENSITY_HIGH);             
    TEST_ASSERT(igh_default_thresholds.shield_battery_level_high        == DEFAULT_SHIELD_BATTERY_LEVEL_HIGH);   
    TEST_ASSERT(igh_default_thresholds.spear_battery_level_high         == DEFAULT_SPEAR_BATTERY_LEVEL_HIGH);     
    TEST_ASSERT(igh_default_thresholds.water_dispensed_period_high      == DEFAULT_WATER_DISPENSED_PERIOD_HIGH);    
}

void test_igh_settings_parse_new_settings_sets_serial_number_if_serial_number_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,0x01, // new op state setting 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    uint8_t expected_serail[12];
    memset(expected_serail, 0xAA, sizeof(expected_serail));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_serail, igh_current_system_settings.serial_number, 12);
}

void test_igh_settings_parse_new_settings_does_not_set_serial_number_if_serial_number_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,0x01, // new op state setting 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x12, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    uint8_t expected_serail[12];
    memcpy(expected_serail, test_shield_id, sizeof(expected_serail)); // expect that the serial number has not changed
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_serail, igh_current_system_settings.serial_number, 12);
}

void test_igh_settings_parse_new_settings_sets_the_op_state_to_standard_if_state_and_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,OP_STANDARD, // new op state settings change to Standard mode
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_STANDARD);
}

void test_igh_settings_parse_new_settings_sets_the_op_state_to_premium_if_state_and_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,OP_PREMIUM, // new op state settings change to premium
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_PREMIUM);
}

void test_igh_settings_parse_new_settings_sets_the_op_state_to_basic_if_state_and_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,OP_BASIC, // new op state settings change to basic
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_BASIC);
}

void test_igh_settings_parse_new_settings_sets_the_op_state_to_inactive_if_state_and_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,OP_INACTIVE, // new op state settings change to inactive
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_INACTIVE);
}

void test_igh_settings_parse_new_settings_does_nothing_to_op_state_if_op_state_is_unknown(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x01,0xFF, // new op state settings change to unknown
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_INACTIVE);
}

void test_igh_settings_parse_new_settings_does_nothing_to_op_state_if_op_state_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x2C, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_NEW_OPSTATE,0x03,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_INACTIVE);
}
