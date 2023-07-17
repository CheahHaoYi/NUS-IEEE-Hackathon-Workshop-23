#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sdkconfig.h>

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_intr_alloc.h"
#include "driver/gpio.h"

#include <iot_button.h>
#include <ws2812_led.h>
#include <app_reset.h>

#include "packet.h"

#define DEFAULT_SWITCH_POWER        true
#define DEFAULT_LIGHT_POWER         true
#define DEFAULT_LIGHT_BRIGHTNESS    25
#define DEFAULT_FAN_POWER           false
#define DEFAULT_FAN_SPEED           3
#define DEFAULT_TEMPERATURE         80
#define REPORTING_PERIOD            20 /* In seconds */

/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          CONFIG_EXAMPLE_BOARD_BUTTON_GPIO
#define BUTTON_ACTIVE_LEVEL  0
/* This is the GPIO on which the power will be set */
#define SENSOR_GPIO    CONFIG_EXAMPLE_SENSOR_POWER_GPIO

#define ADC_UNIT  ADC_UNIT_1
#define ADC_PIN ADC_CHANNEL_0
#define ADC_ATTEN ADC_ATTEN_DB_11
#define ADC_BITWIDTH ADC_BITWIDTH_DEFAULT
#define ADC_RAW_MAX (4095)
#define SENSOR_RANGE (100)

// Value boundaries for moisture sensor
// The higher, the drier
#define MOISTURE_DRY 80 
#define MOISTURE_WET 60

#define RELAY_GPIO CONFIG_EXAMPLE_RELAY_GPIO
#define RELAY_ACTIVE_LEVEL 1

/* These values correspoind to H,S,V = 120,100,10 */
#define DEFAULT_RED     0
#define DEFAULT_GREEN   25
#define DEFAULT_BLUE    0

/* To reset & display QR code after reset*/
#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10

void hardware_init(bool initial_onoff_state, float initial_sensor_reading);

void hardware_update(event_packet_t event);

void set_onBoard_led(bool isLedOn);
void set_pump(bool isPumpOn);
float get_sensor_reading(void);
