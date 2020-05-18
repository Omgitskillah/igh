#include "unity.h"

#include "string.h" 
#include "igh_message.h"

extern uint8_t igh_msg_buffer[];
extern uint8_t igh_msg_buffer_tracker;

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

