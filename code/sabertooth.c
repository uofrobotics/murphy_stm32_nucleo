#include "FreeRTOS.h"
#include "stm32f10x.h"
#include "sabertooth.h"
#include "queue.h"
#include "io_control.h"
#include "indicator.h"

#define CHKSUM_MASK 0x7f


void setup_sabertooth_serial(void) {
		// PA9 AFIO push/pull max speed 50MHz, USART TX
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
	GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
	
	
	USART1->CR1 |= USART_CR1_UE;
	
		// 115.2 kbps, fplc = 36Mhz
	// USART1->BRR = (0x8 & USART_BRR_DIV_Fraction) | ((0x13 << 4) & USART_BRR_DIV_Mantissa);
	
	// 115200 Only USART1 is clocked with PCLK2 (72 MHz max)
	USART1->BRR = (0x1 & USART_BRR_DIV_Fraction) | ((0x27 << 4) & USART_BRR_DIV_Mantissa);
	// enable transmit
	USART1->CR1 |= USART_CR1_TE;
	
	// reenable usart
	USART1->CR1 |= USART_CR1_UE;
	
	
}

int send_sabertooth_byte(uint8_t byte, uint32_t retries) {

		uint32_t attempts = 0;
    while (attempts <= retries)   { 

        // if Transmit data register (buffer) empty
        if ((USART1->SR & USART_SR_TXE) == USART_SR_TXE) {
            // send data
            USART1->DR = byte;
            return 0;
        }
				attempts++;
    }
    return 1;  // exceeded retries
	
}

void control_sabertooth(uint8_t addr, uint8_t com, uint8_t data) {
	
			
			send_sabertooth_byte(addr, 10);
			// do not decrease it, any lower and sometimes packets will be dropped
			dirty_delay(125);
	
			send_sabertooth_byte(com, 10);
			dirty_delay(125);

			send_sabertooth_byte(data, 10); 
			dirty_delay(125);

			send_sabertooth_byte(  ((uint8_t) (addr + com + data)) & CHKSUM_MASK, 10); // check sum
			dirty_delay(125);

	
}

void lock_all_motors_stopped(void) {
				// PC10
				GPIOC->BSRR = GPIO_BSRR_BR10;
	
			// also send speeds of zero to all drive motors
			stop_drive_motors();
			control_red_indicator(1);
}

void unlock_all_motors_moveable(void) {
			// PC10
			GPIOC->BSRR = GPIO_BSRR_BS10;
}


void stop_drive_motors(void) {
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M1, 0);
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M2, 0);
	
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M1, 0);
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M2, 0);
	
	
}
