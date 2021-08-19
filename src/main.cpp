#include "system.h"


int main() {
    system::clock_init_hse_pll_72MHz();
    system::enable_rcc_all();
    gpio::config_all();
    spi::config_for_nrf(SPI1);
    i2c::config_for_tea(I2C2);

    return 0;
}
