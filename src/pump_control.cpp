#include "pump_control.h"
#include "global.h"

static void applyPumpState(bool isOn)
{
  digitalWrite(PUMP_GPIO_1, isOn ? HIGH : LOW);
  //digitalWrite(PUMP_GPIO_2, isOn ? HIGH : LOW);
}

void pump_control_task(void *pvParameters)
{
  pinMode(PUMP_GPIO_1, OUTPUT);
  pinMode(PUMP_GPIO_2, OUTPUT);
  applyPumpState(false);

  for (;;)
  {
    if (xPumpSemaphore != nullptr)
    {
      xSemaphoreTake(xPumpSemaphore, pdMS_TO_TICKS(1000));
    }
    applyPumpState(glob_pump_enabled);
  }
}
