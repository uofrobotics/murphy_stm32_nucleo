#include "stm32f10x.h"

void setup_jetson_serial(void);

void vJetson_packet_parser (void* params);

int send_jetson_byte(uint8_t byte, uint32_t retries);

void USART3_IRQHandler(void);

struct drive_motor_state {
	uint8_t m1_128_com;
	uint8_t m2_128_com;
	uint8_t m1_129_com;
	uint8_t m2_129_com;
	uint8_t packet_count;
	
};