#include "task_handler.h"
#include "global.h"
#include "task_webserver.h"
#include "led_blinky.h"
#include "neo_blinky.h"
#include "coreiot.h"

void handleWebSocketMessage(String message)
{
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  String page = doc["page"] | "";
  JsonObject value = doc["value"].isNull() ? JsonObject() : doc["value"].as<JsonObject>();

  if (page == "device")
  {
    String name = value["name"] | "";
    String status = value["status"] | "";
    int gpio = value["gpio"] | -1;
    bool isOn = (status == "ON");

    if (name == "LED1")
    {
      glob_led01_enabled = isOn;
    }
    else if (name == "LED2")
    {
      glob_led02_enabled = isOn;
      if (xLed02Semaphore != nullptr)
      {
        xSemaphoreGive(xLed02Semaphore);
      }
    }

    if (gpio >= 0)
    {
      pinMode(gpio, OUTPUT);
      digitalWrite(gpio, isOn ? HIGH : LOW);
    }

    Serial.printf("[WebUI] Device %s (GPIO %d) -> %s\n", name.c_str(), gpio, isOn ? "ON" : "OFF");
    coreiot_publish_led_states();
  }
  else if (page == "setting")
  {
    String WIFI_SSID_local = value["ssid"] | "";
    String WIFI_PASS_local = value["password"] | "";
    String CORE_TOKEN_local = value["token"] | "";
    String CORE_SERV_local = value["server"] | "";
    String CORE_PORT_local = value["port"] | "";

    WIFI_SSID = WIFI_SSID_local;
    WIFI_PASS = WIFI_PASS_local;
    CORE_IOT_TOKEN = CORE_TOKEN_local;
    CORE_IOT_SERVER = CORE_SERV_local;
    CORE_IOT_PORT = CORE_PORT_local;

    Serial.println("Nhan cau hinh tu WebSocket:");
    Serial.println("SSID: " + WIFI_SSID);
    Serial.println("PASS: " + WIFI_PASS);
    Serial.println("TOKEN: " + CORE_IOT_TOKEN);
    Serial.println("SERVER: " + CORE_IOT_SERVER);
    Serial.println("PORT: " + CORE_IOT_PORT);

    Save_info_File(WIFI_SSID, WIFI_PASS, CORE_IOT_TOKEN, CORE_IOT_SERVER, CORE_IOT_PORT);

    String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
    ws.textAll(msg);
  }
  else if (page == "pump_setting")
  {
    int presetMin = value["presetMin"] | 0;
    if (presetMin == 3 || presetMin == 5 || presetMin == 7)
    {
      glob_irrigation_mode = 2;
      glob_irrigation_duration_ms = (uint32_t)presetMin * 60UL * 1000UL;
      glob_irrigation_timer_started_ms = millis();
      glob_irrigation_timer_active = true;
      glob_pump_enabled = true;
      glob_manual_pump_state = false;

      if (xPumpSemaphore != nullptr)
      {
        xSemaphoreGive(xPumpSemaphore);
      }
    }

    StaticJsonDocument<192> resp;
    resp["page"] = "pump_state";
    JsonObject v = resp.createNestedObject("value");
    v["pumpState"] = glob_pump_enabled;
    v["timerActive"] = glob_irrigation_timer_active;
    v["durationSec"] = glob_irrigation_duration_ms / 1000U;
    v["mode"] = glob_irrigation_mode;

    String out;
    serializeJson(resp, out);
    Webserver_sendata(out);
  }
  else if (page == "pump_force_off")
  {
    glob_irrigation_mode = 1;
    glob_manual_pump_state = false;
    glob_irrigation_timer_active = false;
    glob_pump_enabled = false;

    if (xPumpSemaphore != nullptr)
    {
      xSemaphoreGive(xPumpSemaphore);
    }

    StaticJsonDocument<192> resp;
    resp["page"] = "pump_state";
    JsonObject v = resp.createNestedObject("value");
    v["pumpState"] = glob_pump_enabled;
    v["timerActive"] = glob_irrigation_timer_active;
    v["durationSec"] = glob_irrigation_duration_ms / 1000U;
    v["mode"] = glob_irrigation_mode;

    String out;
    serializeJson(resp, out);
    Webserver_sendata(out);
  }
  else if (page == "pump_restore_auto")
  {
    glob_irrigation_mode = 0;
    glob_manual_pump_state = false;
    glob_irrigation_timer_active = false;

    if (xPumpSemaphore != nullptr)
    {
      xSemaphoreGive(xPumpSemaphore);
    }

    StaticJsonDocument<192> resp;
    resp["page"] = "pump_state";
    JsonObject v = resp.createNestedObject("value");
    v["pumpState"] = glob_pump_enabled;
    v["timerActive"] = glob_irrigation_timer_active;
    v["durationSec"] = glob_irrigation_duration_ms / 1000U;
    v["mode"] = glob_irrigation_mode;

    String out;
    serializeJson(resp, out);
    Webserver_sendata(out);
  }
  else if (page == "get_config")
  {
    StaticJsonDocument<512> resp;
    resp["page"] = "config";
    JsonObject v = resp.createNestedObject("value");

    JsonArray devs = v.createNestedArray("devices");
    JsonObject d1 = devs.createNestedObject();
    d1["name"] = "LED1";
    d1["gpio"] = LED_GPIO;
    d1["status"] = glob_led01_enabled ? "ON" : "OFF";

    JsonObject d2 = devs.createNestedObject();
    d2["name"] = "LED2";
    d2["gpio"] = NEO_PIN;
    d2["status"] = glob_led02_enabled ? "ON" : "OFF";

    JsonObject s = v.createNestedObject("settings");
    s["ssid"] = WIFI_SSID;
    s["password"] = WIFI_PASS;
    s["token"] = CORE_IOT_TOKEN;
    s["server"] = CORE_IOT_SERVER;
    s["port"] = CORE_IOT_PORT;

    JsonObject p = v.createNestedObject("pump");
    p["pumpState"] = glob_pump_enabled;
    p["timerActive"] = glob_irrigation_timer_active;
    p["durationSec"] = glob_irrigation_duration_ms / 1000U;
    p["mode"] = glob_irrigation_mode;

    String out;
    serializeJson(resp, out);
    Webserver_sendata(out);
  }
  else if (page == "reset_factory")
  {
    Serial.println("Yeu cau Reset Factory tu Web UI");
    Delete_info_File();
  }
}
