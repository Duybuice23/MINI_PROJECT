#include "temp_humi_monitor.h"
#include <Wire.h>
#include <ArduinoJson.h>
#include "task_webserver.h"
#include "soil_moisture_sensor.h"
DHT20 dht20;

LiquidCrystal_I2C lcd(33, 16, 2);

static float convertSoilMoistureToPercent(int rawAdc);
static void updateLcd(float temperature, float humidity, float soilMoisturePercent);
static void sendSensorToWeb(float temperature, float humidity, float soilMoisturePercent);

void temp_humi_monitor(void *pvParameters)
{
  Wire.begin(11, 12);
  dht20.begin();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");
  vTaskDelay(pdMS_TO_TICKS(1500));

  for (;;)
  {
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity    = dht20.getHumidity();
    int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
    float soilMoisturePercent = convertSoilMoistureToPercent(soilMoistureRaw);
    bool pumpShouldOn = false;
    static bool lastPumpState = false;

    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("Failed to read from DHT20!");
      temperature = -1.0f;
      humidity    = -1.0f;
    }

    glob_temperature = temperature;
    glob_humidity    = humidity;
    glob_soil_moisture = soilMoisturePercent;
    glob_soil_moisture_raw = soilMoistureRaw;
    if (glob_irrigation_mode == 0)
    {
      pumpShouldOn = (soilMoistureRaw < SOIL_MOISTURE_RAW_THRESHOLD);
    }
    else if (glob_irrigation_mode == 1)
    {
      pumpShouldOn = glob_manual_pump_state;
    }
    else if (glob_irrigation_mode == 2)
    {
      unsigned long now = millis();
      unsigned long elapsed = now - glob_irrigation_timer_started_ms;
      bool timerOn = glob_irrigation_timer_active && (elapsed < glob_irrigation_duration_ms);
      if (!timerOn)
      {
        glob_irrigation_timer_active = false;
      }
      pumpShouldOn = timerOn;
    }

    glob_pump_enabled = pumpShouldOn;

    if (pumpShouldOn != lastPumpState)
    {
      lastPumpState = pumpShouldOn;
      if (xPumpSemaphore != nullptr)
      {
        xSemaphoreGive(xPumpSemaphore);
      }
    }

    updateLcd(temperature, humidity, soilMoisturePercent);

    sendSensorToWeb(temperature, humidity, soilMoisturePercent);

    Serial.print("[DHT20] H: ");
    Serial.print(humidity);
    Serial.print("%  T: ");
    Serial.print(temperature);
    Serial.print(" C ");
    Serial.print(" Soil: ");
    Serial.print(soilMoisturePercent, 0);
    Serial.print("% (raw=");
    Serial.print(soilMoistureRaw);
    Serial.print(") Pump:");
    Serial.println(pumpShouldOn ? "ON" : "OFF");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

static float convertSoilMoistureToPercent(int rawAdc)
{

  long mapped = map(rawAdc, SOIL_ADC_WET, SOIL_ADC_DRY, 0, 100);
  mapped = constrain(mapped, 0, 100);
  return (float)mapped;
}

static void updateLcd(float temperature, float humidity, float soilMoisturePercent)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Soil: ");
  lcd.print(soilMoisturePercent, 0);
  lcd.print("%");


  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("%");
}

static void sendSensorToWeb(float temperature, float humidity, float soilMoisturePercent)
{
  StaticJsonDocument<192> doc;
  doc["page"] = "sensor";
  doc["temp"] = temperature;
  doc["humi"] = humidity;
  doc["soilMoisture"] = soilMoisturePercent;
  doc["pumpState"] = glob_pump_enabled;

  String json;
  serializeJson(doc, json);
  Webserver_sendata(json);
}

