#include "unity.h"

#include "string.h" 
#include "igh_message.h"
#include "igh_settings.h"

extern uint8_t igh_cmd_buffer[MESSAGE_SIZE];
extern uint8_t igh_cmd_buffer_tracker;

extern uint8_t igh_msg_buffer[MESSAGE_SIZE];
extern uint8_t igh_msg_buffer_tracker;
extern uint8_t igh_message_id;
extern system_settings igh_current_system_settings;

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

void test_igh_message_add_tuple_adds_tuple_to_payload_if_tuple_fits(void)
{
    igh_msg_buffer_tracker = 54;

    igh_pkt_id local_pkt_id = SPEAR_ID;
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    uint8_t expected_tuple[14];
    expected_tuple[0] = local_pkt_id;
    expected_tuple[1] = 12;
    memcpy(&expected_tuple[2], test_shield_id, 12);

    uint8_t local_buffer_tracker = igh_message_add_tuple(local_pkt_id, test_shield_id);
    
    uint8_t actual_tuple[14];
    memcpy(actual_tuple, &igh_msg_buffer[54], 14);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(actual_tuple, expected_tuple, 14);
    TEST_ASSERT(local_buffer_tracker == (54 + 14));
}

void test_igh_message_add_tuple_does_nothing_if_tuple_does_not_fit(void)
{
    igh_msg_buffer_tracker = 250;

    igh_pkt_id local_pkt_id = SPEAR_ID;
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};

    uint8_t local_buffer_tracker = igh_message_add_tuple(local_pkt_id, test_shield_id);
    
    TEST_ASSERT(local_buffer_tracker == 250);
}

void test_igh_message_add_tuple_does_nothing_if_pkt_id_is_unknown(void)
{
    igh_msg_buffer_tracker = 38;

    igh_pkt_id local_pkt_id = (igh_pkt_id)0x70; // unknown tuple id
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};

    uint8_t local_buffer_tracker = igh_message_add_tuple(local_pkt_id, test_shield_id);
    
    TEST_ASSERT(local_buffer_tracker == 38);
}

void test_igh_message_process_incoming_msg_returns_zero_if_there_is_no_start_byte(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t settings_cmd[15] = {0x10,0x0D,0x01,0x01,0x02,0x04,0x02,0x23,0x45,0x19,0x04,0x37,0x52,0x67,0x90};
    uint8_t msg_type = 0x11;  // Settings message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x29; // wrong start of frame
    igh_cmd_buffer[1] = 0x21; // right length
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], test_shield_id, 12); 
    igh_cmd_buffer[16] = msg_id;
    memcpy(&igh_cmd_buffer[17], settings_cmd, 15);
    igh_cmd_buffer[32] = 0x3E; // end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == 0x00);
}

void test_igh_message_process_incoming_msg_returns_zero_if_length_is_not_valid(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t settings_cmd[15] = {0x10,0x0D,0x01,0x01,0x02,0x04,0x02,0x23,0x45,0x19,0x04,0x37,0x52,0x67,0x90};
    uint8_t msg_type = 0x11;  // Settings message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x3C; // start of frame
    igh_cmd_buffer[1] = 0x14; // The wrong length  
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], test_shield_id, 12); 
    igh_cmd_buffer[16] = msg_id;
    memcpy(&igh_cmd_buffer[17], settings_cmd, 15);
    igh_cmd_buffer[32] = 0x3E; // end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == 0x00);
}

void test_igh_message_process_incoming_msg_returns_zero_if_there_is_no_stop_byte(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t settings_cmd[15] = {0x10,0x0D,0x01,0x01,0x02,0x04,0x02,0x23,0x45,0x19,0x04,0x37,0x52,0x67,0x90};
    uint8_t msg_type = 0x11;  // Settings message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x3C; // start of frame
    igh_cmd_buffer[1] = 0x21; // The right length  
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], test_shield_id, 12); 
    igh_cmd_buffer[16] = msg_id;
    memcpy(&igh_cmd_buffer[17], settings_cmd, 15);
    igh_cmd_buffer[32] = 0x23; // wrong end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == 0x00);
}

void test_igh_message_process_incoming_msg_returns_zero_if_serial_is_wrong(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t wrong_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x20,0x05,0xe7,0x9a,0x37,0x37};
    uint8_t settings_cmd[15] = {0x10,0x0D,0x01,0x01,0x02,0x04,0x02,0x23,0x45,0x19,0x04,0x37,0x52,0x67,0x90};
    uint8_t msg_type = 0x11;  // Settings message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x3C; // start of frame
    igh_cmd_buffer[1] = 0x21; // length
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], wrong_shield_id, 12); // use the wrong serial number
    igh_cmd_buffer[16] = msg_id;
    memcpy(&igh_cmd_buffer[17], settings_cmd, 15);
    igh_cmd_buffer[20] = 0x3E; // end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == 0x00);
}

void test_igh_message_process_incoming_msg_returns_ACK_if_message_is_ACK(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t ack_msg[3] = {0x00,0x01,0x13}; // An Ack that says message number 0x13 was processed by cloud
    uint8_t msg_type = MSG_ACK;  // ACK message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x3C; // start of frame
    igh_cmd_buffer[1] = 0x15; // length
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], test_shield_id, 12); 
    igh_cmd_buffer[16] = msg_id; 
    memcpy(&igh_cmd_buffer[17], ack_msg, 3); 
    igh_cmd_buffer[20] = 0x3E; // end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == MSG_ACK);
}

void test_igh_message_process_incoming_msg_returns_settings_if_message_is_settings(void)
{
    uint8_t test_shield_id[12] = {0xe0,0x0f,0xce,0x68,0x9a,0x75,0x47,0x05,0xe7,0x9a,0x0e,0x37};
    memcpy(igh_current_system_settings.serial_number, test_shield_id, 12);
    uint8_t settings_cmd[15] = {0x10,0x0D,0x01,0x01,0x02,0x04,0x02,0x23,0x45,0x19,0x04,0x37,0x52,0x67,0x90};
    uint8_t msg_type = SETTINGS_MSG;  // Settings message 
    uint8_t direction = 0x44;
    uint8_t msg_id = 0x35;

    igh_cmd_buffer[0] = 0x3C; // start of frame
    igh_cmd_buffer[1] = 0x21; // length
    igh_cmd_buffer[2] = msg_type; 
    igh_cmd_buffer[3] = direction;
    memcpy(&igh_cmd_buffer[4], test_shield_id, 12); 
    igh_cmd_buffer[16] = msg_id;
    memcpy(&igh_cmd_buffer[17], settings_cmd, 15);
    igh_cmd_buffer[32] = 0x3E; // end of frame  

    uint8_t _ret = igh_message_process_incoming_msg(igh_cmd_buffer);
    TEST_ASSERT(_ret == SETTINGS_MSG);
}
