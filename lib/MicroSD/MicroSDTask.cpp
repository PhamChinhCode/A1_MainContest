#include "MicroSDTask.h"
MicroSDTask::MicroSDTask()
{
    // create mutex for SD card access
    _sdMutex = xSemaphoreCreateMutex();
}
bool MicroSDTask::begin()
{
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5))
    {
        _isMounted = false;
    }
    // cardtye  = CARD_NONE, CARD_MMC, CARD_SD, CARD_SDHC
    _cardType = SD_MMC.cardType();
    _cardSize = SD_MMC.cardSize() / (1024 * 1024);
    _isMounted = true;
    _error = "";
    return _isMounted;
}
String MicroSDTask::readData(const char *path)
{
    _error = "";
    String data;
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return "";
    }
    // protect file access
    if (!lock(5000))
    { // wait up to 5s
        _error = "Failed to obtain SD mutex";
        return "";
    }
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        unlock();
        _error = "Failed to open file for reading";
        return "";
    }
    while (file.available())
    {
        data += (char)file.read();
    }
    file.flush();
    file.close();
    unlock();
    return data;
}
bool MicroSDTask::writeData(const char *path, const char *data)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return false;
    }
    if (!lock(5000))
    {
        _error = "Failed to obtain SD mutex";
        return false;
    }
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        unlock();
        _error = "Failed to open file for writing";
        return false;
    }
    bool isSuccess = false;
    if (file.print(data))
    {
        isSuccess = true;
    }
    else
    {
        _error = "Write failed";
        isSuccess = false;
    }
    file.flush();
    file.close();
    unlock();
    return isSuccess;
}
bool MicroSDTask::addData(const char *path, const char *data)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return false;
    }
    if (!lock(5000))
    {
        _error = "Failed to obtain SD mutex";
        return false;
    }
    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file)
    {
        unlock();
        _error = "Failed to open file for appending";
        return false;
    }
    bool isSuccess = false;
    if (file.print(data))
    {
        isSuccess = true;
    }
    else
    {
        _error = "Append failed";
        isSuccess = false;
    }
    file.flush();
    file.close();
    unlock();
    return isSuccess;
}
File MicroSDTask::openFile(const char *path, const char *mode)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return File();
    }
    // Caller receives a File and is responsible for calling closeFile()
    // We still protect the open operation itself, but keep the file open
    // so the caller can operate on it. The mutex remains available for other
    // operations; to protect the file during caller usage they should call
    // lock()/unlock() around operations or use closeFile which will flush/close.
    if (!lock(5000))
    {
        _error = "Failed to obtain SD mutex";
        return File();
    }
    File file = SD_MMC.open(path, mode);
    if (!file)
    {
        _error = "Failed to open file";
        unlock();
    }
    return file;
}
void MicroSDTask::closeFile(File &file)
{
    if (file)
    {

        file.flush();
        file.close();
    }
    unlock();
}

bool MicroSDTask::lock(uint32_t timeoutMs)
{
    if (!_sdMutex)
        return false;
    TickType_t ticks = (timeoutMs == portMAX_DELAY) ? portMAX_DELAY : pdMS_TO_TICKS(timeoutMs);
    _isLock = xSemaphoreTake(_sdMutex, ticks) == pdTRUE;
    return _isLock;
}

void MicroSDTask::unlock()
{
    if (!_sdMutex)
        return;
    if (_isLock)
        xSemaphoreGive(_sdMutex);
}