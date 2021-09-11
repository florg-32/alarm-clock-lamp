/**
 * @file STMF1_SPI_Handler.h
 * An implementation of the SPI Handler for the STM32F1
 * @author Florian Guggi
 * @date 04.09.2021
 */

#ifndef ALARM_CLOCK_LAMP_STMF1_SPI_HANDLER_H
#define ALARM_CLOCK_LAMP_STMF1_SPI_HANDLER_H

#include "SPI_Handler.h"
#include "stm32f1xx.h"

class STMF1_SPI_Handler : public SPI_Handler {

private:
    SPI_TypeDef *SPI{};
    DMA_Channel_TypeDef *DMA_Ch_TX{}, *DMA_Ch_RX{};
    GPIO_TypeDef *GPIO_CS{};
    uint8_t pin_cs{};

public:

    STMF1_SPI_Handler() = default;

    void set_periphs(SPI_TypeDef *SPI_, DMA_Channel_TypeDef *DMA_TX, DMA_Channel_TypeDef *DMA_RX, GPIO_TypeDef *GPIO, uint8_t pin) {
        SPI = SPI_; DMA_Ch_TX = DMA_TX; DMA_Ch_RX = DMA_RX;
        GPIO_CS = GPIO; pin_cs = pin;
    }

    void config_periph() override {
        SPI->CR2 |= SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;
        DMA_Ch_TX->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
        DMA_Ch_TX->CPAR = (uint32_t) &SPI->DR;
        DMA_Ch_RX->CCR |= DMA_CCR_MINC;
        DMA_Ch_RX->CPAR = (uint32_t) &SPI->DR;
    }

    void write_transaction(const uint8_t *wrdata, uint8_t wrdata_length, bool blocking) override {
        gpio::set(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_RX->CCR &= ~DMA_CCR_EN;
        DMA_Ch_TX->CMAR = (uint32_t) wrdata;
        DMA_Ch_TX->CNDTR = wrdata_length;
        gpio::reset(GPIO_CS, pin_cs);
        DMA_Ch_TX->CCR |= DMA_CCR_EN;
        if (blocking) {
            while(is_busy() || DMA_Ch_TX->CNDTR);
            gpio::set(GPIO_CS, pin_cs);
            (void) SPI->DR;
        }
    }

    void read_transaction(const uint8_t *wrdata, uint8_t wrdata_length, const uint8_t *rxbuffer, uint8_t rxbuffer_length, bool blocking) override {
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
        if (blocking || DMA_Ch_RX->CNDTR) {
            while(is_busy());
            gpio::set(GPIO_CS, pin_cs);
        }
    }

    bool is_busy() override {
        return (SPI->SR & SPI_SR_BSY);
    }
};

#endif //ALARM_CLOCK_LAMP_STMF1_SPI_HANDLER_H
