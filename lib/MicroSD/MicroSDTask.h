#ifndef MICROSDTASK_H
#define MICROSDTASK_H

#include <Myconfig.h>
#include <Arduino.h>
#include "SD_MMC.h"
#include "FS.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define SD_MMC_CMD 38 // Please do not modify it.
#define SD_MMC_CLK 39 // Please do not modify it.
#define SD_MMC_D0 40  // Please do not modify it.

#define WEBSERVER_HTML_PATH "/Webserver/index.html"
#define WEBSERVER_CSS_PATH "/Webserver/style.css"
#define WEBSERVER_JS_PATH "/Webserver/script.js"

#define CONFIG_CONTEST1_PATH "/Contest/contest1.txt"
#define CONFIG_CONTEST2_PATH "/Contest/contest2.txt"
#define CONFIG_CONTEST3_PATH "/Contest/contest3.txt"
#define CONFIG_CONTEST4_PATH "/Contest/contest4.txt"

#define SYSTEM_LOG_PATH "/System/log.txt"
#define SYSTEM_WIFI_CONFIG_PATH "/System/wifi.txt"

#define LOG_PATH "/Log"

class MicroSDTask
{
private:
    uint8_t _cardType;
    uint64_t _cardSize;
    bool _isMounted;
    String _error;
    bool _isLock = false;
    SemaphoreHandle_t _sdMutex; ///< mutex to protect SD card access

public:
    MicroSDTask();
    bool begin();
    /**
     * Acquire the SD mutex. Timeout in ms (portMAX_DELAY to wait forever).
     * Returns true if the lock was obtained.
     */
    bool lock(uint32_t timeoutMs = portMAX_DELAY);

    /**
     * Release the SD mutex.
     */
    void unlock();
    String readData(const char *path);
    bool writeData(const char *path, const char *data);
    bool addData(const char *path, const char *data);
    String getError() { return _error; }
    File openFile(const char *path, const char *mode = FILE_READ);
    void closeFile(File &file);
};

#endif // MICROSDTASK_H