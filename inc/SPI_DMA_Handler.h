/**
 * @file SPI.h
 * This file contains the definition for an SPI utility object
 * @author Florian Guggi
 * @date 21.08.2021
 */
#ifndef ALARM_CLOCK_LAMP_SPI_DMA_HANDLER_H
#define ALARM_CLOCK_LAMP_SPI_DMA_HANDLER_H

#include "stm32f1xx.h"

class SPI_DMA_Handler {
public:
    SPI_TypeDef *SPI;
    DMA_Channel_TypeDef *DMA_Ch_TX, *DMA_Ch_RX;
    GPIO_TypeDef *GPIO_CS;
    uint8_t pin_cs;

    SPI_DMA_Handler() = default;

    void set_periphs(SPI_TypeDef *SPI_, DMA_Channel_TypeDef *DMA_TX, DMA_Channel_TypeDef *DMA_RX, GPIO_TypeDef *GPIO, uint8_t pin) {
        SPI = SPI_; DMA_Ch_TX = DMA_TX; DMA_Ch_RX = DMA_RX;
        GPIO_CS = GPIO; pin_cs = pin;
    }

    /**
     * Configures the SPI and DMA peripherals for their usage
     * @param rx_dma_ena enable DMA requests on RX
     * @param tx_dma_ena enable DMA requests for TX
     */
    void config_periph(bool rx_dma_ena=true, bool tx_dma_ena=true) const {
        SPI->CR2 |= (rx_dma_ena << SPI_CR2_RXDMAEN_Pos) | (tx_dma_ena << SPI_CR2_TXDMAEN_Pos);
        DMA_Ch_TX->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
        DMA_Ch_TX->CPAR = (uint32_t) &SPI->DR;
        DMA_Ch_RX->CCR |= DMA_CCR_MINC;
        DMA_Ch_RX->CPAR = (uint32_t) &SPI->DR;
    }

    /**
     * Starts a write only SPI transaction
     * @param wrdata Array of bytes to write
     * @param wrdata_length Number of bytes to write
     * @param blocking Whether the function should busy wait until the transaction is complete
     */
    void write_transaction(const uint8_t *wrdata, uint8_t wrdata_length, bool blocking=false) const {
        gpio::set(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_RX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_TX->CMAR = (uint32_t) wrdata;
        DMA_Ch_TX->CNDTR = wrdata_length;
        gpio::reset(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR |= DMA_CCR_EN;
        if (blocking)
            while(is_busy() || DMA_Ch_TX->CNDTR);
    }

    /**
     * Starts a SPI transaction with the provided write data, and places the received data into the rx buffer.\n
     * Make sure to write extra 0x00 for each byte to receive (keeps the CLK signal going)
     * @param wrdata Array of bytes to write
     * @param wrdata_length Number of bytes to write
     * @param rxbuffer Array of bytes to read
     * @param rxbuffer_length Number of bytes to read
     */
    void read_transaction(uint8_t *wrdata, uint8_t wrdata_length, uint8_t *rxbuffer, uint8_t rxbuffer_length, bool blocking=true) const {
        gpio::set(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_RX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_TX->CMAR = (uint32_t) wrdata;
        DMA_Ch_TX->CNDTR = wrdata_length;
        DMA_Ch_RX->CMAR = (uint32_t) rxbuffer;
        DMA_Ch_RX->CNDTR = rxbuffer_length;
        gpio::reset(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR |= DMA_CCR_EN;
        DMA_Ch_RX->CCR |= DMA_CCR_EN;
        if (blocking)
            while(is_busy() || DMA_Ch_TX->CNDTR);
    }

    bool is_busy() const {
        return (SPI->SR & SPI_SR_BSY);
    }
};

#endif //ALARM_CLOCK_LAMP_SPI_DMA_HANDLER_H
