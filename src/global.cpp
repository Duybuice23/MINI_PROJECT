#include "global.h"

// ====== Bien toan cuc cam bien ======
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

// ====== Bat/tat hien thi LED tu WebUI ======
volatile bool glob_led01_enabled = true;
volatile bool glob_led02_enabled = true;

// ====== WiFi / CoreIoT config ======
String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN = "b3y9zdp1ewheh4v0469i";
String CORE_IOT_SERVER = "thingsboard.cloud";
String CORE_IOT_PORT = "1883";

// ====== WiFi AP mac dinh ======
String ssid = "ESP32 LOCAL";
String password = "12345678";

// ====== WiFi STA ======
String wifi_ssid = "B4-1020";
String wifi_password = "10202005";

// Co bao da co Internet
bool isWifiConnected = false;

// ====== Semaphore cho Internet ======
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();

// ====== Semaphore cho cac task ======
SemaphoreHandle_t xLed01Semaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t xLed02Semaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t xPumpSemaphore = xSemaphoreCreateBinary();
