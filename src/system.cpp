/**
* Contains functions for peripheral configuration and access
* @author Florian Guggi
* @date 16.08.2021
*/

#include "system.h"

/**
 * Enables and configures the clocks and PLL to a 72MHz SYSCLK.\n
 * APB1: 36MHz, ADC: 9MHz
 * @warning Must only be called after reset!
 */
void system::clock_init_hse_pll_72MHz() {
    RCC->CR |= RCC_CR_HSEON; // Turn on HSE
    RCC->CFGR |= RCC_CFGR_HPRE_DIV2 | RCC_CFGR_ADCPRE_DIV8 | RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC; // Dividers and PLL
    FLASH->ACR |= FLASH_ACR_LATENCY_1; // Set 2 wait states for FLASH access
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait for HSE to stabilize
    RCC->CR |= RCC_CR_PLLON; // Turn on PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL; // Switch SYSCLK source to PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait for PLL to stabilize
}

/**
 * Sets the RCC enable bits for the projects peripherals
 * @warning specific to the alarm-clock-lamp project
 */
void system::enable_rcc_all() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
}

/**
 * Configures a pin as provided
 * @param GPIO typedef e.g. GPIOB
 * @param pin to be set e.g. 6
 * @param mode see gpio::cfg_t
 * @param speed see gpio::cfg_t, ignored if input mode
 */
template<GPIO_TypeDef* GPIO, uint8_t pin>
void gpio::config(gpio::cfg_t mode, gpio::cfg_t speed) {
    if constexpr(pin < 8) {
        MODIFY_REG(GPIO->CRL, 0xf << pin, (mode | speed) << pin);
    }
    else {
        MODIFY_REG(GPIO->CRH, 0xf << pin, (mode | speed) << pin);
    }
}

/**
 * Enables an exti irq for falling edges on the provided pin
 * @param bank on which the pin is
 * @param pin used pin e.g. 6
 */
template<gpio::bank_t bank, uint8_t pin>
void gpio::exti_enable_falling_irq() {
    MODIFY_REG(AFIO->EXTICR[pin / 4], 0xf << (pin % 4), bank << (pin % 4));
    EXTI->IMR |= 0x1 << pin;
    EXTI->FTSR |= 0x1 << pin;
}

/**
 * Configures the SPI for use with the NRF24L01\n
 * Baudrate: 72/64MHz, LSB first
 * @param SPI typedef e.g. SPI1
 */
void spi::config_for_nrf(SPI_TypeDef *SPI) {
    SPI->CR1 = SPI_CR1_LSBFIRST | SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_0 | SPI_CR1_MSTR;
}

/**
 * Configures the I2C for use with the TEA5767\n
 * CLK low time 722ns, high time 1444ns; max rise time 300ns
 * @param I2C typedef e.g. I2C2
 */
void i2c::config_for_tea(I2C_TypeDef *I2C) {
    MODIFY_REG(I2C->CR2, I2C_CR2_FREQ, 36); // APB1 Freq = 36MHz
    MODIFY_REG(I2C->CCR, I2C_CCR_CCR, I2C_CCR_FS | 26); // CLK low time = 26/36MHz ~ 722ns
    I2C->TRISE = 12UL; // Maximum rise time = 300ns/36MHz ~ 11
    I2C->CR1 |= I2C_CR1_PE;
}
