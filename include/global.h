#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ====== Gia tri cam bien toan cuc ======
#define LED_GPIO 48
#define NEO_PIN 45
#define LED_COUNT 1
#define PUMP_GPIO_1 2
#define PUMP_GPIO_2 3
#define SOIL_MOISTURE_RAW_THRESHOLD 1200
extern float glob_temperature;
extern float glob_humidity;
extern float glob_soil_moisture;
extern int   glob_soil_moisture_raw;
extern volatile bool glob_pump_enabled;
extern volatile uint8_t glob_irrigation_mode; // 0:auto, 1:manual, 2:timer
extern volatile uint32_t glob_irrigation_duration_ms;
extern volatile bool glob_manual_pump_state;
extern volatile bool glob_irrigation_timer_active;
extern volatile uint32_t glob_irrigation_timer_started_ms;

// Cho phep bat/tat LED tu Web UI
extern volatile bool glob_led01_enabled;
extern volatile bool glob_led02_enabled;

// ====== WiFi / CoreIoT config ======
extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

// ====== WiFi AP dung cho main_server_task ======
extern String ssid;
extern String password;

// ====== WiFi STA ======
extern String wifi_ssid;
extern String wifi_password;

// Co bao da co Internet
extern bool isWifiConnected;

// Semaphore bao co internet
extern SemaphoreHandle_t xBinarySemaphoreInternet;

// ====== Semaphore dong bo cac task ======
extern SemaphoreHandle_t xLed01Semaphore;
extern SemaphoreHandle_t xLed02Semaphore;
extern SemaphoreHandle_t xPumpSemaphore;

#endif
