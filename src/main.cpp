#include "Myconfig.h"
#include <defineName.h>
#include <FrameConvert.h>
#include <MicroSDTask.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Motorbike.h>
#include <Update.h>
#include <ContestManager.h>
#include <HardwareManager.h>
#include <NetworkManager.h>

#define VERSION "1.0"

const char *ssid_default = "PC_NET_MOBILE";
const char *password_default = "1234567890";
float otaDone;
unsigned long timerConnectWifi;
bool lastRunState = false;

HardwareManager hardwareManager;
NetworkManager networkManager(hardwareManager);

ContestManager contestManager(hardwareManager);
JsonString contest;
TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t contestTaskHandle = NULL;
TaskHandle_t runTaskHandle = NULL;
QueueHandle_t queueServer;
QueueHandle_t queueContest;

void runTask(void *pvParameters);
void webServerTask(void *pvParameters);
void contestTask(void *pvParameters);
void firstSetup()
{
  String data;
  JsonDocument contest1Doc;

  contest1Doc["distance1"] = "12";
  contest1Doc["distance2"] = "13";
  contest1Doc["distance3"] = "14";
  contest1Doc["distance4"] = "15";
  contest1Doc["distance5"] = "16";
  contest1Doc["distance6"] = "17";

  serializeJson(contest1Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST1_PATH, data.c_str());

  JsonDocument contest2Doc;

  contest2Doc["distance7"] = "18";

  serializeJson(contest2Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());

  JsonDocument contest3Doc;

  contest3Doc["distance8"] = "19";
  contest3Doc["distance9"] = "20";

  serializeJson(contest3Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());

  JsonDocument contest4Doc;

  contest4Doc["distance10"] = "21";
  contest4Doc["distance11"] = "22";

  serializeJson(contest4Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());
}
// Server functionality is provided by NetworkManager now.

// main task
void webServerTask(void *pvParameters)
{
  networkManager.begin();
  networkManager.startServer();
  hardwareManager.serialLog.println("Web server task inited running on core " + String(xPortGetCoreID()));
  uint32_t notifyFromContest;
  Command commandRecv;
  Command commandSend;

  while (true)
  {
    networkManager.handleClient();
    if (networkManager.isNotify())
    {
      commandSend = networkManager.getCommand();
      xQueueSend(queueContest, &commandSend, 0);
      hardwareManager.serialLog.println("SERVER_TASK : receive notify command: " + String(commandSend.key) + " , " + String(commandSend.value));
    }
    if (xQueueReceive(queueServer, &commandRecv, 0) == pdTRUE)
    {
      hardwareManager.serialLog.println("SERVER_TASK : receive notify command: " + String(commandRecv.key) + " , " + String(commandRecv.value));

      networkManager.sendCommand(&commandRecv);
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
void contestTask(void *pvParameters)
{
  contestManager.begin();
  hardwareManager.serialLog.println("Contest task inited running on core " + String(xPortGetCoreID()));
  hardwareManager.serialLog.println("Version: " + String(VERSION));
  bool lastHall;
  bool lastSL;
  unsigned long timer = millis();
  uint32_t notifyType;
  Command commandRecv;
  Command commandSend;

  while (true)
  {
    motor.update();
    hardwareManager.update();
    if (contestManager.isNotify())
    {
      commandSend = contestManager.getCommand();
      xQueueSend(queueServer, &commandSend, 0);
      hardwareManager.serialLog.println("CONTEST_TASK : send notify command to server task: " + String(commandSend.key) + " , " + String(commandSend.value));
    }

    if (xQueueReceive(queueContest, &commandRecv, 0) == pdTRUE)
    {
      hardwareManager.serialLog.println("CONTEST_TASK : receive command from server task: " + String(commandRecv.key) + " , " + String(commandRecv.value));
      switch (commandRecv.key)
      {
      case CONTROL_COMMAND:
        switch (commandRecv.value)
        {
        case START_CONTEST_COMMAND:
          hardwareManager.serialLog.println("Starting contest from contest task...");
          contestManager.addContest(CONTEST_1_ID, 0);
          contestManager.addContest(CONTEST_2_ID, 1);
          contestManager.addContest(CONTEST_3_ID, 2);
          contestManager.addContest(CONTEST_4_ID, 3);
          contestManager.run();
          break;
        case STOP_CONTEST_COMMAND:
          hardwareManager.serialLog.println("Stopping contest from contest task...");
          contestManager.stop();
          break;
        }
        break;
      case MARK_COMMAND:
        hardwareManager.serialLog.println("Mark off contest command received: " + String(commandRecv.value));
        hardwareManager.display.setMark(commandRecv.value);
        contestManager.addLog(0xFF, "Mark off contest: " + String(commandRecv.value));
        break;
      case STATE_COMMAND:
        contestManager.setNotify(STATE_COMMAND, contestManager.getStatus());
        hardwareManager.serialLog.println("Contest status request from web client: " + String(contestManager.getStatus()));
        break;
      }
    }

    // if (motor.getSignelLeft() && motor.lastMotorSignel.signelLeft)
    // {
    //   hardwareManager.serialLog.println("---> signelLeft attack");
    // }
    // motor.lastMotorSignel.signelLeft = motor.getSignelLeft();
    // if (motor.getSignelEngine() && motor.lastMotorSignel.signelEngine)
    // {
    //   hardwareManager.serialLog.println("---> attachLine attack");
    // }
    // motor.lastMotorSignel.signelEngine = motor.getSignelEngine();
    // if (motor.getSensorHall() && motor.lastMotorSignel.sensorHall)
    // {
    //   hardwareManager.serialLog.println("---> sensorHall attack");
    // }
    // motor.lastMotorSignel.sensorHall = motor.getSensorHall();

    if (millis() - timer > 100)
    {
      hardwareManager.display.setEncoder(motor.getEncoderCount() % 2 == 0 ? 0 : 1);
      hardwareManager.display.setHallSensor(motor.getSensorHall());
      hardwareManager.display.setEngine(motor.getSignelEngine());
      hardwareManager.display.setSignalLeft(motor.getSignelLeft());
      hardwareManager.display.setContest(contestManager.getStatus());
      // contestManager.setNotify(STATE_COMMAND, contestManager.getStatus());
      //  hardwareManager.serialLog.println("Encoder count : " + String(motor.getEncoderCount()) + " DeltaTime : " + String(motor.DeltaTimeENC));
      //   hardwareManager.serialLog.println("DateTime : " + hardwareManager.dateTime.toString() + " , Timestamp: " + String(hardwareManager.dateTime.getTimestamp()));
      hardwareManager.display.display();
      timer = millis();
    }
    if (lastRunState != contestManager.isRunning())
    {

      lastRunState = contestManager.isRunning();
      if (lastRunState)
      {
        hardwareManager.serialLog.println("Creating run contest task...");
        xTaskCreatePinnedToCore(runTask, "RunTask", 4096, NULL, 1, &runTaskHandle, 1); // Core 1
      }
      else
      {
        hardwareManager.serialLog.println("Deleting run contest task...");
        vTaskDelete(runTaskHandle);
        runTaskHandle = NULL;
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
void runTask(void *pvParameters)
{
  if (contestManager.isRunning())
  {
    contestManager._runContest();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial0.begin(115200);
  delay(1000);
  Serial.println("Booting... \n Version: " + String(VERSION));

  hardwareManager.begin();
  motor.init();

  xTaskCreatePinnedToCore(webServerTask, "WebServerTask", 8192, NULL, 1, &webServerTaskHandle, 0); // Core 0
  xTaskCreatePinnedToCore(contestTask, "ContestTask", 8192, NULL, 1, &contestTaskHandle, 1);       // Core 1

  queueServer = xQueueCreate(10, sizeof(Command));
  queueContest = xQueueCreate(10, sizeof(Command));
}

void loop()
{
}
