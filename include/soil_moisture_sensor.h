#include <Arduino.h>
#include <global.h>
#define SOIL_MOISTURE_PIN 1
// Calibrate these values based on your sensor and soil conditions.
// raw <= SOIL_ADC_WET  -> 100%
// raw >= SOIL_ADC_DRY  -> 0%
#define SOIL_ADC_WET  1200
#define SOIL_ADC_DRY  3200
#include "freertos/FreeRTOS.h"
