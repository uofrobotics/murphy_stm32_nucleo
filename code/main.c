
#include "CLI.h"
#include "FreeRTOS.h"
#include "io_control.h"
#include "queue.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "task.h"
#include "usart.h"
#include "sabertooth.h"
#include "ultrasonic.h"
#include "jetson_comms.h"

#define JETSON_PARSER_PRIO (tskIDLE_PRIORITY + 4)
#define PING_PARSER_PRIO (tskIDLE_PRIORITY + 3)
#define CLI_RECEIVE_PRIO (tskIDLE_PRIORITY + 2)
#define BLINKY_TASK_PRIO (tskIDLE_PRIORITY + 1)

/*
note: under normal conditions code should never reach any forever_blink_led() calls
it is only meant to clearly indicate that something is wrong
this would only happen when:
a hardfault occurs
tasks, queues could not be created
scheduler could not start



*/


int main() {
    turn_on_clocks();
    configure_io();
    serial_open();
	setup_sabertooth_serial();
	configure_ultrasonic();
	setup_jetson_serial();

    extern QueueHandle_t buff;
		extern QueueHandle_t jetson_packet_char_q;


    refresh_screen(1, -1, -1, -1);
    CLI_transmit(prompt, strlen(prompt));

    // if cannot create queue due to insufficent heap mem
    if ((buff = xQueueCreate(1, sizeof(char))) == NULL) {
        forever_blink_led();
    }

    if ((jetson_packet_char_q = xQueueCreate(1, sizeof(char))) == NULL) {
        forever_blink_led();
    }
		
    if (xTaskCreate(vCLI_receive, "CLI_recv", configMINIMAL_STACK_SIZE + 300,
                    NULL, CLI_RECEIVE_PRIO,
                    NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        forever_blink_led();
    }

		extern TaskHandle_t Ping_parser_h;
		if (xTaskCreate(vPing_parser, "vPing_parser", configMINIMAL_STACK_SIZE + 300,
                    NULL, PING_PARSER_PRIO,
                    &Ping_parser_h) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        forever_blink_led();
    }
										
		
		extern TaskHandle_t jetson_packet_parser_h;
		if (xTaskCreate(vJetson_packet_parser, "vJetson_packet_parser", configMINIMAL_STACK_SIZE + 300,
                    NULL, JETSON_PARSER_PRIO,
                    &jetson_packet_parser_h) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        forever_blink_led();
    }
    vTaskStartScheduler();

    // if got here, there was not enough heap mem available for the idle task to
    // be created
    forever_blink_led();
}
void HardFault_Handler(void);
void HardFault_Handler(void) {
			
			// indicate clearly that something is wrong
	    forever_blink_led();
}