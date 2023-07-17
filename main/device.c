#include "device.h"

#include <stdlib.h>

// RTOS items
#ifdef CONFIG_EXAMPLE_APP_CONNECTION
static TimerHandle_t sensor_timer;
#endif
extern QueueHandle_t event_queue;

static bool current_led_state = false;
static float current_temperature = 0.0;
static bool current_pump_state = false;

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t adc_cali_handle;

static const char *TAG = "DEVICE";

esp_err_t water_pump_init(bool initial_state);
esp_err_t sensor_init(float initial_reading);
static void push_btn_callback(void *arg);
void sensor_set_led(int sensor_reading);
void sensor_set_pump(int sensor_reading);

void hardware_init(bool initial_onoff_state, float initial_sensor_reading)
{
    // Configure boot button
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);

    if (btn_handle) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_callback, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
    }


    // Configure on board led
    ws2812_led_init();
    set_onBoard_led(initial_onoff_state);

    #ifdef CONFIG_EXAMPLE_ENABLE_PUMP
    // Configure water pump
    water_pump_init(initial_onoff_state);
    #endif

    #ifdef CONFIG_EXAMPLE_ENABLE_SENSOR
    // Configure sensor and timer to update at a regular interval
    sensor_init(initial_sensor_reading);
    #endif
}

void hardware_update(event_packet_t event)
{
    switch (event.device) {
        case DEVICE_LED:
            set_onBoard_led(event.data_on_off);
            break;
        case DEVICE_PUMP:
            set_pump(event.data_on_off);
            break;
        case DEVICE_SENSOR:
            // Do nothing
            break;
        default:
            ESP_LOGE(TAG, "Unknown device type");
            break;
    }
    return;
}


/******************************************************
 * On Board LED functions
******************************************************/
/** 
 * @brief Callback for boot button press
 * 
*/
static void push_btn_callback(void *arg)
{
    // Note that the global variable current_led_state is modified throught the function
    set_onBoard_led(!current_led_state);

    event_packet_t led_data_to_app = {
        .direction = ESP_TO_APP,
        .device = DEVICE_LED,
        .data_on_off = current_led_state,
    };

    xQueueSend(event_queue, &led_data_to_app, 0);
}

void set_onBoard_led(bool isLedOn)
{   
    current_led_state = isLedOn;

    if (isLedOn) {
        ws2812_led_set_rgb(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);
    } else {
        ws2812_led_clear();
    }
}

/******************************************************
 * Water Pump Functions
******************************************************/
 
void set_pump(bool isPumpOn)
{
    current_pump_state = isPumpOn; 

    if (isPumpOn) {
        gpio_set_level(RELAY_GPIO, RELAY_ACTIVE_LEVEL);
    } else {
        gpio_set_level(RELAY_GPIO, 1 - RELAY_ACTIVE_LEVEL);
    }
}

esp_err_t water_pump_init(bool initial_state)
{
    // Configure GPIO for water pump control
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = ((uint64_t)1 << RELAY_GPIO),
    };
    gpio_config(&io_conf);

    return ESP_OK;
}

/******************************************************
 * Sensor functions
******************************************************/

int map_range(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float get_sensor_reading(void)
{
    // Hint: Something feels off here hmmmmm
    int rand_num = rand() % 20;
    return (float)(rand_num + 60);

    // Provide power to Sensor
    gpio_set_level(SENSOR_GPIO, 1);
    // Let value stabilize
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    // Read values
    int raw_value = -1;
    adc_oneshot_read(adc_handle, ADC_PIN, &raw_value);
    int sensor_val = map_range(raw_value, 0, ADC_RAW_MAX, 0, SENSOR_RANGE);
    // ESP_LOGI(TAG, "Raw ADC value: %d, mapped sensor value: %d", raw_value, sensor_val);
    // Turn off power to sensor
    gpio_set_level(SENSOR_GPIO, 0);
    return sensor_val;
}

static void sensor_update(TimerHandle_t handle)
{
    float reading = get_sensor_reading();

    event_packet_t sensor_data_to_app = {
        .direction = ESP_TO_APP,
        .device = DEVICE_SENSOR,
        .data_sensor = reading,
    };

    xQueueSend(event_queue, &sensor_data_to_app, 0);
}

void sensor_set_led(int sensor_reading)
{
    return;
    // TODO: insert how to control LED based on sensor reading
    // Hint: can use MOISTURE_DRY and MOISTURE_WET from device.h
}

void sensor_set_pump(int sensor_reading)
{
    return;
    // TODO: insert how to control pump based on sensor reading
    // Hint: can use MOISTURE_DRY and MOISTURE_WET from device.h
}

esp_err_t sensor_init(float initial_reading)
{
    // Digital output to control sensor power
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = ((uint64_t)1 << SENSOR_GPIO),
    };
    gpio_config(&io_conf);

    // ADC for analog read
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT,
    };

    adc_oneshot_new_unit(&adc_config, &adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTEN,
    };

    adc_oneshot_config_channel(adc_handle, ADC_PIN, &channel_config);

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };

    adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);

    // Start timer to trigger every reporting interval 
    current_temperature = initial_reading;

    sensor_timer = xTimerCreate("sensor_update_tm", (REPORTING_PERIOD * 1000) / portTICK_PERIOD_MS,
                            pdTRUE, NULL, sensor_update);
    if (sensor_timer) {
        xTimerStart(sensor_timer, 0);
        return ESP_OK;
    }
    return ESP_FAIL;
}

