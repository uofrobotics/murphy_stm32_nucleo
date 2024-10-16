#include "usart.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t buff;

void serial_open(void) {
    NVIC->ISER[1] = 1 << 6;  // enable usart2 in nvic
    // PA2 AFIO push/pull max speed 50MHz, USART TX
    GPIOA->CRL |= GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1 | GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;

    // PA3 floating input, USART RX (default port config)

    USART2->CR1 |= USART_CR1_UE;

    // default reset is: for data bits =8, no parity, 1 stop bit, don't need to
    // configure here

    // 36Mhz clock might not matter??, see page 799 and table 192 for 115.2 kbps
    // is 19.5
    USART2->BRR =
        (0x8 & USART_BRR_DIV_Fraction) | ((0x13 << 4) & USART_BRR_DIV_Mantissa);

    // enable interrupts when a char is received
    USART2->CR1 |= USART_CR1_RXNEIE;

    // enable transmit and receive
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // reenable usart
    USART2->CR1 |= USART_CR1_UE;
}
void serial_close(void) {
    // TODO: probably don't need to clear config, maybe? just turn it off for
    // now
    USART2->CR1 &= ~USART_CR1_UE;
}

int send_byte(uint8_t b, uint16_t timeout) {
    if (timeout <= 1) {
        timeout = 2;
    }  // workaround for inputs of 0 and 1

    TIM2->PSC =
        24000 - 1;  // Set counter clk to 1 KHz, function has 1 ms resolution
    TIM2->ARR = timeout - 1;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CR1 |= TIM_CR1_DIR | TIM_CR1_OPM | TIM_CR1_CEN;  // start/enable timer
    while (
        // while the timer has not yet expired
        ((TIM2->SR & TIM_SR_UIF) != TIM_SR_UIF)) {  // while not reached timeout

        // if Transmit data register (buffer) empty
        if ((USART2->SR & USART_SR_TXE) == USART_SR_TXE) {
            // send data
            USART2->DR = b;
            return 0;
        }
    }
    TIM2->CR1 &= ~TIM_CR1_CEN;  // turn off timer.
    return 1;                   // if got to this point, timer expired
}

void USART2_IRQHandler(void) {
    // because we only enabled the RXNE == 1 interrupt, we don't need to check
    // why we ended up here

    uint8_t received_char = (uint8_t)USART2->DR;
    xQueueSendToFrontFromISR(buff, &received_char, NULL);
}
