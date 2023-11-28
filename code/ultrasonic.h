#include "stm32f10x.h"
#define ULTRASONIC_TRIG_PULSE_RATE_MS 100
#define ULTRASONIC_MAX_DIST_CM 400
#define ULTRASONIC_MIN_DIST_CM 2

void send_trig_pulse_check(void);
void configure_ultrasonic(void);
uint16_t get_ultrasonic_sensor_dist(void);
void send_trig_pulse(void);
void EXTI2_IRQHandler(void);
void vPing_parser(void* param);