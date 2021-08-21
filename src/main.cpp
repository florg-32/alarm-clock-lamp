#include "peripherals.h"
#include "system.h"


int main() {
    rcc::clock_init_hse_pll_72MHz();
    system::enable_all_periphs();
    system::config_gpios();
    spi::config_for_nrf(SPI1);
    i2c::config_for_tea(I2C2);

    return 0;
}
