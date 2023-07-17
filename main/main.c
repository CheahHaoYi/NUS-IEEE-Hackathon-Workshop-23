#include <esp_log.h>

#include "packet.h"
#include "device.h"
#include "rainMaker.h"

static const char *TAG = "MAIN";
#define INITIAL_POWER_STATE false
#define INITIAL_SENSOR_READING 20.0

QueueHandle_t event_queue = NULL;

#define DELAY(x) vTaskDelay(x / portTICK_PERIOD_MS)

void flash_led() 
{
    while (true) {
        set_onBoard_led(true);
        DELAY(1000);
        set_onBoard_led(false);
        DELAY(1000);
    }
}

void queue_processing()
{
    event_packet_t event = {0};

    while (true) {
        DELAY(1000);

        if (xQueueReceive(event_queue, &event, 100) == pdTRUE) {
            ESP_LOGI(TAG, "Receive dir (%d), dev (%d)", event.direction, event.device);
            
            if (event.direction == ESP_TO_APP) {
                rainMaker_update(event);
            } else if (event.direction == APP_TO_ESP) {
                hardware_update(event);
            }
        }
    }
}

void app_main()
{
    /* Initialize Application specific hardware drivers and
     * set initial state.
     */
    hardware_init(INITIAL_POWER_STATE, INITIAL_SENSOR_READING);

    // Workshop Part 1: Blink on-board LED & Change LED color
    flash_led();
    
    ESP_LOGI(TAG, "Hardware initialization complete");

    // Rainmaker initialization sequence:
    /**
     * 1. Initialize Wi-Fi
     * 2. Initialize RainMaker node
     * 3. Add devices and services
     * 4. Start RainMaker
     * 5. Start Wi-Fi
    */

    // Workshop Part 2: Wi-Fi Provisioning
    // wifi_init();
    // rainMaker_init();
    // rm_add_dummy();
    // rainMaker_start();
    // wifi_start();
    
    // Workshop Part 3: RainMaker & Toggle LED from App
        // Task: Add a "school name" attribute to the device
    // wifi_init();
    // rainMaker_init();
    // rm_add_light_switch(INITIAL_POWER_STATE);
    // rainMaker_start();
    // wifi_start();

    // Workshop Part 4: Add Sensor
        // Task: Fix the sensor reading (why is it inconsistent? Hint: device.c)
    // wifi_init();
    // rainMaker_init();
    // rm_add_light_switch(INITIAL_POWER_STATE);
    // rm_add_sensor(INITIAL_SENSOR_READING);
    // rainMaker_start();
    // wifi_start();

    // Workshop Part 5: Add water pump
    // wifi_init();
    // rainMaker_init();
    // rm_add_light_switch(INITIAL_POWER_STATE);
    // rm_add_water_pump(INITIAL_POWER_STATE);
    // rm_add_sensor(INITIAL_SENSOR_READING);
    // rainMaker_start();
    // wifi_start();

    // Workshop Part 6: Putting everything together
        // Task:
            // 1. Add a "wetness" attribute to the sensor that changes based on the perceived wetness
            // 2. Make the water pump turn on when the wetness crosses a certain threshold

    // Create input processing queue
    event_queue = xQueueCreate(50, sizeof(event_packet_t));
    queue_processing(); 
}
