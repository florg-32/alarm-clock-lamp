/**
 * @file system.cpp
 * Contains project specific function for peripheral configuration
 * @author Florian Guggi
 * @date 16.08.2021
 */

#include "peripherals.h"

namespace system {

    /**
     * Sets the RCC enable bits for the projects peripherals
     */
    void enable_all_periphs() {
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
    }

    /**
     * Configures all gpios needed for the project
     * @see README
     */
    void config_gpios() {
        gpio::config(GPIOA, 2, gpio::OUT_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 3, gpio::IN_PUSHPULL);
        gpio::config(GPIOA, 4, gpio::OUT_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 5, gpio::AF_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 6, gpio::AF_OD, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 7, gpio::AF_OD, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 8, gpio::AF_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 9, gpio::AF_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 11, gpio::IN_PUSHPULL);
        gpio::config(GPIOA, 12, gpio::IN_PUSHPULL);
        gpio::config(GPIOA, 15, gpio::OUT_PUSHPULL);
        gpio::config(GPIOB, 10, gpio::AF_OD, gpio::SPEED_50MHZ);
        gpio::config(GPIOB, 11, gpio::AF_OD, gpio::SPEED_50MHZ);
        gpio::exti_enable_falling_irq<gpio::BANK_A, 3>();
        gpio::exti_enable_falling_irq<gpio::BANK_A, 11>();
        gpio::exti_enable_falling_irq<gpio::BANK_A, 12>();
    }

}