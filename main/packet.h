#pragma once

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

// Direction of packet flow
enum {
    ESP_TO_APP = 0,
    APP_TO_ESP = 1,
};

// Device target
enum {
    DEVICE_LED = 0,
    DEVICE_PUMP,
    DEVICE_SENSOR,
}; 

// Packet structure
typedef struct {
    uint8_t direction;
    uint8_t device;
    uint8_t data_on_off;
    uint8_t data_light_brightness;
    uint8_t data_pump_speed;
    float data_sensor;
    bool is_on_off;
} event_packet_t;