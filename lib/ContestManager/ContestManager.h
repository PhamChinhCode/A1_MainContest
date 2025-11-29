#pragma once
#include <Myconfig.h>
#include "defineName.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Motorbike.h>
#include <LogUtil.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <HardwareManager.h>
#include <FrameConvert.h>

extern TaskHandle_t _contestHandle;

class ContestManager
{
private:
    HardwareManager *hardwareManager;
    uint8_t _contestIndex[4];
    const char *_contestName[4] = {
        "Đi qua hình số 8",
        "Đi qua vạch đường thẳng",
        "Đi qua đường có vạch cản",
        "Đi qua đường gồ ghề"};
    bool _isRunning;
    unsigned long _startTime;

    struct LogEntry
    {
        uint16_t counter;
        uint8_t eventID;
        String description;
        uint32_t timestamp;
    };
    String _logPath;
    LogEntry _logEntries;

    uint32_t _distance[11];
    uint32_t startTime = 0;
    uint8_t hallCount = 0;
    uint8_t errOverrideStart = 0;
    uint8_t errRunoutTimeStart = 0;
    uint8_t errRunoutTimeContest = 0;
    uint8_t errGoWrongWay = 0;
    uint8_t errStopEngine = 0;
    uint8_t errNoSignalLeft = 0;

    bool _notify = false;
    uint8_t _notifyType = 0;
    uint32_t _notifiValue = 0;
    Command command;

    uint8_t status;

    static void _Runner(void *pvParameters);

    bool _contest1Runer();
    bool _contest2Runer();
    bool _contest3Runer();
    bool _contest4Runer();
    bool inDeltaValue(uint32_t value, uint32_t target, uint32_t delta);
    void addError(uint8_t errorID);
    void setStatus(uint8_t status);

public:
    ContestManager(HardwareManager &hwManager);
    ~ContestManager();
    void _runContest();
    void begin();
    void loadConfig(String str);
    bool run();
    void stop();
    bool addContest(uint8_t contestID, uint8_t index);
    bool isRunning() { return _isRunning; }
    void removeContest(int index);
    void clearAllContest();
    void addLog(uint8_t eventID, String description);

    void setNotify(uint8_t type, uint32_t val);
    uint8_t getNotifyKey();
    uint32_t getNotifyValue();
    Command getCommand();
    bool isNotify();
    uint8_t getStatus();
};
