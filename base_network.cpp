/**
 * @file base_network.cpp
 * @author Jacob Wachlin
 * @date Feb 2019
 * @brief Network interface for AR Cart 
 */

#include <WiFi.h>
#include <esp_now.h>
#include <stdint.h>
#include <string.h>

#include "ARCartBase.h"
#include "freertos/queue.h"

#include "base_network.h"

#define WIFI_CHANNEL                        (1)
#define TELEMETRY_QUEUE_SIZE                (20)

static QueueHandle_t telemetry_command_queue;

static uint8_t remoteMac[] = {0x60,0xAA,0xFC,0x3F,0xC8,0x23};
static uint8_t broadcast_mac[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static void handle_error(esp_err_t err)
{
    switch (err)
    {
        case ESP_ERR_ESPNOW_NOT_INIT:
            Serial.println("Not init");
            break;
        
        case ESP_ERR_ESPNOW_ARG:
            Serial.println("Argument invalid");
            break;

        case ESP_ERR_ESPNOW_INTERNAL:
            Serial.println("Internal error");
            break;

        case ESP_ERR_ESPNOW_NO_MEM:
            Serial.println("Out of memory");
            break;

        case ESP_ERR_ESPNOW_NOT_FOUND:
            Serial.println("Peer is not found");
            break;

        case ESP_ERR_ESPNOW_IF:
            Serial.println("Current WiFi interface doesn't match that of peer");
            break;

        default:
            break;
    }
}

// ESP docs says this runs from high priority WiFi task, so avoid lengthy operations
// Does not seem to require ISR versions of FreeRTOS functions
static void msg_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    if(len < 1){return;}
    // TODO check who its from?

    if(*data == MSG_TELEMETRY && len == (sizeof(cart_telemetry_t)+1))
    {
        cart_telemetry_t tele;
        memcpy(&tele,(data+1),sizeof(cart_telemetry_t));
        xQueueSend(telemetry_command_queue, &tele,0);
    }
}

static void msg_send_cb(const uint8_t* mac, esp_now_send_status_t sendStatus)
{

    switch (sendStatus)
    {
        case ESP_NOW_SEND_SUCCESS:
            Serial.println("Send success");
            break;

        case ESP_NOW_SEND_FAIL:
            Serial.println("Send Failure");
            break;
    
        default:
            break;
    }
}

bool network_init(void)
{
    //Puts ESP in STATION MODE
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init()!=0)
    {
        // TODO restart?
        return false;
    }

    esp_now_peer_info_t peer_info;
    peer_info.channel = WIFI_CHANNEL;
    memcpy(peer_info.peer_addr,broadcast_mac,6);
    peer_info.ifidx = ESP_IF_WIFI_STA;
    peer_info.encrypt = false;
    esp_err_t status = esp_now_add_peer(&peer_info);
    if(ESP_OK != status)
    {
        Serial.println("Could not add peer");
        handle_error(status);
    }

    // Set up callback
    status = esp_now_register_recv_cb(msg_recv_cb);
    if(ESP_OK != status)
    {
        Serial.println("Could not register callback");
        handle_error(status);
    }

    status = esp_now_register_send_cb(msg_send_cb);
    if(ESP_OK != status)
    {
        Serial.println("Could not register send callback");
        handle_error(status);
    }
    return true;
}

void send_command(controller_input_t * controller_input)
{
    // Pack
    uint16_t packet_size = sizeof(controller_input_t)+1;
    uint8_t msg[packet_size];
    msg[0] = MSG_CONTROLLER_INPUT;
    memcpy(&msg[1], controller_input, sizeof(controller_input_t));

    esp_err_t status = esp_now_send(broadcast_mac, msg, packet_size);
    if(ESP_OK != status) 
    {
        Serial.println("Error sending message");
        handle_error(status);
    }
}

void network_task(void * param)
{   
    telemetry_command_queue = xQueueCreate(TELEMETRY_QUEUE_SIZE, sizeof(cart_telemetry_t));

    cart_telemetry_t tele;

    for(;;)
    {
        if(xQueueReceive(telemetry_command_queue, &tele, portMAX_DELAY) == pdPASS)
        {
            // TODO process/pass on
            Serial.println("Received telemetry");
        }
    }
}