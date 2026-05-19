#include "task_core_iot.h"
#include "global.h"
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

constexpr char LED_STATE_ATTR[] = "ledState";
constexpr char PUMP_STATE_ATTR[] = "pumpState";

volatile int ledMode = 0;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;
volatile bool pumpTimerActive = false;
volatile unsigned long pumpTimerStartMs = 0U;
volatile unsigned long pumpTimerDurationMs = 0U;

constexpr int16_t telemetrySendInterval = 10000U;

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
};

void processSharedAttributes(const Shared_Attribute_Data &data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {


















    }
}

RPC_Response handleSetLogic(const char* methodName, bool state) {
    if (strcmp(methodName, "setLed01") == 0) {
        glob_led01_enabled = state;

    }
    else if (strcmp(methodName, "setLed02") == 0) {
        glob_led02_enabled = state;

    }

    Serial.printf("[%s] State changed to: %s\n", methodName, state ? "ON" : "OFF");


    return RPC_Response(methodName, state);
}

static void publishPumpState()
{
    tb.sendAttributeData(PUMP_STATE_ATTR, glob_pump_enabled);
    tb.sendTelemetryData(PUMP_STATE_ATTR, glob_pump_enabled ? 1 : 0);
}

static void setPumpState(bool state)
{
    glob_pump_enabled = state;
    if (xPumpSemaphore != nullptr)
    {
        xSemaphoreGive(xPumpSemaphore);
    }
    publishPumpState();
}

static RPC_Response startPumpPreset(const char* methodName, unsigned long durationMs)
{
    pumpTimerDurationMs = durationMs;
    pumpTimerStartMs = millis();
    pumpTimerActive = true;
    setPumpState(true);

    Serial.printf("[%s] Pump ON for %lu ms\n", methodName, durationMs);
    return RPC_Response(methodName, true);
}


RPC_Response handleGetLogic(const char* methodName) {
    bool currentState = false;

    if (strcmp(methodName, "getLed01") == 0) {
        currentState = glob_led01_enabled;
    }
    else if (strcmp(methodName, "getLed02") == 0) {
        currentState = glob_led02_enabled;
    }

    return RPC_Response(methodName, currentState);
}


RPC_Response setLed01Switch(const RPC_Data &data) { return handleSetLogic("setLed01", data); }
RPC_Response setLed02Switch(const RPC_Data &data) { return handleSetLogic("setLed02", data); }

RPC_Response getLed01Status(const RPC_Data &data) { return handleGetLogic("getLed01"); }
RPC_Response getLed02Status(const RPC_Data &data) { return handleGetLogic("getLed02"); }
RPC_Response setPump03m(const RPC_Data &data) { return startPumpPreset("setPump03m", 3UL * 60UL * 1000UL); }
RPC_Response setPump05m(const RPC_Data &data) { return startPumpPreset("setPump05m", 5UL * 60UL * 1000UL); }
RPC_Response setPump07m(const RPC_Data &data) { return startPumpPreset("setPump07m", 7UL * 60UL * 1000UL); }
RPC_Response getPumpStatus(const RPC_Data &data) { return RPC_Response("getPumpStatus", glob_pump_enabled); }

RPC_Response setLedSwitchValue(const RPC_Data &data)
{
    Serial.println("Received Switch state");
    bool newState = data;
    Serial.print("Switch state change: ");
    Serial.println(newState);
    return RPC_Response("setLedSwitchValue", newState);
}

const std::array<RPC_Callback, 8U> callbacks = {
    RPC_Callback{"setLed01", setLed01Switch},
    RPC_Callback{"setLed02", setLed02Switch},
    RPC_Callback{"getLed01", getLed01Status},
    RPC_Callback{"getLed02", getLed02Status},
    RPC_Callback{"setPump03m", setPump03m},
    RPC_Callback{"setPump05m", setPump05m},
    RPC_Callback{"setPump07m", setPump07m},
    RPC_Callback{"getPumpStatus", getPumpStatus}
};

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());

void CORE_IOT_sendata(String mode, String feed, String data)
{
    if (mode == "attribute")
    {
        tb.sendAttributeData(feed.c_str(), data);
    }
    else if (mode == "telemetry")
    {
        float value = data.toFloat();
        tb.sendTelemetryData(feed.c_str(), value);
    }
    else
    {

    }
}

void CORE_IOT_reconnect()
{
    if (!tb.connected())
    {
        if (!tb.connect(CORE_IOT_SERVER.c_str(), CORE_IOT_TOKEN.c_str(), CORE_IOT_PORT.toInt()))
        {

            return;
        }

        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

        Serial.println("Subscribing for RPC...");
        if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend()))
        {

            return;
        }

        if (!tb.Shared_Attributes_Subscribe(attributes_callback))
        {

            return;
        }

        Serial.println("Subscribe done");

        if (!tb.Shared_Attributes_Request(attribute_shared_request_callback))
        {

            return;
        }
        tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
    }
    else if (tb.connected())
    {
        if (pumpTimerActive)
        {
            const unsigned long now = millis();
            if (now - pumpTimerStartMs >= pumpTimerDurationMs)
            {
                pumpTimerActive = false;
                setPumpState(false);
                Serial.println("[PumpTimer] Timer ended, Pump OFF");
            }
        }
        tb.loop();
    }
}


