#include "io_control.h"
#include "indicator.h"
#include "usart.h"
void turn_on_clocks(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM2EN;
}

void configure_io(void) {
    /* enable on board led */
    GPIOA->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF5 & ~GPIO_CRL_CNF5_1;
	
		// PC12 / IN2  relay / red light
			GPIOC->CRH |= GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1;
			GPIOC->CRH&= ~GPIO_CRH_CNF12_0 & ~GPIO_CRH_CNF12_1;
	// PD2 / IN3 relay / amber light
			GPIOD->CRL |= GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1;
			GPIOD->CRL &= ~GPIO_CRL_CNF2_0 & ~GPIO_CRL_CNF2_1;
	
	// PB15 / IN4 relay / green light
				GPIOB->CRH |= GPIO_CRH_MODE15_0 | GPIO_CRH_MODE15_1;
			GPIOB->CRH &= ~GPIO_CRH_CNF15_0 & ~GPIO_CRH_CNF15_1;
	control_amber_indicator(0);
	control_green_indicator(0);
	control_red_indicator(0);
	
}

void control_green_onboard_led(unsigned int on_or_off) {
    /* off == 0, on == 1 */
    if (on_or_off == 0) {
        GPIOA->ODR &= ~(unsigned int)GPIO_ODR_ODR5;
    } else if (on_or_off == 1) {
        GPIOA->ODR |= GPIO_ODR_ODR5;
    }
}

uint8_t get_led_state(void) { return (GPIOA->ODR & GPIO_ODR_ODR5) >> 5; }

void dirty_delay(uint32_t microseconds) {
    for (unsigned int i = 0; i < microseconds * 10; i++) {
        __ASM("nop");
    }
}

void forever_blink_led(void) {
    while (1) {
        control_green_onboard_led(1);
        dirty_delay(1000000);
        control_green_onboard_led(0);
        dirty_delay(1000000);
    }
}
