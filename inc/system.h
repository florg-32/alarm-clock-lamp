/**
 * Contains definitions for peripheral configuration
 * @author Florian Guggi
 * @date 16.08.2021
 */

#ifndef ALARM_CLOCK_LAMP_SYSTEM_H
#define ALARM_CLOCK_LAMP_SYSTEM_H

#include "stm32f1xx.h"

namespace system {
    void clock_init_hse_pll_72MHz();
    void enable_rcc_all();
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

    void config(GPIO_TypeDef *GPIO, uint8_t pin, cfg_t mode, cfg_t speed=SPEED_10MHZ);
    void config_all();

    template<bank_t bank, uint8_t pin>
    void exti_enable_falling_irq();
}

namespace spi {
    void config_for_nrf(SPI_TypeDef* SPI);
}

namespace i2c {
    void config_for_tea(I2C_TypeDef* I2C);
}



#endif //ALARM_CLOCK_LAMP_SYSTEM_H
