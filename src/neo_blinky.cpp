#include "neo_blinky.h"
#include "global.h"

static Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

void neo_blinky(void *pvParameters)
{
  strip.begin();
  strip.clear();
  strip.show();

  while (1)
  {

    if (!glob_led02_enabled)
    {

      strip.clear();
      strip.show();


      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }







    for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    strip.show();


    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

