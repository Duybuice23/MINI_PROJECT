#include "global.h"


float glob_temperature = 0.0f;
float glob_humidity = 0.0f;
float glob_soil_moisture = 0.0f;
int glob_soil_moisture_raw = 0;
volatile bool glob_pump_enabled = false;
volatile uint8_t glob_irrigation_mode = 0;
volatile uint32_t glob_irrigation_duration_ms = 30000;
volatile bool glob_manual_pump_state = false;
volatile bool glob_irrigation_timer_active = false;
volatile uint32_t glob_irrigation_timer_started_ms = 0;


volatile bool glob_led01_enabled = true;
volatile bool glob_led02_enabled = true;


String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN = "qax57svnkv2qoz2nmkva";
String CORE_IOT_SERVER = "thingsboard.cloud";
String CORE_IOT_PORT = "1883";


String ssid = "ESP32 LOCAL";
String password = "12345678";


String wifi_ssid = "B4-1020";
String wifi_password = "10202005";


bool isWifiConnected = false;


SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();


SemaphoreHandle_t xLed01Semaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t xLed02Semaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t xPumpSemaphore = xSemaphoreCreateBinary();

