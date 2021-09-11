#include "peripherals.h"
#include "system.h"
#include "STMF1_SPI_Handler.h"
#include "nRF24.h"

STMF1_SPI_Handler nrf_spi_handler;
nRF24 nRF;
uint8_t rx_buffer[32];

int main() {
    rcc::clock_init_hse_pll_72MHz();
    system::enable_all_periphs();
    system::config_gpios();
    system::config_for_nrf(SPI1);
    system::config_for_tea(I2C2);
    nrf_spi_handler.set_periphs(SPI1, DMA1_Channel3, DMA1_Channel2, GPIOA, 4);
    nrf_spi_handler.config_periph();
    nRF.set_spi_handler(&nrf_spi_handler);

    NVIC_EnableIRQ(EXTI3_IRQn);
    gpio::config(GPIOC, 13, gpio::OUT_PUSHPULL);
    gpio::set(GPIOC, 13);

    tim::blocking_delay(TIM2, 36000, 100); // Wait 100ms for nRF poweron reset
    nRF.reset();
    nRF.write_reg(nRF24::EN_AA, 0x01);
    nRF.write_reg(nRF24::EN_RXADDR, 0x01); // Enable pipe 0
    nRF.write_reg(nRF24::SETUP_RETR, 0x35); // 5 retries, 1ms delay
    nRF.write_reg(nRF24::RF_CH, 0x14); // Channel 20
    nRF.write_reg(nRF24::RF_SETUP, 0x07); // 0dBm, 1Mbps
    nRF.write_reg(nRF24::RX_PW_P0, 32); // 32 byte packet
    nRF.write_reg(nRF24::FEATURE, nRF24::EN_DYN_ACK);
    nRF.write_reg(nRF24::CONFIG, 0x0f); // 2 byte CRC, PRX, power up
    tim::blocking_delay(TIM2, 36000, 2); // Wait 2ms for power up
    gpio::set(GPIOA, 2);
    tim::blocking_delay(TIM2, 36000, 2);
    //uint8_t payload[] = {0x00, 0x11, 0x22, 0x33, 0xaa, 0xbb, 0xcc};
    //nRF_handler.write_payload(payload, 7);

    while (true) {
        asm("wfi");
    }
    return 0;
}

extern "C" {
[[maybe_unused]]
void EXTI3_IRQHandler() {
    GPIOC->ODR ^= 1 << 13;
    nRF.write_reg(nRF24::STATUS, 0xff);
    nRF.read_payload(rx_buffer, nRF.get_payload_length());
    EXTI->PR = EXTI_PR_PIF3;
}
}