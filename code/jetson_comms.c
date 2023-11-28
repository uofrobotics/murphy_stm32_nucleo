#include "FreeRTOS.h"
#include "queue.h"

#include "jetson_comms.h"
#include "sabertooth.h"
#include "indicator.h"

QueueHandle_t jetson_packet_char_q;
TaskHandle_t jetson_packet_parser_h;

void setup_jetson_serial(void) {
    NVIC->ISER[1] = 1 << 7;  // enable usart3 in nvic
	
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    // PB10 AFIO push/pull max speed 50MHz, USART3 TX
    GPIOB->CRH |= GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF10_0;

    // PB11 floating input, USART RX (default port config)

    USART3->CR1 |= USART_CR1_UE;

    // default reset is: for data bits =8, no parity, 1 stop bit, don't need to
    // configure here

    // 36Mhz clock might not matter??, see page 799 and table 192 for 115.2 kbps
    // is 19.5
	// PCLK1 (36 MHz max).
    USART3->BRR =
        (0x8 & USART_BRR_DIV_Fraction) | ((0x13 << 4) & USART_BRR_DIV_Mantissa);

    // enable interrupts when a char is received
    USART3->CR1 |= USART_CR1_RXNEIE;

    // enable transmit and receive
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // reenable usart
    USART3->CR1 |= USART_CR1_UE;
	
}

void vJetson_packet_parser (void* params) {
	uint8_t packet;
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 50 );
	xLastWakeTime = xTaskGetTickCount();
	
    while (1) {
        if (xQueueReceive(jetson_packet_char_q, &packet, portMAX_DELAY) == pdPASS) {
						send_jetson_byte(packet, 10);
					
						switch(packet) {
							case 'f':
								control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M1, 30);
								control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M2, 30);
								
								control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M1, 30);
								control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M2, 30);
							control_green_indicator(1);
								control_red_indicator(0);
							control_amber_indicator(0);
								break;
							case 'r':
								control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M1, 30);
								control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M2, 30);
								
								control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M1, 30);
								control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M2, 30);
							control_green_indicator(1);
															control_red_indicator(0);
														control_amber_indicator(0);


								break;
							case 's':
								stop_drive_motors();
								control_red_indicator(1);
							control_green_indicator(0);
							
								break;
							
						}
					
				}
					
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
    }
	
	
}


void USART3_IRQHandler(void) {
	
	uint8_t packet = (uint8_t) USART3->DR;
	
	xQueueSendToBackFromISR(jetson_packet_char_q, &packet, NULL);
	
}

// TODO: refactor this since its the same for all usart sends
int send_jetson_byte(uint8_t byte, uint32_t retries) {

		uint32_t attempts = 0;
    while (attempts <= retries)   { 

        // if Transmit data register (buffer) empty
        if ((USART3->SR & USART_SR_TXE) == USART_SR_TXE) {
            // send data
            USART3->DR = byte;
            return 0;
        }
				attempts++;
    }
    return 1;  // exceeded retries
	
}


