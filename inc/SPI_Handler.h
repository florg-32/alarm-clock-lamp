/**
 * @file SPI.h
 * This file contains the definition for an SPI utility object
 * @author Florian Guggi
 * @date 21.08.2021
 */
#ifndef ALARM_CLOCK_LAMP_SPI_HANDLER_H
#define ALARM_CLOCK_LAMP_SPI_HANDLER_H

#include "stm32f1xx.h"

class SPI_Handler {
public:
    /**
     * Configures the SPI and DMA peripherals for their usage
     */
    virtual void config_periph() = 0;

    /**
     * Starts a write only SPI transaction
     * @param wrdata Array of bytes to write
     * @param wrdata_length Number of bytes to write
     * @param blocking Whether the function should busy wait until the transaction is complete
     */
    virtual void write_transaction(const uint8_t *wrdata, uint8_t wrdata_length, bool blocking) = 0;

    /**
     * Starts a SPI transaction with the provided write data, and places the received data into the rx buffer.\n
     * Make sure to write extra 0x00 for each byte to receive (keeps the CLK signal going)
     * @param wrdata Array of bytes to write
     * @param wrdata_length Number of bytes to write
     * @param rxbuffer Array of bytes to read
     * @param rxbuffer_length Number of bytes to read
     */
    virtual void read_transaction(const uint8_t *wrdata, uint8_t wrdata_length, const uint8_t *rxbuffer, uint8_t rxbuffer_length, bool blocking) = 0;

    /**
     * Returns the wether a SPI transaction is currently ongoing
     */
    virtual bool is_busy() = 0;
};

#endif //ALARM_CLOCK_LAMP_SPI_HANDLER_H
