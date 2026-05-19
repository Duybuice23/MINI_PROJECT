#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "pump_control.h"

#include "coreiot.h"


#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"


void setup()
{
  Serial.begin(115200);


  check_info_File(false);


  xTaskCreate(led_blinky,
              "Task LED Blink",
              2048,
              nullptr,
              2,
              nullptr);


  xTaskCreate(neo_blinky,
              "Task NEO Blink",
              2048,
              nullptr,
              2,
              nullptr);


  xTaskCreate(temp_humi_monitor,
              "Task TEMP HUMI Monitor",
              4096,
              nullptr,
              3,
              nullptr);


  xTaskCreate(pump_control_task,
              "Task Pump Control",
              2048,
              nullptr,
              2,
              nullptr);



















  xTaskCreate(coreiot_task,
              "CoreIOT Task",
              4096,
              nullptr,
              2,
              nullptr);


  xTaskCreate(Task_Toogle_BOOT,
              "Task_Toogle_BOOT",
              2048,
              nullptr,
              1,
              nullptr);
}

void loop()
{

  if (check_info_File(true))
  {
    Wifi_reconnect();
  }


  Webserver_reconnect();
}

