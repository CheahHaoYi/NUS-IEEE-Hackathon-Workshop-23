#pragma once

#include <string.h>
#include <nvs_flash.h>

#include "esp_err.h"
#include "esp_log.h"

#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_scenes.h>
#include <esp_rmaker_console.h>

#include <app_wifi.h>
#include "packet.h"

#define DEFAULT_PUMP_SPEED 3
#define DEFAULT_LIGHT_BRIGHTNESS 25

#define PARAM_NAME_ON_OFF ESP_RMAKER_DEF_POWER_NAME
#define PARAM_NAME_LED "LED Brightness"
#define PARAM_NAME_PUMP "Water Pump Speed"

void wifi_init(void);

void wifi_start(void);

esp_rmaker_node_t* rainMaker_init(void);

void rainMaker_start(void);

void rainMaker_update(event_packet_t event);

esp_err_t rm_add_dummy();

esp_err_t rm_add_light_switch(bool initial_state);

esp_err_t rm_add_water_pump(bool initial_state);

esp_err_t rm_add_sensor(float initial_state);