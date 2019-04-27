/**
 * @file base_serial.cpp
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Firmware for AR Cart base station interface to computer
 */
#include <stdint.h>

#include "ARCartBase.h"
#include "base_serial.h"
#include "base_network.h"

// TODO later handle detecting messages in ISR and processing in queue
#define SERIAL_QUEUE_SIZE            (300)

static QueueHandle_t serial_queue;
static volatile serial_packet_t packet;

static bool process_byte(char data)
{
    static uint8_t step = 0;
    static uint8_t pos = 0;
    static uint8_t len = 0;
    static uint8_t chk = 0;

    switch (step)
    {
        case 0:
            if(data == START_BYTE_0)
            {
                step++;
            }
            break;

        case 1:
            if(data == START_BYTE_1)
            {
                step++;
            }
            break;
        
        case 2:
            packet.cls = data;
            step++;
            break;

        case 3:
            packet.id = data;
            step++;
            break;

        case 4:
            len = data;
            pos = 0;
            chk = 0;
            step++;
            break;

        case 5:
            if(pos < len-1)
            {
                packet.payload[pos] = data;
                chk += data;
            }
            else
            {
                packet.payload[pos] = data;
                chk += data;
                step++;
            }
            break;

        case 6:
            if(chk != data)
            {
                step = 0;
                pos = 0;
                chk = 0;
            }
            else
            {
                packet.checksum = data;
                step = 0;
                pos = 0;
                chk = 0;
                return true;
            }
            break;
    
        default:
            step = 0;
            pos = 0;
            chk = 0;
            break;
    }
    return false;
}

void IRAM_ATTR serialEvent()
{
    char data;
    while(Serial.available())
    {
        data = Serial.read();
        xQueueSendFromISR(serial_queue, &data, NULL);
    }
}

void serial_task(void * param)
{   
    // TODO need to use ESP32 uart functions to attach interrupt
    //Serial.setInterrupt(&serialEvent);

    serial_queue = xQueueCreate(SERIAL_QUEUE_SIZE, sizeof(char));

    char rec_byte;

    for(;;)
    {
        //xQueueReceive(serial_queue, &rec_byte, portMAX_DELAY);
        rec_byte = 'k';
        vTaskDelay(2); // tick rate is 100 hz
        if(Serial.available())
        {
            rec_byte = Serial.read();
        }

        controller_input_t cont;
        cont.left_stick_ud = 0.0;
        cont.left_stick_lr = 0.0;

        float stick_cmd = 0.5;

        switch (rec_byte)
        {
            case 'w':
                /* code */
                cont.left_stick_ud = stick_cmd;
                break;
            
            case 's':
                /* code */
                cont.left_stick_ud = -stick_cmd;
                break;

            case 'a':
                /* code */
                cont.left_stick_lr = -stick_cmd;
                break;

            case 'd':
                /* code */
                cont.left_stick_lr = stick_cmd;
                break;
        
            default:
                break;
        }

        // TODO for now just send something 
        if( cont.left_stick_lr != 0.0 ||  cont.left_stick_ud != 0.0)
        {
            Serial.println("Command received");
            send_command(&cont);
        }
    }
}