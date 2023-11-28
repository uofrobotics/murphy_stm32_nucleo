#include "indicator.h"
#include "stm32f10x.h"
// 1 == on, 0 == off
void control_red_indicator(uint8_t on_or_off) {
						// PC12
					if (on_or_off != 1) {
						GPIOC->BSRR = GPIO_BSRR_BS12;
						
					} else {
						GPIOC->BSRR = GPIO_BSRR_BR12;
						
					}
	
}

void control_amber_indicator(uint8_t on_or_off) {

					// PD2
					if (on_or_off != 1) {  
						GPIOD->BSRR = GPIO_BSRR_BS2;
						
					} else {
						GPIOD->BSRR = GPIO_BSRR_BR2;
					}
}

void control_green_indicator(uint8_t on_or_off) {

					
					// PB15
					if (on_or_off != 1) {
						GPIOB->BSRR = GPIO_BSRR_BS15;
					} else {
						GPIOB->BSRR = GPIO_BSRR_BR15;
						
					}
}
