#pragma once
#include <Myconfig.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <HardwareManager.h>
#include <ClientHandler.h>
#include <ContestManager.h>
#include <FrameConvert.h>
class NetworkManager
{
private:
    WebServer server;
    WiFiServer socketServer;
    WiFiClient socketClient;
    ClientHandler clientHandler;
    FrameConvert frameConverter;
    HardwareManager *hardwareManager;
    float otaDone = 0.0f;
    unsigned long timerConnectWifi = 0;
    bool _notify = false;
    uint8_t _notifyType = 0;
    uint32_t _notifiValue = 0;
    uint8_t buffer[256];
    uint8_t len;
    Command command;
    void setNotify(uint8_t type, uint32_t val);

public:
    NetworkManager(HardwareManager &hwManager);
    ~NetworkManager();
    bool begin();
    void startServer();
    void handleClient();
    void handleFileRequest();
    void handleData(HTTPMethod method);
    void handleCamera();
    void handleControl();
    void handleServer(HTTPMethod method);
    void handleUpdate();
    void handleUpdateEnd();
    void handleSocketClient();

    void sendCommand(Command *cmd, uint8_t stopByte = BYTE_STOP);
    void sendImage();

    uint8_t getNotifyKey();
    uint32_t getNotifyValue();
    Command getCommand();
    bool isNotify();
};
