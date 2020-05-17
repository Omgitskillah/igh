#include "unity.h"

#include "string.h" 
#include "igh_message.h"

extern uint8_t igh_msg_buffer[];

void setUp(void)
{
}

void tearDown(void)
{
}

void test_igh_message_reset_buffer_clears_buffer_and_moves_tracker_appropriately()
{
    memset(igh_msg_buffer,'A',MESSAGE_SIZE);

    uint8_t tracker = 0;
    tracker = igh_message_reset_buffer();

    TEST_ASSERT(igh_msg_buffer[0] == FRAME_START);
    TEST_ASSERT(tracker == 17);
    TEST_ASSERT(igh_msg_buffer[13] == '\0'); // test random bytes that they are cleared
    TEST_ASSERT(igh_msg_buffer[65] == '\0');
}