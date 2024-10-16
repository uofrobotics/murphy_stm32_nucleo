#include "CLI.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"
#include "test.h"

QueueHandle_t buff;

// from geeksforgeeks how to convert given number to char array
char* convertIntegerToChar(int N)
{
	    // Declare char array for result
    char* arr = NULL;
	
		if (N == 0) {
			 arr[0] = '0';
			 return (char*)arr;

			
		}
 
    // Count digits in number N
    int m = N;
    int digit = 0;
    while (m) {
 
        // Increment number of digits
        digit++;
 
        // Truncate the last
        // digit from the number
        m /= 10;
    }
 

 
    // Declare duplicate char array
    char arr1[digit];
 
    // Memory allocation of array
    arr = (char*) malloc((unsigned int) digit);
 
    // Separating integer into digits and
    // accommodate it to character array
    int index = 0;
    while (N) {
 
        // Separate last digit from
        // the number and add ASCII
        // value of character '0' is 48
        arr1[++index] = ( (char) (N % 10)) + '0';
 
        // Truncate the last
        // digit from the number
        N /= 10;
    }
 
    // Reverse the array for result
    int i;
    for (i = 0; i < index; i++) {
        arr[i] = arr1[index - i];
    }
 
    // Char array truncate by null
    arr[i] = '\0';
 
    // Return char array
    return (char*)arr;
}

// size is number of chars/bytes
void CLI_transmit(char p_data[], unsigned int size) {
    for (int i = 0; i < (int)size; i++) {
        send_byte(p_data[i], 5);
    }
}

// size: return only when 'size' amount of chars/bytes have been read
void vCLI_receive(void* param) {
    // attempt to fill entire buffer, waiting for user to type command
    char input_buff[INPUT_BUFF_LEN];  // a max of 40 chars can be accepted
    int current_buff_index = 0;
    uint8_t msg;

		TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 200 );
	xLastWakeTime = xTaskGetTickCount();
	
    while (1) {
        // while the interrupt is enabled, we need to wait... block untill then
        // while ((USART2->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE) ;
        if (current_buff_index > INPUT_BUFF_LEN - 1) {
            // reached max chars, stop/disable interrupt
            USART2->CR1 &= ~USART_CR1_RXNEIE;
        }

        // do not block here, if you do will never get to handing the command
        // when usart interrupt is disabled
        else if (xQueueReceive(buff, &msg, portMAX_DELAY) == pdPASS) {
            input_buff[current_buff_index] = msg;

            if (input_buff[current_buff_index] == DEL &&
                current_buff_index > 0) {
                // handle the DEL manually in our buffer
                input_buff[current_buff_index] = NULL;
                input_buff[current_buff_index - 1] = NULL;
                current_buff_index = current_buff_index - 2;

                // manually send our own DEL to stay consisent with tty
                send_byte(DEL, 5);

            }

            // if the user is done before buffer is filled, just return it,
            // command is ready to parse
            else if (input_buff[current_buff_index] == '\r') {
                // done reading line, disable interrupts because we don't want
                // to overrwite userdata if they type something untill we are
                // done processing it disable interrupt
                USART2->CR1 &= ~USART_CR1_RXNEIE;
            }

            else {
                // normal char, echo it back to user
                send_byte(input_buff[current_buff_index], 5);
            }

            current_buff_index++;
        }

        // if the usart is disabled, ready to make descisions the user input is
        // ready
				
				// full command processing
        if ((USART2->CR1 & USART_CR1_RXNEIE) != USART_CR1_RXNEIE) {
            if (input_buff[0] != '\r') {
                if (strcmp(input_buff, unit_test_cmd) == 0) {
									   refresh_screen(1, 0, -1, -1);
										unit_test_all_robot_functionality();
                }
								else if (strcmp(input_buff, help_cmd) == 0) {
                    CLI_transmit(help, strlen(help));

                } else {
                    CLI_transmit(invalid_cmd, strlen(invalid_cmd));
                    CLI_transmit(input_buff, strlen(input_buff));
                }
            }

            CLI_transmit(prompt, strlen(prompt));

            // screen is only refreshed after command is entered.
            // which is fine for this since led can't update otherwise
            // since this main loop blocks untill the entire command
            // is present. Fix this by making the refresh screen
            // use systick to refresh at regular intervals regardless of
            // what the main loop is doing
            refresh_screen(0, -1, -1, -1);

            flush_buff(input_buff, INPUT_BUFF_LEN);
            current_buff_index = 0;

            // need to reable usart interrupt again to be receivering data
            USART2->CR1 |= USART_CR1_RXNEIE;
        }

        // to prevent task starvation, make sure this task blocks at least a bit
        // for other stuff probably better controlable via queue
				vTaskDelayUntil( &xLastWakeTime, xPeriod );
    }
}


void refresh_screen(uint8_t should_init, int current_test_num, int left_ping_dist, int right_ping_dist) {
    if (should_init) {
        CLI_transmit(CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    } else {
        CLI_transmit(SAVE_CURSOR_POS, strlen(SAVE_CURSOR_POS));
    }

    CLI_transmit(POSITION_CURSOR, strlen(POSITION_CURSOR));

    if (should_init) {
        CLI_transmit(MAKE_SCROLL, strlen(MAKE_SCROLL));
    }

		
		
		// update fixed area of window
		if (current_test_num == -1)
		{
			CLI_transmit(idle_status, strlen(idle_status));
		} else {
		
			char* current_test_num_str = convertIntegerToChar(current_test_num);
			CLI_transmit(current_test_num_str, strlen(current_test_num_str));
			free(current_test_num_str);
		}
		
		CLI_transmit(spaces, strlen(spaces));
		
		CLI_transmit(left_ping_label, strlen(left_ping_label));

		if (left_ping_dist == -1) {
			CLI_transmit("-1", strlen("-1"));
		} else {
			char* left_ping_dist_str = convertIntegerToChar(left_ping_dist);
			CLI_transmit(left_ping_dist_str, strlen(left_ping_dist_str));
			free(left_ping_dist_str);
		}
		
		CLI_transmit(spaces, strlen(spaces));
		CLI_transmit(right_ping_label, strlen(right_ping_label));

		if (right_ping_dist == -1) {
			CLI_transmit("-1", strlen("-1"));
		} else {
			char* right_ping_dist_str = convertIntegerToChar(right_ping_dist);
			CLI_transmit(right_ping_dist_str, strlen(right_ping_dist_str));
			
			free(right_ping_dist_str);
			
		}
		
    CLI_transmit("\n\n\n\n\n\n\n", strlen("\n\n\n\n\n\n\n"));

    if (!should_init) {
        CLI_transmit(RESTORE_CURSOR_POS, strlen(RESTORE_CURSOR_POS));
    }
}

void flush_buff(char p_data[], unsigned int size) {
    for (int i = 0; i < (int)size; i++) {
        p_data[i] = NULL;
    }
}
