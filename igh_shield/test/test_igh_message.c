#include "unity.h"

#include "string.h" 
#include "igh_message.h"

extern uint8_t igh_msg_buffer[];
extern uint8_t igh_msg_buffer_tracker;
extern uint8_t igh_message_id;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_igh_message_reset_buffer_clears_buffer_and_moves_tracker_appropriately(void)
{
    memset(igh_msg_buffer,'A',MESSAGE_SIZE);

    uint8_t local_buffer_tracker = 0;
    local_buffer_tracker = igh_message_reset_buffer();

    TEST_ASSERT(igh_msg_buffer[0] == FRAME_START);
    TEST_ASSERT(local_buffer_tracker == 17);
    TEST_ASSERT(igh_msg_buffer[13] == '\0'); // test random bytes that they are cleared
    TEST_ASSERT(igh_msg_buffer[65] == '\0');
}

void test_igh_message_add_frame_end_adds_last_byte(void)
{
    igh_msg_buffer_tracker = 186; // random position where last byte should be
    uint8_t local_buffer_tracker = igh_message_add_frame_end();

    TEST_ASSERT(igh_msg_buffer[igh_msg_buffer_tracker] == FRAME_END);
}

void test_igh_message_add_length_appends_length_at_right_index_when_message_is_valid(void)
{
    igh_msg_buffer_tracker = 139;

    igh_msg_buffer[139] = FRAME_END;
    uint8_t local_buffer_tracker = igh_message_add_length();

    TEST_ASSERT(local_buffer_tracker == igh_msg_buffer_tracker);
    TEST_ASSERT(igh_msg_buffer[1] == igh_msg_buffer_tracker);
}

void test_igh_message_add_length_resets_the_buffer_when_message_is_not_valid(void)
{
    igh_msg_buffer_tracker = 139;

    igh_msg_buffer[139] = '\0';
    uint8_t local_buffer_tracker = igh_message_add_length();

    TEST_ASSERT(local_buffer_tracker == 17);  
    TEST_ASSERT(igh_msg_buffer[1] == '\0');
}

void test_igh_message_add_msg_type_adds_only_known_msg_types_to_buffer(void)
{
    igh_msg_type msg_type;

    msg_type = igh_message_add_msg_type(SENSOR_DATA);
    TEST_ASSERT(msg_type == SENSOR_DATA);
    TEST_ASSERT(igh_msg_buffer[2] == SENSOR_DATA);

    msg_type = igh_message_add_msg_type(SETTINGS_MSG);
    TEST_ASSERT(msg_type == SETTINGS_MSG);
    TEST_ASSERT(igh_msg_buffer[2] == SETTINGS_MSG);

    msg_type = igh_message_add_msg_type(ERROR_MSG);
    TEST_ASSERT(msg_type == ERROR_MSG);
    TEST_ASSERT(igh_msg_buffer[2] == ERROR_MSG);

    msg_type = igh_message_add_msg_type(MSG_ACK);
    TEST_ASSERT(msg_type == MSG_ACK);
    TEST_ASSERT(igh_msg_buffer[2] == MSG_ACK);

    msg_type = igh_message_add_msg_type(0x28); // should return unknown for anything else
    TEST_ASSERT(msg_type == UNKNOWN_MSG);
    TEST_ASSERT(igh_msg_buffer[2] == UNKNOWN_MSG);
}

void test_igh_message_add_direction_adds_right_message_direction(void)
{
    igh_msg_dir _dir;

    _dir = igh_message_add_direction(IGH_DOWNLOAD);
    TEST_ASSERT(_dir == IGH_DOWNLOAD);
    TEST_ASSERT(igh_msg_buffer[3] == IGH_DOWNLOAD);

    _dir = igh_message_add_direction(IGH_UPLOAD);
    TEST_ASSERT(_dir == IGH_UPLOAD);
    TEST_ASSERT(igh_msg_buffer[3] == IGH_UPLOAD);
}


void test_igh_message_add_serial_number_adds_valid_serial_number_to_msg_buffer(void)
{
    uint8_t expected_serial_number[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};

    uint8_t local_buffer_tracker = igh_message_add_serial_number(expected_serial_number);

    uint8_t actual_serial_number[12];
    memcpy(actual_serial_number, &igh_msg_buffer[4], 12); // copy 12 bytes of the serial number from msg buffer

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_serial_number, actual_serial_number, 12);
}

void test_igh_message_add_msg_id_adds_current_msg_id_and_increments_value(void)
{
    igh_message_id = 49;

    uint8_t next_msg_id = igh_message_add_msg_id();

    TEST_ASSERT(igh_msg_buffer[16] == 49);
    TEST_ASSERT(next_msg_id == (49+1));
}

void test_igh_message_check_tuple_fits_returns_false_or_true_appropriately(void)
{
    igh_msg_buffer_tracker = 54;
    uint8_t _ret = igh_message_check_tuple_fits(4); // total message will be 6 bytes, hence 60 is still within range

    TEST_ASSERT_TRUE(_ret);

    igh_msg_buffer_tracker = 252;
    _ret = igh_message_check_tuple_fits(4); // total bytes will be 6 hennce 258 will be out of range
    TEST_ASSERT_FALSE(_ret);
}

