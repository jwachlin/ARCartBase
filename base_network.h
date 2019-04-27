/**
 * @file base_network.h
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Network interface for AR Cart 
 */

#ifndef _BASE_NETWORK_H_
#define _BASE_NETWORK_H_

#include <stdint.h>

#include "ARCartBase.h"

typedef enum message_type_t{
    MSG_CONTROLLER_INPUT,
    MSG_TELEMETRY
};

/**
 * @brief Controller structure
 */
typedef struct __attribute__((packed)) controller_input_t
{
    uint32_t address;
	float right_stick_lr;       // Right joystick left right [-1.0,1.0]
	float right_stick_ud;       // Right joystick up down [-1.0,1.0]
	float left_stick_lr;        // Left joystick left right [-1.0,1.0]
	float left_stick_ud;        // Left joystick up down [-1.0,1.0]
} controller_input_t;

/**
 * @brief Speed information. Note no direction information possible
 */
typedef struct cart_speed_t
{
	float lfm;
	float lrm;
	float rfm;
	float rrm;
	float avg;
} cart_speed_t;

/**
 * @brief Cart telemetry
 */
typedef struct __attribute__((packed)) cart_telemetry_t
{
    uint32_t cart_number;
	motor_command_t motor_command;
    cart_speed_t speed;
} cart_telemetry_t;

bool network_init(void);

void send_command(controller_input_t * controller_input);

#endif