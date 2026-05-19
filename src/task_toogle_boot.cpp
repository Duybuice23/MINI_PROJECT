#include "task_toogle_boot.h"
#include "global.h"
#include <WiFi.h>

#define BOOT_BUTTON_PIN 0
#define HOLD_TIME_MS    5000

void Task_Toogle_BOOT(void *pvParameters)
{
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  unsigned long buttonPressStartTime = 0;
  bool isPressed = false;

  Serial.println("BOOT: San sang. Nhan giu > 3s de Factory Reset.");

  for (;;)
  {

    if (digitalRead(BOOT_BUTTON_PIN) == LOW)
    {
      if (!isPressed)
      {
        isPressed = true;
        buttonPressStartTime = millis();
        Serial.println(">> Nut BOOT dang duoc nhan...");
      }
      else
      {
        unsigned long holdDuration = millis() - buttonPressStartTime;


        if (holdDuration > HOLD_TIME_MS)
        {
          Serial.println("\n[SYSTEM] === FACTORY RESET KICH HOAT ===");


          pinMode(LED_GPIO, OUTPUT);
          for(int i=0; i<5; i++){
              digitalWrite(LED_GPIO, !digitalRead(LED_GPIO));
              vTaskDelay(pdMS_TO_TICKS(100));
          }
          digitalWrite(LED_GPIO, LOW);


          Delete_info_File();
          Serial.println("[SYSTEM] Da xoa file config.");




          WiFi.disconnect(true, true);
          vTaskDelay(pdMS_TO_TICKS(500));
          Serial.println("[SYSTEM] Da xoa WiFi NVS.");


          Serial.println("[SYSTEM] Dang khoi dong lai...");
          Serial.flush();
          ESP.restart();
        }
      }
    }
    else
    {

      if (isPressed)
      {
        isPressed = false;
        buttonPressStartTime = 0;
        Serial.println(">> Da nha nut BOOT.");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
