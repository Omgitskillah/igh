#include "unity.h"

#include "string.h" 
#include "igh_settings.h"
#include "igh_default_settings.h"
#include "igh_message.h"

uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};

void setUp(void)
{
    current_valve_position = VALVE_CLOSE;

    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    igh_current_system_settings.op_state = OP_INACTIVE;
    igh_current_system_settings.reporting_interval = 0x11111111;
    igh_current_system_settings.data_resolution = 0x11111111;

    igh_current_threshold_settings.soil_moisture_low = 0x1111;
    igh_current_threshold_settings.soil_moisture_high = 0x1111;
    igh_current_threshold_settings.air_humidity_low = 0x1111;
    igh_current_threshold_settings.air_humidity_high = 0x1111;
    igh_current_threshold_settings.soil_humidity_low = 0x1111;
    igh_current_threshold_settings.soil_humidity_high = 0x1111;
    igh_current_threshold_settings.carbon_dioxide_low = 0x1111;
    igh_current_threshold_settings.carbon_dioxide_high = 0x1111;
    igh_current_threshold_settings.air_temperature_low = 0x1111;
    igh_current_threshold_settings.air_temperature_high = 0x1111;
    igh_current_threshold_settings.soil_temperature_low = 0x1111;
    igh_current_threshold_settings.soil_temperature_high = 0x1111;
    igh_current_threshold_settings.soil_npk_low = 0x1111;
    igh_current_threshold_settings.soil_npk_high = 0x1111;
    igh_current_threshold_settings.light_intensity_high = 0x1111;
    igh_current_threshold_settings.light_intensity_low = 0x1111;
    igh_current_threshold_settings.shield_battery_level_low = 0x1111;
    igh_current_threshold_settings.shield_battery_level_high = 0x1111;
    igh_current_threshold_settings.spear_battery_level_low = 0x1111;
    igh_current_threshold_settings.spear_battery_level_high = 0x1111;
    igh_current_threshold_settings.water_dispensed_period_high = 0x11111111;
    igh_current_threshold_settings.water_dispensed_period_low = 0x11111111;
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

void test_igh_settings_parse_new_settings_does_nothing_if_total_settings_length_is_invalid(void)
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
        IGH_SEND_SETTINGS,0x00, // settings with invalid length
        SUBID_OPSTATE,0x01,0x01, // new op state setting 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
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
        SUBID_OPSTATE,0x01,0x01, // new op state setting 3
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
        SUBID_OPSTATE,0x01,0x01, // new op state setting 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x12, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
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
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to Standard mode
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
        SUBID_OPSTATE,0x01,OP_PREMIUM, // new op state settings change to premium
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
        SUBID_OPSTATE,0x01,OP_BASIC, // new op state settings change to basic
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
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x19, // length
        SUBID_OPSTATE,0x01,OP_INACTIVE, // new op state settings change to inactive
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

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
        SUBID_OPSTATE,0x01,0xFF, // new op state settings change to unknown
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
        SUBID_OPSTATE,0x03,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT(igh_current_system_settings.op_state == OP_INACTIVE);
}

void test_igh_settings_parse_new_settings_updates_the_reporting_interval_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x1F, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x23334498, igh_current_system_settings.reporting_interval);
}

void test_igh_settings_parse_new_settings_does_not_update_the_reporting_interval_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x1F, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x02, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x11111111, igh_current_system_settings.reporting_interval);
}

void test_igh_settings_parse_new_settings_returns_zero_if_any_setting_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x1F, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state 
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x02, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
}

void test_igh_settings_parse_new_settings_updates_the_data_resolution_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x1F, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x23334498, igh_current_system_settings.data_resolution);
}

void test_igh_settings_parse_new_settings_does_not_update_the_data_resolution_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x1F, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x02, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x11111111, igh_current_system_settings.data_resolution);
}

void test_igh_settings_parse_new_settings_updates_the_soil_moisture_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x29, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.soil_moisture_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_moisture_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x29, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x04, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_moisture_low);
}

void test_igh_settings_parse_new_settings_updates_the_soil_moisture_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x2D, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.soil_moisture_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_moisture_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x2D, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x04, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_moisture_high);
}

void test_igh_settings_parse_new_settings_updates_the_air_humidity_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.air_humidity_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_air_humidity_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x04, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.air_humidity_low);
}

void test_igh_settings_parse_new_settings_updates_the_air_humidity_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.air_humidity_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_air_humidity_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x04, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.air_humidity_high);
}

void test_igh_settings_parse_new_settings_updates_the_soil_humidity_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.soil_humidity_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_humidity_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_HUMIDITY_LOW, 0x04, 0x29, 0x92, // air humidity wrong length
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_humidity_low);
}

void test_igh_settings_parse_new_settings_updates_the_soil_humidity_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x2992, igh_current_threshold_settings.soil_humidity_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_humidity_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x04, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_humidity_high);
}

void test_igh_settings_parse_new_settings_updates_the_carbon_dioxide_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_LOW, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.carbon_dioxide_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_carbon_dioxide_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_LOW, 0x04, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.carbon_dioxide_low);
}

void test_igh_settings_parse_new_settings_updates_the_carbon_dioxide_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.carbon_dioxide_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_carbon_dioxide_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_CARBON_DIOXIDE_HIGH, 0x04, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.carbon_dioxide_high);
}

void test_igh_settings_parse_new_settings_updates_the_air_temperature_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_AIR_TEMPERATURE_LOW, 0x02, 0x30, 0x54, //air temperature
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.air_temperature_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_air_temperature_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_TEMPERATURE_LOW, 0x04, 0x30, 0x54, // air temperature
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.air_temperature_low);
}

void test_igh_settings_parse_new_settings_updates_the_air_temperature_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_AIR_TEMPERATURE_HIGH, 0x02, 0x30, 0x54, // air temperature
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.air_temperature_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_air_temperature_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_TEMPERATURE_HIGH, 0x04, 0x30, 0x54, // co2 settings 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.air_temperature_high);
}

void test_igh_settings_parse_new_settings_updates_the_soil_temperature_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_NPK_LOW, 0x02, 0x12, 0x32,
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.soil_temperature_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_temperature_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_TEMPERATURE_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_temperature_low);
}

void test_igh_settings_parse_new_settings_updates_the_soil_temperature_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SOIL_TEMPERATURE_HIGH, 0x02, 0x30, 0x54,
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.soil_temperature_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_temperature_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SOIL_TEMPERATURE_HIGH, 0x04, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_temperature_high);
}

void test_igh_settings_parse_new_settings_updates_the_soil_npk_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SOIL_NPK_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.soil_npk_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_npk_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_NPK_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_npk_low);
}

void test_igh_settings_parse_new_settings_updates_the_soil_npk_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SOIL_NPK_HIGH, 0x02, 0x30, 0x54,
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.soil_npk_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_soil_npk_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SOIL_NPK_HIGH, 0x04, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.soil_npk_high);
}

void test_igh_settings_parse_new_settings_updates_the_light_intensoty_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_LIGHT_INTENSITY_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.light_intensity_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_light_intensity_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_LIGHT_INTENSITY_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.light_intensity_low);
}

void test_igh_settings_parse_new_settings_updates_the_light_intensity_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_LIGHT_INTENSITY_HIGH, 0x02, 0x30, 0x54,
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.light_intensity_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_light_intenisty_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_LIGHT_INTENSITY_HIGH, 0x04, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.light_intensity_high);
}

void test_igh_settings_parse_new_settings_updates_the_shield_battery_level_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SHIELD_BATTERY_LEVEL_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.shield_battery_level_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_shield_battery_level_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SHIELD_BATTERY_LEVEL_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.shield_battery_level_low);
}

void test_igh_settings_parse_new_settings_updates_the_shield_battery_level_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SHIELD_BATTERY_LEVEL_HIGH, 0x02, 0x30, 0x54,
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.shield_battery_level_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_shield_battery_level_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SHIELD_BATTERY_LEVEL_HIGH, 0x04, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.shield_battery_level_high);
}

void test_igh_settings_parse_new_settings_updates_the_spear_battery_level_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.spear_battery_level_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_spear_battery_level_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.spear_battery_level_low);
}

void test_igh_settings_parse_new_settings_updates_the_spear_battery_level_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 6
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SPEAR_BATTERY_LEVEL_HIGH, 0x02, 0x30, 0x54,
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold 4
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x3054, igh_current_threshold_settings.spear_battery_level_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_spear_battery_level_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x35, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_DATA_RESOLUTION, 0x04, 0x23, 0x33, 0x44, 0x98, // new data resolution 
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, // soil moisture high threshold 4
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, // air humidity
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity 
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x29, 0x92, // air humidity wrong length
        SUBID_SPEAR_BATTERY_LEVEL_HIGH, 0x04, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, // soil moisture low threshold
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x1111, igh_current_threshold_settings.spear_battery_level_high);
}

void test_igh_settings_parse_new_settings_updates_the_water_dispensed_period_low_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_WATER_DISPENSED_PERIOD_LOW, 0x04, 0x23, 0x33, 0x44, 0x98,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x23334498, igh_current_threshold_settings.water_dispensed_period_low);
}

void test_igh_settings_parse_new_settings_does_not_update_the_water_dispensed_period_low_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_WATER_DISPENSED_PERIOD_LOW, 0x12, 0x23, 0x33, 0x44, 0x98,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x11111111, igh_current_threshold_settings.water_dispensed_period_low);
}

void test_igh_settings_parse_new_settings_updates_the_water_dispensed_period_high_th_if_length_is_valid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length 3
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x12, 0x32, // temp threshold 4
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent 6
        SUBID_WATER_DISPENSED_PERIOD_HIGH, 0x04, 0x23, 0x33, 0x44, 0x98,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the write length 14
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x02, 0x30, 0x54, 
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);
    TEST_ASSERT_TRUE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32( 0x23334498, igh_current_threshold_settings.water_dispensed_period_high);
}

void test_igh_settings_parse_new_settings_does_not_update_the_water_dispensed_period_high_th_if_length_is_invalid(void)
{
    // given a valid settings message that includes a setting to change the serial number
    uint8_t test_message[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_SEND_SETTINGS,0x31, // length
        SUBID_OPSTATE,0x01,OP_STANDARD, // new op state settings change to standard with invalid length
        SUBID_REPORTING_INTERVAL, 0x04, 0x23, 0x33, 0x44, 0x98, // reporting interval uint32 equivalent
        SUBID_WATER_DISPENSED_PERIOD_HIGH, 0x12, 0x23, 0x33, 0x44, 0x98,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // new serial number but with the wrong length
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x29, 0x92, 
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x04, 0x30, 0x54, 
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x29, 0x92, 
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x29, 0x92, 
        0x3E // end 
    };

    test_message[1] = sizeof(test_message); // update the length

    uint8_t ret = igh_settings_parse_new_settings(test_message);

    TEST_ASSERT_FALSE(ret);
    // check new serial number
    TEST_ASSERT_EQUAL_UINT32(0x11111111, igh_current_threshold_settings.water_dispensed_period_high);
}

void test_igh_settings_reset_system__to_default_resets_all_settings_to_defalult(void)
{
    igh_settings_reset_system_to_default();

    // test that all settings are set to the defaults
    TEST_ASSERT_EQUAL_UINT8_ARRAY(igh_current_system_settings.serial_number, default_serial_number, 12);
    TEST_ASSERT(igh_current_system_settings.op_state                            == DEFAULT_NEW_OPSTATE);
    TEST_ASSERT(igh_current_system_settings.reporting_interval                  == DEFAULT_REPORTING_INTERVAL);
    TEST_ASSERT(igh_current_system_settings.data_resolution                     == DEFAULT_DATA_RESOLUTION);
    //High Threshold tirggers
    TEST_ASSERT(igh_current_threshold_settings.soil_moisture_low                == DEFAULT_SOIL_MOISTURE_LOW);                   
    TEST_ASSERT(igh_current_threshold_settings.air_humidity_low                 == DEFAULT_AIR_HUMIDITY_LOW);                     
    TEST_ASSERT(igh_current_threshold_settings.soil_humidity_low                == DEFAULT_SOIL_HUMIDITY_LOW);                   
    TEST_ASSERT(igh_current_threshold_settings.carbon_dioxide_low               == DEFAULT_CARBON_DIOXIDE_LOW);        
    TEST_ASSERT(igh_current_threshold_settings.air_temperature_low              == DEFAULT_AIR_TEMPERATURE_LOW);               
    TEST_ASSERT(igh_current_threshold_settings.soil_temperature_low             == DEFAULT_SOIL_TEMPERATURE_LOW);             
    TEST_ASSERT(igh_current_threshold_settings.soil_npk_low                     == DEFAULT_SOIL_NPK_LOW);                             
    TEST_ASSERT(igh_current_threshold_settings.light_intensity_low              == DEFAULT_LIGHT_INTENSITY_LOW);               
    TEST_ASSERT(igh_current_threshold_settings.shield_battery_level_low         == DEFAULT_SHIELD_BATTERY_LEVEL_LOW);     
    TEST_ASSERT(igh_current_threshold_settings.spear_battery_level_low          == DEFAULT_SPEAR_BATTERY_LEVEL_LOW);      
    TEST_ASSERT(igh_current_threshold_settings.water_dispensed_period_low       == DEFAULT_WATER_DISPENSED_PERIOD_LOW);
    // Low Threshold Trigger
    TEST_ASSERT(igh_current_threshold_settings.soil_moisture_high               == DEFAULT_SOIL_MOISTURE_HIGH);                 
    TEST_ASSERT(igh_current_threshold_settings.air_humidity_high                == DEFAULT_AIR_HUMIDITY_HIGH);                   
    TEST_ASSERT(igh_current_threshold_settings.soil_humidity_high               == DEFAULT_SOIL_HUMIDITY_HIGH);                 
    TEST_ASSERT(igh_current_threshold_settings.carbon_dioxide_high              == DEFAULT_CARBON_DIOXIDE_HIGH);        
    TEST_ASSERT(igh_current_threshold_settings.air_temperature_high             == DEFAULT_AIR_TEMPERATURE_HIGH);             
    TEST_ASSERT(igh_current_threshold_settings.soil_temperature_high            == DEFAULT_SOIL_TEMPERATURE_HIGH);           
    TEST_ASSERT(igh_current_threshold_settings.soil_npk_high                    == DEFAULT_SOIL_NPK_HIGH);                           
    TEST_ASSERT(igh_current_threshold_settings.light_intensity_high             == DEFAULT_LIGHT_INTENSITY_HIGH);             
    TEST_ASSERT(igh_current_threshold_settings.shield_battery_level_high        == DEFAULT_SHIELD_BATTERY_LEVEL_HIGH);   
    TEST_ASSERT(igh_current_threshold_settings.spear_battery_level_high         == DEFAULT_SPEAR_BATTERY_LEVEL_HIGH);     
    TEST_ASSERT(igh_current_threshold_settings.water_dispensed_period_high      == DEFAULT_WATER_DISPENSED_PERIOD_HIGH); 
}

void test_igh_settings_build_settings_request_payload_returns_zero_if_if_settings_request_length_is_zero(void)
{
    uint8_t settings_request[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_READ_SETTINGS,0x00, // wrong length, 0x1A is the right length
        SUBID_OPSTATE,
        SUBID_REPORTING_INTERVAL,
        SUBID_DATA_RESOLUTION,
        SUBID_SET_SERIAL_NUMBER,
        
        //High Threshold tirggers
        SUBID_SOIL_MOISTURE_LOW,          
        SUBID_AIR_HUMIDITY_LOW,           
        SUBID_SOIL_HUMIDITY_LOW,               
        SUBID_CARBON_DIOXIDE_LOW,
        SUBID_AIR_TEMPERATURE_LOW,        
        SUBID_SOIL_TEMPERATURE_LOW,       
        SUBID_SOIL_NPK_LOW,               
        SUBID_LIGHT_INTENSITY_LOW,        
        SUBID_SHIELD_BATTERY_LEVEL_LOW,   
        SUBID_SPEAR_BATTERY_LEVEL_LOW,   
        SUBID_WATER_DISPENSED_PERIOD_LOW,

        // Low Threshold Trigger
        SUBID_SOIL_MOISTURE_HIGH,          
        SUBID_AIR_HUMIDITY_HIGH,           
        SUBID_SOIL_HUMIDITY_HIGH,                
        SUBID_CARBON_DIOXIDE_HIGH,
        SUBID_AIR_TEMPERATURE_HIGH,        
        SUBID_SOIL_TEMPERATURE_HIGH,       
        SUBID_SOIL_NPK_HIGH,               
        SUBID_LIGHT_INTENSITY_HIGH,        
        SUBID_SHIELD_BATTERY_LEVEL_HIGH,   
        SUBID_SPEAR_BATTERY_LEVEL_HIGH,             
        SUBID_WATER_DISPENSED_PERIOD_HIGH,
        0x3E // end 
    };
    settings_request[1] = sizeof(settings_request); // update the length

    uint8_t buffer[256];
    uint8_t start_index = 20;
    uint8_t ret = igh_settings_build_settings_request_payload(settings_request, buffer, start_index);

    TEST_ASSERT_EQUAL_UINT8(start_index, ret);
}

void test_igh_settings_build_settings_request_payload_populates_buffer_with_system_settings_when_requested(void)
{
    uint8_t settings_request[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_READ_SETTINGS,0x04, 
        SUBID_OPSTATE, //3
        SUBID_REPORTING_INTERVAL, //6
        SUBID_DATA_RESOLUTION, //6
        SUBID_SET_SERIAL_NUMBER, //14
        
        // //High Threshold tirggers
        // SUBID_SOIL_MOISTURE_LOW,          
        // SUBID_AIR_HUMIDITY_LOW,           
        // SUBID_SOIL_HUMIDITY_LOW,               
        // SUBID_CARBON_DIOXIDE_LOW,
        // SUBID_AIR_TEMPERATURE_LOW,        
        // SUBID_SOIL_TEMPERATURE_LOW,       
        // SUBID_SOIL_NPK_LOW,               
        // SUBID_LIGHT_INTENSITY_LOW,        
        // SUBID_SHIELD_BATTERY_LEVEL_LOW,   
        // SUBID_SPEAR_BATTERY_LEVEL_LOW,   
        // SUBID_WATER_DISPENSED_PERIOD_LOW,

        // // Low Threshold Trigger
        // SUBID_SOIL_MOISTURE_HIGH,          
        // SUBID_AIR_HUMIDITY_HIGH,           
        // SUBID_SOIL_HUMIDITY_HIGH,                
        // SUBID_CARBON_DIOXIDE_HIGH,
        // SUBID_AIR_TEMPERATURE_HIGH,        
        // SUBID_SOIL_TEMPERATURE_HIGH,       
        // SUBID_SOIL_NPK_HIGH,               
        // SUBID_LIGHT_INTENSITY_HIGH,        
        // SUBID_SHIELD_BATTERY_LEVEL_HIGH,   
        // SUBID_SPEAR_BATTERY_LEVEL_HIGH,             
        // SUBID_WATER_DISPENSED_PERIOD_HIGH,
        0x3E // end 
    };
    settings_request[1] = sizeof(settings_request); // update the length

    uint8_t buffer[256];
    uint8_t start_index = 20;
    uint8_t ret = igh_settings_build_settings_request_payload(settings_request, buffer, start_index);

    uint8_t expected_conntents[] = {
        IGH_READ_SETTINGS, 0x00,
        SUBID_OPSTATE, 0x01, OP_INACTIVE,
        SUBID_REPORTING_INTERVAL, 0x04, 0x11, 0x11, 0x11, 0x11,
        SUBID_DATA_RESOLUTION, 0x04, 0x11, 0x11, 0x11, 0x11,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37   
    };
    expected_conntents[1] = sizeof(expected_conntents) - 2; // minus two to remove the header

    TEST_ASSERT_EQUAL_UINT8((sizeof(expected_conntents) + start_index) , ret ); // the sum of all the bytes expected
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_conntents, &buffer[start_index], sizeof(expected_conntents));
}

void test_igh_settings_build_settings_request_payload_populates_buffer_with_threshold_settings_when_requested(void)
{
    uint8_t settings_request[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        IGH_READ_SETTINGS,0x1A, 
        SUBID_OPSTATE, //3
        SUBID_REPORTING_INTERVAL, //6
        SUBID_DATA_RESOLUTION, //6
        SUBID_SET_SERIAL_NUMBER, //14
        
        //High Threshold tirggers
        SUBID_SOIL_MOISTURE_LOW,          
        SUBID_AIR_HUMIDITY_LOW,           
        SUBID_SOIL_HUMIDITY_LOW,               
        SUBID_CARBON_DIOXIDE_LOW,
        SUBID_AIR_TEMPERATURE_LOW,        
        SUBID_SOIL_TEMPERATURE_LOW,       
        SUBID_SOIL_NPK_LOW,               
        SUBID_LIGHT_INTENSITY_LOW,        
        SUBID_SHIELD_BATTERY_LEVEL_LOW,   
        SUBID_SPEAR_BATTERY_LEVEL_LOW,   
        SUBID_WATER_DISPENSED_PERIOD_LOW,

        // Low Threshold Trigger
        SUBID_SOIL_MOISTURE_HIGH,          
        SUBID_AIR_HUMIDITY_HIGH,           
        SUBID_SOIL_HUMIDITY_HIGH,                
        SUBID_CARBON_DIOXIDE_HIGH,
        SUBID_AIR_TEMPERATURE_HIGH,        
        SUBID_SOIL_TEMPERATURE_HIGH,       
        SUBID_SOIL_NPK_HIGH,               
        SUBID_LIGHT_INTENSITY_HIGH,        
        SUBID_SHIELD_BATTERY_LEVEL_HIGH,   
        SUBID_SPEAR_BATTERY_LEVEL_HIGH,             
        SUBID_WATER_DISPENSED_PERIOD_HIGH,
        0x3E // end 
    };
    settings_request[1] = sizeof(settings_request); // update the length

    uint8_t buffer[256];
    uint8_t start_index = 20;
    uint8_t ret = igh_settings_build_settings_request_payload(settings_request, buffer, start_index);

    uint8_t expected_conntents[] = {
        IGH_READ_SETTINGS, 0x00,
        SUBID_OPSTATE, 0x01, OP_INACTIVE,
        SUBID_REPORTING_INTERVAL, 0x04, 0x11, 0x11, 0x11, 0x11,
        SUBID_DATA_RESOLUTION, 0x04, 0x11, 0x11, 0x11, 0x11,
        SUBID_SET_SERIAL_NUMBER, 0x0C, 0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,  
        //High Threshold tirggers
        SUBID_SOIL_MOISTURE_LOW, 0x02, 0x11, 0x11,          
        SUBID_AIR_HUMIDITY_LOW, 0x02, 0x11, 0x11,           
        SUBID_SOIL_HUMIDITY_LOW, 0x02, 0x11, 0x11,               
        SUBID_CARBON_DIOXIDE_LOW, 0x02, 0x11, 0x11,
        SUBID_AIR_TEMPERATURE_LOW, 0x02, 0x11, 0x11,        
        SUBID_SOIL_TEMPERATURE_LOW, 0x02, 0x11, 0x11,       
        SUBID_SOIL_NPK_LOW, 0x02, 0x11, 0x11,               
        SUBID_LIGHT_INTENSITY_LOW, 0x02, 0x11, 0x11,        
        SUBID_SHIELD_BATTERY_LEVEL_LOW, 0x02, 0x11, 0x11,   
        SUBID_SPEAR_BATTERY_LEVEL_LOW, 0x02, 0x11, 0x11,   
        SUBID_WATER_DISPENSED_PERIOD_LOW, 0x04, 0x11, 0x11, 0x11, 0x11,
        // Low Threshold Trigger
        SUBID_SOIL_MOISTURE_HIGH, 0x02, 0x11, 0x11,          
        SUBID_AIR_HUMIDITY_HIGH, 0x02, 0x11, 0x11,           
        SUBID_SOIL_HUMIDITY_HIGH, 0x02, 0x11, 0x11,                
        SUBID_CARBON_DIOXIDE_HIGH, 0x02, 0x11, 0x11,
        SUBID_AIR_TEMPERATURE_HIGH, 0x02, 0x11, 0x11,        
        SUBID_SOIL_TEMPERATURE_HIGH, 0x02, 0x11, 0x11,       
        SUBID_SOIL_NPK_HIGH, 0x02, 0x11, 0x11,               
        SUBID_LIGHT_INTENSITY_HIGH, 0x02, 0x11, 0x11,        
        SUBID_SHIELD_BATTERY_LEVEL_HIGH, 0x02, 0x11, 0x11,   
        SUBID_SPEAR_BATTERY_LEVEL_HIGH, 0x02, 0x11, 0x11,             
        SUBID_WATER_DISPENSED_PERIOD_HIGH, 0x04, 0x11, 0x11, 0x11, 0x11
    };
    expected_conntents[1] = sizeof(expected_conntents) - 2; // minus two to remove the header

    TEST_ASSERT_EQUAL_UINT8((sizeof(expected_conntents) + start_index) , ret ); // the sum of all the bytes expected
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_conntents, &buffer[start_index], sizeof(expected_conntents));
}

void test_igh_settings_remote_valvle_control_opens_the_valve_if_valid(void)
{
    uint8_t valve_position_change[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        VALVE_POSITION, 0x01, VALVE_OPEN, // open the valve
        0x3E // end 
    };
    valve_position_change[1] = sizeof(valve_position_change); // update the length

    uint8_t ret = igh_settings_remote_valvle_control(valve_position_change);

    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(VALVE_OPEN, current_valve_position);
}

void test_igh_settings_remote_valvle_control_closes_the_valve_if_valid(void)
{
    current_valve_position = VALVE_OPEN;

    uint8_t valve_position_change[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        VALVE_POSITION, 0x01, VALVE_CLOSE, // open the valve
        0x3E // end 
    };
    valve_position_change[1] = sizeof(valve_position_change); // update the length

    uint8_t ret = igh_settings_remote_valvle_control(valve_position_change);

    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_UINT8(VALVE_CLOSE, current_valve_position);
}

void test_igh_settings_remote_valvle_control_does_nothing_if_valve_state_is_invalid(void)
{   
    uint8_t valve_position_change[] =
    {
        0x3C, // start
        0x00, // length
        SETTINGS_MSG,
        IGH_DOWNLOAD,
        0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37,// serial nuber 
        0x35, // message id
        VALVE_POSITION, 0x01, 0x20, // open the valve
        0x3E // end 
    };
    valve_position_change[1] = sizeof(valve_position_change); // update the length

    uint8_t ret = igh_settings_remote_valvle_control(valve_position_change);

    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_UINT8(VALVE_CLOSE, current_valve_position);
}

void test_igh_settings_calculate_checksum_returns_the_right_checksum_for_system_settings(void)
{
    //uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    system_settings test_system_settings;

    memcpy(test_system_settings.serial_number, test_shield_id, 12);
    test_system_settings.op_state = OP_PREMIUM;
    test_system_settings.reporting_interval = 0x12345678;
    test_system_settings.data_resolution = 0xABCDEF12;

    uint8_t expected_checksum = 0;
    uint8_t checksum = 0;

    // e0+0f+ce+68+9a+75+47+05+e7+9a+0e+37+0+12+34+56+78+AB+CD+EF+12
    int buffer = (((0xe0 + 0x0f + 0xce + 0x68 + 0x9a + 0x75 + 0x47 + 0x05 + 0xe7 + 0x9a + 0x0e + 0x37)
                        + OP_PREMIUM
                        + (0x12 + 0x34 + 0x56 + 0x78)
                        + (0xAB + 0xCD + 0xEF + 0x12)) % 256);
    // (1350 + 0 + 276 + 633) % 256 = 2259 % 256 = 211
    expected_checksum = (uint8_t)buffer;

    // checksum = igh_settings_calculate_system_settings_checksum(test_system_settings);
    checksum = igh_settings_calculate_checksum(&test_system_settings, sizeof(test_system_settings));
    TEST_ASSERT_EQUAL_UINT8(expected_checksum, checksum);
}

void test_igh_settings_calculate_checksum_returns_the_right_checksum_for_threshold_settings(void)
{
    thresholds test_thresholds_settings;

    uint8_t expected_checksum = 0;
    uint8_t checksum = 0;

    test_thresholds_settings.soil_moisture_low = 0x1234;
    test_thresholds_settings.soil_moisture_high = 0x1234;
    test_thresholds_settings.air_humidity_low = 0x1234;
    test_thresholds_settings.air_humidity_high = 0x1234;
    test_thresholds_settings.soil_humidity_low = 0x1234;
    test_thresholds_settings.soil_humidity_high = 0x1234;
    test_thresholds_settings.carbon_dioxide_low = 0x1234;
    test_thresholds_settings.carbon_dioxide_high = 0x1234;
    test_thresholds_settings.air_temperature_low = 0x1234;
    test_thresholds_settings.air_temperature_high = 0x1234;
    test_thresholds_settings.soil_temperature_low = 0x1234;
    test_thresholds_settings.soil_temperature_high = 0x1234;
    test_thresholds_settings.soil_npk_low = 0x1234;
    test_thresholds_settings.soil_npk_high = 0x1234;
    test_thresholds_settings.light_intensity_high = 0x1234;
    test_thresholds_settings.light_intensity_low = 0x1234;
    test_thresholds_settings.shield_battery_level_low = 0x1234;
    test_thresholds_settings.shield_battery_level_high = 0x1234;
    test_thresholds_settings.spear_battery_level_low = 0x1234;
    test_thresholds_settings.spear_battery_level_high = 0x1234;
    test_thresholds_settings.water_dispensed_period_high = 0x12345678;
    test_thresholds_settings.water_dispensed_period_low = 0x12345678;

    int buffer = (((0x12 + 0x34 + 0x56 + 0x78) * 2)
                 + ((0x12 + 0x34) * 20)) % 256;

    // ((276 * 2) + (70 * 20)) % 256 = (552 + 1400) % 256 = 160
    expected_checksum = (uint8_t)buffer;

    checksum = igh_settings_calculate_checksum(&test_thresholds_settings, sizeof(test_thresholds_settings));
    TEST_ASSERT_EQUAL_UINT32(buffer, checksum);
}


