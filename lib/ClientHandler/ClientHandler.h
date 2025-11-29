#pragma once
#include <WiFi.h>
#include <HardwareManager.h>
#include <ContestManager.h>

#define DEFAUT_SERVER_IP "192.168.0.2"
#define DEFAUT_SERVER_PORT "1234"

class ClientHandler
{
private:
    WiFiClient client;
    WiFiServer &server;
    HardwareManager *hardwareManager;
    IPAddress serverIP;
    uint16_t port;
    uint8_t buffer[256];
    uint8_t len;
    bool hardwareConfiged = false;

public:
    ClientHandler(WiFiServer &srv);
    ~ClientHandler();
    void setHardwareAdapter(HardwareManager &hw);
    bool configServer(String ip, String port);
    bool startClient();
    bool startClient(String ip, String port);
    void checkClient();
};
