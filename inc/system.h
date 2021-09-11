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
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN | RCC_APB1ENR_TIM2EN;
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
    }

    /**
     * Configures all gpios needed for the project
     * @see README
     */
    void config_gpios() {
        gpio::reset(GPIOA, 2);
        gpio::config(GPIOA, 2, gpio::OUT_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 3, gpio::IN_PUSHPULL);
        gpio::set(GPIOA, 3);
        gpio::set(GPIOA, 4);
        gpio::config(GPIOA, 4, gpio::OUT_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 5, gpio::AF_PUSHPULL, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 6, gpio::AF_OD, gpio::SPEED_50MHZ);
        gpio::config(GPIOA, 7, gpio::AF_PUSHPULL, gpio::SPEED_50MHZ);
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

    /**
     * Configures the SPI for use with the NRF24L01\n
     * Baudrate: 72/64MHz, LSB first
     * @param SPI typedef e.g. SPI1
     */
    void config_for_nrf(SPI_TypeDef *SPI) {
        SPI->CR1 = SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    }

    /**
     * Configures the I2C for use with the TEA5767\n
     * CLK low time 722ns, high time 1444ns; max rise time 300ns
     * @param I2C typedef e.g. I2C2
     */
    void config_for_tea(I2C_TypeDef *I2C) {
        MODIFY_REG(I2C->CR2, I2C_CR2_FREQ, 36); // APB1 Freq = 36MHz
        MODIFY_REG(I2C->CCR, I2C_CCR_CCR, I2C_CCR_FS | 26); // CLK low time = 26/36MHz ~ 722ns
        I2C->TRISE = 12UL; // Maximum rise time = 300ns/36MHz ~ 11
        I2C->CR1 |= I2C_CR1_PE;
    }

}