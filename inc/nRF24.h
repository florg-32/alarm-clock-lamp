/**
 * @file nRF24.h
 * A wrapper class for easy communication with the nRF24l01
 * @author Florian Guggi
 * @date 26.08.21
 */

#ifndef ALARM_CLOCK_LAMP_NRF24_H
#define ALARM_CLOCK_LAMP_NRF24_H

#include "SPI_DMA_Handler.h"

class nRF24 : public SPI_DMA_Handler {
public:
    enum addr_width_t : uint8_t {
        BYTE3 = 1,
        BYTE4 = 2,
        BYTE5 = 3
    };

    /**
     * Writes to the SETUP_AW register
     */
    void set_addr_width(addr_width_t width) {
        uint8_t data[] = {0x23, width};
        write_transaction(data, 2, true);
    }

    /**
     * Sets its own read address, should conform to the set addr_width
     * @param addr The address to set, the first byte will be ignored
     * @param addr_length Length of the address + 1!
     * @param pipe which data pipe should be configured
     * @warning first byte of address will be ignored!
     */
    void set_rx_addr(uint8_t *addr, uint8_t addr_length, uint8_t pipe) {
        addr[0] = 0x2a + pipe;
        write_transaction(addr, addr_length, true);
    }

    /**
     * Sets the tx address, should conform to the set addr_width
     * @param addr The address to set, the first byte will be ignored
     * @param addr_length Length of address + 1!
     * @param pipe which data pipe should be configured
     * @warning first byte of address will be ignored!
     */
    void set_tx_addr(uint8_t *addr, uint8_t addr_length) {
        addr[0] = 0x30;
        write_transaction(addr, addr_length, true);
    }

    /**
     * Writes the payload length in to the RX_PW registers
     * @param length payload length
     * @param pipe The pipe to write to
     */
    void set_payload_length(uint8_t length, uint8_t pipe) {
        uint8_t data[] = {static_cast<uint8_t>(0x31+pipe), length};
        write_transaction(data, 2, true);
    }

    /**
     * Write to the DYNPD register. Dont forget to enable the feature as well.
     * @param pipes The pipes to enable e.g. 1 << 4
     */
    void set_dynamic_payload_length(uint8_t pipes) {
        uint8_t data[] = {0x3c, pipes};
        write_transaction(data, 2, true);
    }

    enum feature_t : uint8_t {
        EN_DPL = 4,
        EN_ACK_PAY = 2,
        EN_DYN_ACK = 1
    };

    /**
     * Writes to the FEATURE register.
     * @param feat see feature_t
     */
    void set_features(uint8_t feat) {
        uint8_t data[] = {0x3d, feat};
        write_transaction(data, 2, true);
    }

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
     * Writes to the CONFIG register
     * @param cfg see cfg_t
     */
    void set_config(uint8_t cfg) {
        uint8_t data[] = {0x20, cfg};
        write_transaction(data, 2, true);
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
        write_transaction(payload, payload_length, blocking);
    }

    /**
     * Reads the nRFs RX FIFO into the provided buffer. First byte will be status register
     * @param buffer A byte array
     * @param buffer_length The buffers size/bytes to read
     * @param blocking Whether the read should busy wait until complete
     */
    void read_payload(uint8_t *buffer, uint8_t buffer_length, bool blocking=true) {
        buffer[0] = 0x61;
        read_transaction(buffer, buffer_length, buffer, buffer_length, blocking);
    }
};

#endif //ALARM_CLOCK_LAMP_NRF24_H
