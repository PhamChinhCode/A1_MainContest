#include "ClientHandler.h"
#include <FrameConvert.h>

ClientHandler::ClientHandler(WiFiServer &srv) : server(srv)
{
    serverIP.fromString(DEFAUT_SERVER_IP);
}
ClientHandler::~ClientHandler()
{
}
void ClientHandler::setHardwareAdapter(HardwareManager &hw)
{
    hardwareManager = &hw;
    hardwareConfiged = true;
}
bool ClientHandler::configServer(String ip, String p)
{
    port = p.toInt();
    return serverIP.fromString(ip);
}
bool ClientHandler::startClient()
{
    return client.connect(serverIP, port);
}
bool ClientHandler::startClient(String ip, String p)
{
    configServer(ip, p);
    return client.connect(serverIP, port);
}
void ClientHandler::checkClient()
{
    if (server.hasClient())
    {
        client = server.available();
    }
    if (!client || !client.connected())
        return;

    if (client.available() >= FRAME_BUFFER_SIZE)
    {
        len = client.readBytes(buffer, sizeof(FRAME_BUFFER_SIZE));

        FrameConvert frameConverter;
        frameConverter.setFrame(buffer, len);
        uint8_t key = frameConverter.getKey();
        uint32_t value = frameConverter.getValue();
    }
}