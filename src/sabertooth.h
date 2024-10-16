
#include "stm32f10x.h"
#define DRIVE_MOTOR_CTRL_START 129
#define M1_128_MASK 0x8
#define M2_128_MASK 0x4
#define M1_129_MASK 0x2
#define M2_129_MASK 0x1

#define SABER_ADDR_BACK 128
#define SABER_ADDR_FRONT 129

// these are mixed for the robot, datasheet is otherwise
#define SABER_CMD_REV_M1 0
#define SABER_CMD_FWD_M1 1
#define SABER_CMD_REV_M2 4
#define SABER_CMD_FWD_M2 5

void setup_sabertooth_serial(void);


int send_sabertooth_byte(uint8_t byte, uint32_t retries);

void control_sabertooth(uint8_t addr, uint8_t com, uint8_t data);


void motor_control_task(void* params);

// gracefully tell sabertooth to stop all four drive motors by serial commands
// slow
void stop_drive_motors(void);


// fast as possible hard brake by pulling S2 pin low via mosfet
// same effect as pressing red e-stop button
// currently, only ultrasonic ping sensor distance too low and processor faults will trigger this:
// still todo: heartbeat
void lock_all_motors_stopped(void);
void unlock_all_motors_moveable(void);
