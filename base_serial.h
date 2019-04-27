/**
 * @file base_serial.h
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Firmware for AR Cart base station interface to computer
 */

#ifndef _BASE_SERIAL_H_
#define _BASE_SERIAL_H_

#define MAX_PAYLOAD_SIZE        (128)
#define START_BYTE_0            (0x77)
#define START_BYTE_1            (0x12)

/**
 * @brief Serial packets
 */
typedef struct serial_packet_t
{
    uint8_t cls;
    uint8_t id;
    uint8_t length;
	uint8_t payload[MAX_PAYLOAD_SIZE];
    uint8_t checksum;
} serial_packet_t;

#endif