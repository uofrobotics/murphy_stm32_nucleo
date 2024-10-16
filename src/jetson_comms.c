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
 struct drive_motor_state drive_mt_state = {0};

	
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
	xLastWakeTime = xTaskGetTickCount();
	
    while (1) {
        if (xQueueReceive(jetson_packet_char_q, &packet, portMAX_DELAY) == pdPASS) {
// data packet
				if (packet <= 127) {
					
					
					// are we expecting it?
					if (drive_mt_state.packet_count >=1) {
						
						// keeping track of packet count is how we know the order of packets
						switch (drive_mt_state.packet_count) {
							
							// BL,128,m1
							case 1:
								control_sabertooth(SABER_ADDR_BACK, drive_mt_state.m1_128_com, packet);
								break;
							
							// BR,128,m2
							case 2:
								control_sabertooth(SABER_ADDR_BACK, drive_mt_state.m2_128_com, packet);
								break;
							
							// FL,129,m1
							case 3:
								control_sabertooth(SABER_ADDR_FRONT, drive_mt_state.m1_129_com, packet);
								break;
							
							// FR,129,m2
							case 4:
								control_sabertooth(SABER_ADDR_FRONT, drive_mt_state.m2_129_com, packet);
								break;
							
							// not sure what a default case looks like, probably error condition
						}
						
						
						
						// if finished with all packets, set packet count to zero
						if (drive_mt_state.packet_count == 4) {
							drive_mt_state.packet_count = 0;
							
						} 
						
						// else normally increment
						else {
							drive_mt_state.packet_count++;
						}
						
					
					}
					
					
					// other things could be using this data, check here
					
					
				}
				
				/*
				// heartbeat, respond immediately
				else if (packet == 128) {
					
					// really shouldn't be waiting much at all
					xQueueSendToBack(heartbeat_q, &heartbeat_state, pdMS_TO_TICKS(2));
					send_jetson_byte(128, 10);
				}
				*/
				// drive motor control
				 else if(packet >= 129 && packet <= 145) {
					 
					 // each bit represents the direction for each motor
					 /*
					 BL,128,m1   BR,128,m2   FL,129,m1  FR,129,m2
			bit  3                2                  1                0
					 
					 where 0 is forward, 1 is reverse
					 */
					 uint8_t drive_motor_dir = packet - DRIVE_MOTOR_CTRL_START;
					 
					 drive_mt_state.m1_128_com = (drive_motor_dir & M1_128_MASK) >> 3;
					 
					 // 4 -> drive forward motor 2
					 // 5 -> drive backwards motor 2
					 drive_mt_state.m2_128_com = ( (drive_motor_dir & M2_128_MASK) >> 2) + 4;
					 
					 drive_mt_state.m1_129_com = (drive_motor_dir & M1_129_MASK) >> 1;
					 
					 drive_mt_state.m2_129_com = (drive_motor_dir & M2_129_MASK) + 4;
					 
					 // do not increment, keep count at one if they send many drive control packets
					 drive_mt_state.packet_count = 1;
					 
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


