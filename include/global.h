#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


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
extern volatile uint8_t glob_irrigation_mode;
extern volatile uint32_t glob_irrigation_duration_ms;
extern volatile bool glob_manual_pump_state;
extern volatile bool glob_irrigation_timer_active;
extern volatile uint32_t glob_irrigation_timer_started_ms;


extern volatile bool glob_led01_enabled;
extern volatile bool glob_led02_enabled;


extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;


extern String ssid;
extern String password;


extern String wifi_ssid;
extern String wifi_password;


extern bool isWifiConnected;


extern SemaphoreHandle_t xBinarySemaphoreInternet;


extern SemaphoreHandle_t xLed01Semaphore;
extern SemaphoreHandle_t xLed02Semaphore;
extern SemaphoreHandle_t xPumpSemaphore;

#endif

