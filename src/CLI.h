#include "stdint.h"
#include "stdlib.h"
// stmlink virtual com port

static char prompt[] = "\r\nRobot CLI (unit_test/help): ";

static char invalid_cmd[] = "\r\nInvalid command: ";

static char help[] =
    "\r\nSupported Commands:"
		"\r\n    unit_test: test all robot functionality"
    "\r\n    help: display this help command";

static char unit_test_cmd[] = "unit_test\r";
static char help_cmd[] = "help\r";
static char idle_status[] = "idle";
static char spaces[] = "     ";
static char left_ping_label[] = "Left ping dist: ";
static char right_ping_label[] = "Right ping dist: ";

/// @brief send a string over the serial
/// @param p_data char buffer of data to send
/// @param size length of the buffer to send
void CLI_transmit(char p_data[], unsigned int size);

/// @brief block untill the buffer of `size` is filled or a return carriage is
/// detected
/// @param p_data char buffer of data to be received/placed in
/// @param size length of the buffer
void vCLI_receive(void* param);

/// @brief refresh the screen with ANSI control characters
/// @param should_init decides if to init scroll, clear or save and restore
/// cursor
void refresh_screen(uint8_t should_init, int current_test_num, int left_ping_dist, int right_ping_dist);
/// @brief NULL every position in the buffer
/// @param p_data char buffer to clear
/// @param size length of buffer
void flush_buff(char p_data[], unsigned int size);

char* convertIntegerToChar(int N);

