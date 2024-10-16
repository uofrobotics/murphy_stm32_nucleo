#include "FreeRTOS.h"
#include "task.h"

#include "ultrasonic.h"
#include "io_control.h"
#include "sabertooth.h"
#include "indicator.h"


#define CPU_FREQ_MHz 72

/*
YES, I KNOW GLOBAL VARIABLES ARE BAD
but I needed help getting the exti2 interrupts NVIC prrority set
properly, else there will be hard faults

*/
static uint16_t ultrasonic_echo_high_time = 0;


TaskHandle_t Ping_parser_h;

// TODO: use TIM1 interrupt to control sample rate, default like this is max
void vPing_parser(void* param) {
	
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 75 );
	xLastWakeTime = xTaskGetTickCount();
	

	while(1) {
		//control_green_onboard_led(1);
		uint16_t current_ping_distance = get_ultrasonic_sensor_dist();
		
		if (current_ping_distance < 10) {
			control_green_onboard_led(1);
			lock_all_motors_stopped();
			control_amber_indicator(1);
			
		} else {
			control_green_onboard_led(0);
						control_amber_indicator(0);
		}
		
			
		send_trig_pulse();
		/* This task should execute every 50 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, xPeriod );
		
	}
}
// returns distance in cm
// TODO: could compensate for temperature for speed of sound
uint16_t get_ultrasonic_sensor_dist(void) {

	// TIM1->CNT is the number of us the echo pulse was high
	// speed of sound is 0.034 cm/us
	// for ~20 to 30 deg c, 40 to 60% rel humitidy
	

	// VERY WEIRD: WHEN ON USB NEED TO SUBTRACT 10 FOR CORRECT DISTANCE
	uint16_t ultrasonic_dist = (uint16_t) (((ultrasonic_echo_high_time * 0.034) / 2) - 39);
	
	// WHEN POWERED VIA VIN (9V BATTERY) THE EXPECTED FORUMLA BELOW WORKS
	//uint16_t ultrasonic_dist = (uint16_t) (((ultrasonic_echo_high_time * 0.034) / 2));
	
	if (ultrasonic_dist > ULTRASONIC_MAX_DIST_CM) {
		ultrasonic_dist = ULTRASONIC_MAX_DIST_CM;
	}
	else if (ultrasonic_dist < ULTRASONIC_MIN_DIST_CM) {
		ultrasonic_dist = ULTRASONIC_MIN_DIST_CM;
	}
	
	return ultrasonic_dist;
	
}

void send_trig_pulse(void) {
	// pb1 high
	GPIOB->BSRR = GPIO_BSRR_BS1;
	
	// TODO: TIM1 is wasted here, instead of blocking use TIM1 for the wait???
	// delay for 10 micro seconds
	dirty_delay(125);
	// pb 1 low
	GPIOB->BSRR = GPIO_BSRR_BR1;

}
void configure_ultrasonic(void) {

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->ARR=0xFFFF; // up count mode counts to ARR then restarts to zero. We want to avoid that, so make arr the max
	TIM1->PSC	= CPU_FREQ_MHz - 1;	// counter val: every count is 1us
	TIM1->SR &= ~TIM_SR_UIF;
	TIM1->EGR |= TIM_EGR_UG; // re init counter
	TIM1->CR1 |= TIM_CR1_OPM; // enable one pulse mode
		// up counter is default

	
	// DO NOT START TIMER HERE

	
	//Select Port B pins as the source for EXTI 0 events
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PB;
	
	// Unmask PB2 as the interrupt source
	EXTI->IMR |= EXTI_IMR_MR2;
	
	// Select the rising edge of PB2 events as the trigger
	EXTI->RTSR |= EXTI_RTSR_TR2;
	
	// Unmask EXTI2 as an interrupt source in the NVIC
	NVIC->ISER[0] |= NVIC_ISER_SETENA_8;
	
	// pb1 trig pin
	GPIOB->CRL |= GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1;
	GPIOB->CRL &= ~(GPIO_CRL_CNF1_0 | GPIO_CRL_CNF1_1);
	GPIOB->BSRR = GPIO_BSRR_BR1;
	
	// start the first measurement
	send_trig_pulse();
}

void EXTI2_IRQHandler(void) {
	EXTI->PR |= EXTI_PR_PR2; // Clear the pending interrupt bit
	
	
	// if currently configured as rising edge
	if ((EXTI->RTSR & EXTI_RTSR_TR2) != 0) {
		
		// first thing start the timer
		TIM1->CR1 |= TIM_CR1_CEN;
				
		// make it no longer rising edge
		EXTI->RTSR &= ~EXTI_RTSR_TR2;
		
		// make falling edge triggered
	  EXTI->FTSR |= EXTI_FTSR_TR2;
	}
	// else currently configured as falling edge
	else {
		// sensor done a reading
		
		// else timer is still counting (valid reading)
    // CNT is in us
		
		/*
		YES, I KNOW GLOBAL VARIABLES ARE BAD
		but I needed help getting the exti2 interrupts NVIC prrority set
		properly, else there will be hard faults
		
		there should be a queue here

		*/
		ultrasonic_echo_high_time = TIM1->CNT;
		
		TIM1->EGR |= TIM_EGR_UG;
		TIM1->CR1 |= TIM_CR1_CEN;
				
		// make it no longer falling edge
		EXTI->FTSR &= ~EXTI_FTSR_TR2;
		
		// make rising edge triggered
		EXTI->RTSR |= EXTI_RTSR_TR2;
	}
}
