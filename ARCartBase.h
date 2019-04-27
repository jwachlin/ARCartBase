/**
 * @file ARCartBase.h
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Firmware for AR Cart base station interface to computer
 */

#ifndef _AR_CART_BASE_H_
#define _AR_CART_BASE_H_

#include <stdint.h>
#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// Task setup
#define SERIAL_TASK_STACK_SIZE	    (1000)
#define NETWORK_TASK_STACK_SIZE		(1000)

#define SERIAL_TASK_PRIORITY		(2)
#define NETWORK_TASK_PRIORITY		(2)

/**
 * @brief Command information
 */
typedef struct motor_command_t
{
	float lfm;
	float lrm;
	float rfm;
	float rrm;
} motor_command_t;

void start_base(void);

void serial_task(void * param);
void network_task(void * param);

#endif