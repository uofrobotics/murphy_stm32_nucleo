#include "FreeRTOS.h"
#include "queue.h"

#include "test.h"
#include "io_control.h"
#include "CLI.h"
#include "usart.h"
#include "indicator.h"
#include "sabertooth.h"
#include "ultrasonic.h"

void unit_test_all_robot_functionality(void) {
	extern TaskHandle_t Ping_parser_h;
		extern TaskHandle_t jetson_packet_parser_h;

		stop_drive_motors();
	
	// we want exclusive access to all hw, suspend all the other tasks but ourselves
	vTaskSuspend(Ping_parser_h);
	vTaskSuspend(jetson_packet_parser_h);
	
	
	// need to reable usart interrupt again to be receivering data
	USART2->CR1 |= USART_CR1_RXNEIE;
	
	
	extern QueueHandle_t buff;
	uint16_t current_test_num = 0;
	uint8_t was_prompt_sent = 0;
	
	uint8_t msg;
	while(current_test_num < NUM_OF_TESTS) {
		
			// don't block
		  xQueueReceive(buff, &msg, 0);
			
			if (msg == 'y') {
				
				// clean up test since it passed
				switch (current_test_num) {
					case 0:
						control_green_onboard_led(0);
						break;
					case 1:
						control_red_indicator(0);
					break;
					case 2:
						control_amber_indicator(0);
					break;
					case 3:
						control_green_indicator(0);
					break;
					case 4:
						// nothing
					break;
					case 5:
						stop_drive_motors();
					break;
					case 6:
						stop_drive_motors();
					break;
					case 7:
						stop_drive_motors();
					break;
					case 8:
						stop_drive_motors();
					break;
					case 9:
						// nothing todo: don't send any more trig pulses
					break;
				}
				
				
			send_byte(msg, 5);
			current_test_num++;
			refresh_screen(0, current_test_num, -1, -1);
			was_prompt_sent = 0;
			msg = NULL;
			}
			

			switch(current_test_num) {
				case 0:
					if(was_prompt_sent == 0) {
						CLI_transmit(test_0_prompt, strlen(test_0_prompt));
						was_prompt_sent = 1;
					}
					test_0_led_blinker();
					break;
				
				case 1:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_1_prompt, strlen(test_1_prompt));
						was_prompt_sent = 1;
					}
					test_1_red_indicator();
					break;
				
				case 2:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_2_prompt, strlen(test_2_prompt));
						was_prompt_sent = 1;
					}
					test_2_amber_indicator();
					break;
				
				case 3:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_3_prompt, strlen(test_3_prompt));
						was_prompt_sent = 1;
					}
					test_3_green_indicator();
					break;
					
				case 4:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_4_prompt, strlen(test_4_prompt));
						was_prompt_sent = 1;
					}
					test_4_drive_motor_spin_warning();
					break;
					
				case 5:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_5_prompt, strlen(test_5_prompt));
						was_prompt_sent = 1;
					}
					test_5_drive_motor_spin_fwd_slow();
					break;
				case 6:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_6_prompt, strlen(test_6_prompt));
						was_prompt_sent = 1;
					}
					test_6_drive_motor_spin_rev_slow();
					break;
				case 7:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_7_prompt, strlen(test_7_prompt));
						was_prompt_sent = 1;
					}
					test_7_drive_motor_spin_fwd_fast();
					break;
					
				case 8:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_8_prompt, strlen(test_8_prompt));
						was_prompt_sent = 1;
					}
					test_8_drive_motor_spin_rev_fast();
					break;
				case 9:
				
					if(was_prompt_sent == 0) {
						CLI_transmit(test_9_prompt, strlen(test_9_prompt));
						was_prompt_sent = 1;
					}
					test_9_left_ultrasonic(current_test_num);
					break;
				}
		
		
	}

		// dont forget to resume them
		vTaskResume(Ping_parser_h);
		vTaskResume(jetson_packet_parser_h);
	
}

void test_0_led_blinker(void) {
        control_green_onboard_led(1);
        dirty_delay(100000);
        control_green_onboard_led(0);
        dirty_delay(100000);

}

void test_1_red_indicator(void) {
	control_red_indicator(1);
	
}

void test_2_amber_indicator(void) {
	control_amber_indicator(1);
}

void test_3_green_indicator(void) {
control_green_indicator(1);
}

void test_4_drive_motor_spin_warning(void) {
	// do nothing
	
}
void test_5_drive_motor_spin_fwd_slow(void) {
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M1, 30);
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M2, 30);
	
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M1, 30);
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M2, 30);
	

	
}
void test_6_drive_motor_spin_rev_slow(void) {
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M1, 30);
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M2, 30);
	
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M1, 30);
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M2, 30);
	
}

void test_7_drive_motor_spin_fwd_fast(void) {
	
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M1, 127);
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_FWD_M2, 127);
	
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M1, 127);
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_FWD_M2, 127);
	
}
void test_8_drive_motor_spin_rev_fast(void) {
	
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M1, 127);
	control_sabertooth(SABER_ADDR_FRONT, SABER_CMD_REV_M2, 127);
	
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M1, 127);
	control_sabertooth(SABER_ADDR_BACK, SABER_CMD_REV_M2, 127);
}

void test_9_left_ultrasonic(uint16_t current_test_num) {
		uint16_t dist = get_ultrasonic_sensor_dist();
		dirty_delay(100000);
		send_trig_pulse();
		refresh_screen(0, current_test_num, dist, -1);
	
	

	
	
}
