#include "led_blinky.h"
#include "global.h"

void led_blinky(void *pvParameters)
{
  pinMode(LED_GPIO, OUTPUT);

  for (;;)
  {

    if (!glob_led01_enabled)
    {

      digitalWrite(LED_GPIO, LOW);


      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }






    digitalWrite(LED_GPIO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));


  }
}

