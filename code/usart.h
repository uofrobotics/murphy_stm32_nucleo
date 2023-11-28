#include <stdint.h>

#include "io_control.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "string.h"

#define DEL 127
#define INPUT_BUFF_LEN 40
#define CLEAR_SCREEN "\x1b[2J"
#define POSITION_CURSOR "\x1b[0;0H"
#define MAKE_SCROLL "\x1b[10;r"
#define SAVE_CURSOR_POS "\x1b[s"
#define RESTORE_CURSOR_POS "\x1b[u"

/// @brief open serial connection
/// @param void
void serial_open(void);

/// @brief close serial connection
/// @param void
void serial_close(void);

/// @brief send a byte within a limit to the timeout
/// @param b the byte to send
/// @param timeout max time to wait in ms
/// @return success is 0, failure is 1
int send_byte(uint8_t b, uint16_t retries);

/// @brief usart2 interrupt handler which is called when a char is received
/// @param void
void USART2_IRQHandler(void);
