/**
 * @file nRF24.h
 * A wrapper class for easy communication with the nRF24l01
 * @author Florian Guggi
 * @date 26.08.21
 */

#ifndef ALARM_CLOCK_LAMP_NRF24_H
#define ALARM_CLOCK_LAMP_NRF24_H

#include "SPI_Handler.h"

class nRF24 {
    SPI_Handler *spi_handler;
public:
    void set_spi_handler(SPI_Handler *handler) {
        spi_handler = handler;
    }

    enum regs_t : uint8_t {
        CONFIG,
        EN_AA,
        EN_RXADDR,
        SETUP_AW,
        SETUP_RETR,
        RF_CH,
        RF_SETUP,
        STATUS,
        OBSERVE_TX,
        RPD,
        RX_ADDR_P0,
        RX_ADDR_P1,
        RX_ADDR_P2,
        RX_ADDR_P3,
        RX_ADDR_P4,
        RX_ADDR_P5,
        TX_ADDR,
        RX_PW_P0,
        RX_PW_P1,
        RX_PW_P2,
        RX_PW_P3,
        RX_PW_P4,
        RX_PW_P5,
        FIFO_STATUS,
        DYNPD = 0x1c,
        FEATURE = 0x1d
    };

    enum addr_width_t : uint8_t {
        BYTE3 = 1,
        BYTE4 = 2,
        BYTE5 = 3
    };

    enum feature_t : uint8_t {
        EN_DPL = 4,
        EN_ACK_PAY = 2,
        EN_DYN_ACK = 1
    };

    enum cfg_t : uint8_t {
        PRIM_RX = 1,
        PWR_UP = 2,
        CRCO = 4,
        EN_CRC = 8,
        MASK_MAX_RT = 16,
        MASK_TX_DS = 32,
        MASK_RX_DR = 64
    };

    /**
     * Powers down the nRF and disables any pipes/features...
     */
    void reset() {
        write_reg(CONFIG, 0x00);
        write_reg(EN_AA, 0x00);
        write_reg(EN_RXADDR, 0x00);
        write_reg(SETUP_AW, 0x03);
        write_reg(SETUP_RETR, 0x00);
        write_reg(RF_CH, 0x00);
        write_reg(RF_SETUP, 0x0e);
        flush_rx();
        flush_tx();
        write_reg(STATUS, 0xff);
        uint8_t addr[] = {0x00, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7};
        write_multireg(RX_ADDR_P0, addr, 6);
        write_multireg(TX_ADDR, addr, 6);
        for (uint8_t &b : addr) {
            b = 0xc2;
        }
        write_multireg(RX_ADDR_P1, addr, 6);
        for (uint8_t i = RX_PW_P0; i <= RX_PW_P5; i++) {
            write_reg((regs_t) i, 0x00);
        }
        write_reg(DYNPD, 0x00);
        write_reg(FEATURE, 0x00);
    }

    /**
     * A blocking write to a nRF register
     * @param reg The register to write to
     * @param byte data to write
     */
    void write_reg(regs_t reg, uint8_t byte) {
        uint8_t buffer[] = {static_cast<uint8_t>(reg | 1 << 5), byte};
        spi_handler->write_transaction(buffer, 2, true);
    }

    /**
     * A blocking write of multiple bytes to a nRF register
     * @param reg The register to write to
     * @param bytes An array of bytes to write, first byte will be overwritten!
     * @param length length of bytes
     */
    void write_multireg(regs_t reg, uint8_t *bytes, uint8_t length) {
        bytes[0] = reg | 1 << 5;
        spi_handler->write_transaction(bytes, length, true);
    }

    /**
     * A blocking read from a nRF register
     * @param reg The register to read from
     * @return read data
     */
    uint8_t read_reg(regs_t reg) {
        uint8_t buffer[] = {reg, 0x00};
        spi_handler->read_transaction(buffer, 2, buffer, 2, true);
        return buffer[1];
    }

    /**
     * Flushes the nRF TX FIFO
     */
    void flush_tx() {
        uint8_t buffer = 0xe1;
        spi_handler->write_transaction(&buffer, 1, true);
    }

    /**
     * Flushes the nRF RX FIFO
     */
    void flush_rx() {
        uint8_t buffer = 0xe2;
        spi_handler->write_transaction(&buffer, 1, true);
    }

    /**
     * Writes the provided data into the nRFs TX FIFO
     * @param payload The bytes to be sent, first will be ignored
     * @param payload_length Length of the payload + 1!
     * @param no_ack Sets the ShockBurst NO_ACK bit with this payload
     * @param blocking Whether the write should busy wait until complete
     */
    void write_payload(uint8_t *payload, uint8_t payload_length, bool no_ack=false, bool blocking=true) {
        payload[0] = no_ack ? 0xb0 : 0xa0;
        spi_handler->write_transaction(payload, payload_length, blocking);
    }

    /**
     * Writes the provided data into the nRFs ACK payload FIFO
     * @param payload The bytes to be sent, first will be ignored
     * @param payload_length Length of the payload + 1!
     * @param pipe Which pipe should send the ack payload
     * @param blocking Whether the write should busy wait until complete
     */
    void write_ack_payload(uint8_t *payload, uint8_t payload_length, uint8_t pipe, bool blocking=true) {
        payload[0] = 0xa8 | pipe;
        spi_handler->write_transaction(payload, payload_length, blocking);
    }
    /**
     * Reads the number of bytes waiting at the top of the RX FIFO
     * @return Number of bytes in RX payload pipe
     */
    uint8_t get_payload_length() {
        uint8_t buffer[] = {0x60, 0x00};
        spi_handler->read_transaction(buffer, 2, buffer, 2, true);
        return buffer[1];
    }

    /**
     * Reads the nRFs RX FIFO into the provided buffer. First byte will be status register, use in RX mode
     * @param buffer A byte array
     * @param buffer_length The buffers size/bytes to read
     * @param blocking Whether the read should busy wait until complete
     */
    void read_payload(uint8_t *buffer, uint8_t buffer_length, bool blocking=true) {
        buffer[0] = 0x61;
        spi_handler->read_transaction(buffer, buffer_length, buffer, buffer_length, blocking);
    }
};

#endif //ALARM_CLOCK_LAMP_NRF24_H
