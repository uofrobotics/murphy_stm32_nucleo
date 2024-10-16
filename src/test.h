
#include "stm32f10x.h"
void unit_test_all_robot_functionality(void);




static char test_0_prompt[] = "\r\nIs the green nucleo led blinking at roughly 10 Hz? (y): ";
static char test_1_prompt[] = "\r\nIs the red indicator on? (y): ";
static char test_2_prompt[] = "\r\nIs the amber indicator on? (y): ";
static char test_3_prompt[] = "\r\nIs the green indicator on? (y): ";

static char test_4_prompt[] = "\r\n------DRIVE MOTOR TESTING: MOTORS WILL SPIN, YOU SHOULD LIFT THE ROBOT------. Proceed?: (y): ";
static char test_5_prompt[] = "\r\nAre all the drive motors spining to drive robot FORWARD slowly? (y): ";
static char test_6_prompt[] = "\r\nAre all the drive motors spining to drive robot REVERSE slowly? (y): ";
static char test_7_prompt[] = "\r\nAre all the drive motors spining to drive robot FORWARD fast? (y): ";
static char test_8_prompt[] = "\r\nAre all the drive motors spining to drive robot REVERSE fast? (y): ";
static char test_9_prompt[] = "\r\nIs the left ultrasonic reading working? (y): ";


void test_0_led_blinker(void);

void test_1_red_indicator(void);

void test_2_amber_indicator(void);
void test_3_green_indicator(void);

void test_4_drive_motor_spin_warning(void);
void test_5_drive_motor_spin_fwd_slow(void);
void test_6_drive_motor_spin_rev_slow(void);
void test_7_drive_motor_spin_fwd_fast(void);
void test_8_drive_motor_spin_rev_fast(void);

void test_9_left_ultrasonic(uint16_t current_test_num);
#define NUM_OF_TESTS 10

