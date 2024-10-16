#include "stm32f10x.h"

/// @brief blocking sleep in miliseconds
/// @param wait_for miliseconds to sleep
void dirty_delay(uint32_t microseconds);

/// @brief get the state of the led
/// @param void
/// @return the state of the led, 0 is off, 1 is on
uint8_t get_led_state(void);

/// @brief enable all clocks for peripherals on stm32
/// @param void
void turn_on_clocks(void);

/// @brief configure io as need for blinking led and enabling delay clocks
/// @param void
void configure_io(void);

/// @brief turn the green on board on or off
/// @param on_or_off, zero is off, one is on
void control_green_onboard_led(unsigned int on_or_off);

/// @brief forever blink led to indicate rtos errors
/// @param void
void forever_blink_led(void);
