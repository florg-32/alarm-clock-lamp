/**
 * @file peripherals.h
 * Contains functions for general peripheral access
 * @author Florian Guggi
 * @date 16.08.2021
 */

#ifndef ALARM_CLOCK_LAMP_PERIPHERALS_H
#define ALARM_CLOCK_LAMP_PERIPHERALS_H

#include "stm32f1xx.h"

namespace rcc {
    /**
     * Enables and configures the clocks and PLL to a 72MHz SYSCLK.\n
     * APB1: 36MHz, ADC: 9MHz
     * @warning Must only be called after reset!
     */
    void clock_init_hse_pll_72MHz() {
        RCC->CR |= RCC_CR_HSEON; // Turn on HSE
        RCC->CFGR |= RCC_CFGR_HPRE_DIV2 | RCC_CFGR_ADCPRE_DIV8 | RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC; // Dividers and PLL
        FLASH->ACR |= FLASH_ACR_LATENCY_1; // Set 2 wait states for FLASH access
        while (!(RCC->CR & RCC_CR_HSERDY)); // Wait for HSE to stabilize
        RCC->CR |= RCC_CR_PLLON; // Turn on PLL
        RCC->CFGR |= RCC_CFGR_SW_PLL; // Switch SYSCLK source to PLL
        while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait for PLL to stabilize
    }
}

namespace gpio {
    enum cfg_t {
        IN_ANALOG = 0,
        IN_FLOATING = 1,
        IN_PUSHPULL = 2,
        OUT_PUSHPULL = 0,
        OUT_OD = 1,
        AF_PUSHPULL = 2,
        AF_OD = 3,
        SPEED_2MHZ = 2,
        SPEED_10MHZ = 1,
        SPEED_50MHZ = 3
    };

    enum bank_t {
        BANK_A,
        BANK_B,
        BANK_C,
    };

    /**
     * Configures a pin as provided
     * @param GPIO typedef e.g. GPIOB
     * @param pin to be set e.g. 6
     * @param mode see gpio::cfg_t
     * @param speed see gpio::cfg_t, ignored if input mode
     */
    void config(GPIO_TypeDef *GPIO, uint8_t pin, gpio::cfg_t mode, gpio::cfg_t speed=SPEED_10MHZ) {
        if (pin < 8) {
            MODIFY_REG(GPIO->CRL, 0xf << pin, (mode | speed) << pin);
        } else {
            MODIFY_REG(GPIO->CRH, 0xf << pin, (mode | speed) << pin);
        }
    }

    /**
     * Enables an exti irq for falling edges on the provided pin
     * @param bank on which the pin is
     * @param pin used pin e.g. 6
     */
    template<gpio::bank_t bank, uint8_t pin>
    void exti_enable_falling_irq() {
        MODIFY_REG(AFIO->EXTICR[pin / 4], 0xf << (pin % 4), bank << (pin % 4));
        EXTI->IMR |= 0x1 << pin;
        EXTI->FTSR |= 0x1 << pin;
    }
}

namespace tim {
    void enable(TIM_TypeDef *TIM) {
        TIM->CR1 |= TIM_CR1_CEN;
    }

    void disable(TIM_TypeDef *TIM) {
        TIM->CR1 &= ~TIM_CR1_CEN;
    }

    void set_period(TIM_TypeDef *TIM, uint16_t period) {
        TIM->ARR = period;
    }

    void set_prescaler(TIM_TypeDef *TIM, uint16_t prescaler) {
        TIM->PSC = prescaler;
    }

    void generate_update(TIM_TypeDef *TIM) {
        TIM->EGR |= TIM_EGR_UG;
    }

    /**
     * Configures the timer in oneshot mode and enable the update irq
     * @param prescaler clock divisor + 1
     */
    void config_oneshot_irq(TIM_TypeDef *TIM, uint16_t prescaler, uint16_t period) {
        set_period(TIM, period);
        set_prescaler(TIM, prescaler);
        generate_update(TIM);
        TIM->CR1 |= TIM_CR1_OPM;
        TIM->DIER |= TIM_DIER_UIE;
    }
}

namespace spi {
    /**
     * Configures the SPI for use with the NRF24L01\n
     * Baudrate: 72/64MHz, LSB first
     * @param SPI typedef e.g. SPI1
     */
    void config_for_nrf(SPI_TypeDef *SPI) {
        SPI->CR1 = SPI_CR1_LSBFIRST | SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_0 | SPI_CR1_MSTR;
    }}

namespace i2c {
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


#endif //ALARM_CLOCK_LAMP_PERIPHERALS_H
