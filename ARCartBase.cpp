/**
 * @file ARCartBase.cpp
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Firmware for AR Cart base station interface to computer
 */

#include <Arduino.h>

#include "ARCartBase.h"
#include "base_network.h"

void start_base(void)
{
    Serial.begin(115200);
	network_init();

    xTaskCreatePinnedToCore(	 network_task,
	 				"net",
					 NETWORK_TASK_STACK_SIZE,
					 NULL,
					 NETWORK_TASK_PRIORITY,
					 NULL,
					 0);

    xTaskCreatePinnedToCore(	 serial_task,
	 				"serial",
					 SERIAL_TASK_STACK_SIZE,
					 NULL,
					 SERIAL_TASK_PRIORITY,
					 NULL,
					 0);

	vTaskStartScheduler();
}