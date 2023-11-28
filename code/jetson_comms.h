#include "stm32f10x.h"

void setup_jetson_serial(void);

void vJetson_packet_parser (void* params);

int send_jetson_byte(uint8_t byte, uint32_t retries);

void USART3_IRQHandler(void);

