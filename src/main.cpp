#include "peripherals.h"
#include "system.h"
#include "nRF24.h"

nRF24 nRF_handler;

int main() {
    rcc::clock_init_hse_pll_72MHz();
    system::enable_all_periphs();
    system::config_gpios();
    system::config_for_nrf(SPI1);
    system::config_for_tea(I2C2);
    nRF_handler.set_periphs(SPI1, DMA1_Channel3, DMA1_Channel2, GPIOA, 4);
    nRF_handler.config_periph();

    tim::blocking_delay(TIM2, 36000, 100); // Wait 100ms for nRF poweron reset
    nRF_handler.set_addr_width(nRF24::BYTE3);
    uint8_t rx_addr[] = {0x00, 0xde, 0xda, 0xd0};
    nRF_handler.set_rx_addr(rx_addr, 4, 0);
    nRF_handler.set_features(nRF24::EN_ACK_PAY | nRF24::EN_DPL | nRF24::EN_DYN_ACK);
    nRF_handler.set_config(nRF24::PRIM_RX | nRF24::PWR_UP);
    gpio::set(GPIOA, 2);
    tim::blocking_delay(TIM2, 36000, 2); // Wait 2ms for RX settling

    while (true);
    return 0;
}
