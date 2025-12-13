
#include <ContestManager.h>
#include <FrameConvert.h>

TaskHandle_t _contestHandle = NULL;

ContestManager::ContestManager(HardwareManager &hwManager)
{
    this->hardwareManager = &hwManager;
    this->_isRunning = false;
    for (int i = 0; i < 4; i++)
    {
        this->_contestIndex[i] = 0;
    }
}
ContestManager::~ContestManager()
{
}
void ContestManager::begin()
{
    this->_isRunning = false;
    this->_startTime = 0;
    for (int i = 0; i < 4; i++)
    {
        this->_contestIndex[i] = 0;
    }
    setStatus(STATE_FREE_COMMAND);
}
void ContestManager::setQueue(QueueHandle_t *queue)
{
    this->_queue = queue;
}
void ContestManager::sendCommandQueue(Command cmd)
{
    if (_queue == NULL)
        return;
    xQueueSend(*_queue, &cmd, 5000 / portTICK_PERIOD_MS);
}
void ContestManager::loadConfig(String str)
{
    JsonDocument doc;
    deserializeJson(doc, str.c_str());
    hardwareManager->serialLog.println("Load config:");

    for (int i = 0; i < 11; i++)
    {
        String key = "distance" + String(i + 1);
        if (!doc[key].isNull())
        {
            this->_distance[i] = doc[key];
        }
        else
        {
            this->_distance[i] = 0;
        }
        hardwareManager->serialLog.print("  D" + String(i) + ": " + String(this->_distance[i]));
    }
    hardwareManager->serialLog.println(" ");
}
bool ContestManager::addContest(uint8_t contestID, uint8_t index)
{
    bool added = false;

    if (index < 4 && (contestID == CONTEST_1_ID || contestID == CONTEST_2_ID || contestID == CONTEST_3_ID || contestID == CONTEST_4_ID))
    {
        _contestIndex[index] = contestID;
        added = true;
    }
    return added;
}
// void ContestManager::_Runner(void *pvParameters)
// {
//     ContestManager *self = static_cast<ContestManager *>(pvParameters);

//     // xử lý logic
//     while (1)
//     {
//         self->_runContest(); // ví dụ gọi hàm thành viên
//         vTaskDelay(1 / portTICK_PERIOD_MS);
//     }
// }
bool ContestManager::run()
{
    if (!_isRunning)
    {
        //_contestHandle = NULL;
        //_isRunning = xTaskCreatePinnedToCore(_Runner, "ContestRuner", 4096, this, 1, &_contestHandle, 1);
        _isRunning = true;
        _logPath = String(LOG_PATH) + "/" + String(hardwareManager->dateTime.getTimestamp()) + ".txt";
        addLog(START_CONTEST_COMMAND, "Start contest");
        loadConfig(hardwareManager->microSD.readData(CONFIG_CONTEST1_PATH));
    }
    hardwareManager->serialLog.println("Contest run: " + String(_isRunning ? "true" : "false"));
    return _isRunning;
}
void ContestManager::stop()
{
    if (this->_isRunning)
    {
        setNotify(CONTROL_COMMAND, STOP_CONTEST_COMMAND);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        setStatus(STATE_FREE_COMMAND);
        addLog(STOP_CONTEST_COMMAND, "Stop contest");

        this->_isRunning = false;
    }
}
void ContestManager::addError(uint8_t errorID)
{
    setNotify(ERROR_COMMAND, errorID);
    switch (errorID)
    {
    case ERROR_OVERRIDE_START:
        hardwareManager->serialLog.println("Error added: ERROR_OVERRIDE_START");
        errOverrideStart++;
        break;
    case ERROR_RUNOUT_START_TIME:
        hardwareManager->serialLog.println("Error added: ERROR_RUNOUT_START_TIME");
        errRunoutTimeStart++;
        break;
    case ERROR_RUNOUT_CONTEST_TIME:
        hardwareManager->serialLog.println("Error added: ERROR_RUNOUT_CONTEST_TIME");
        errRunoutTimeContest++;
        break;
    case ERROR_GO_WRONG_WAY:
        hardwareManager->serialLog.println("Error added: ERROR_GO_WRONG_WAY");
        errGoWrongWay++;
        break;
    case ERROR_STOP_ENGINE:
        hardwareManager->serialLog.println("Error added: ERROR_STOP_ENGINE");
        errStopEngine++;
        break;
    case ERROR_NO_SIGNAL_LEFT_IN:
        hardwareManager->serialLog.println("Error added: ERROR_NO_SIGNAL_LEFT_IN");
        errNoSignalLeft++;
        break;
    default:
        break;
    }
}
bool ContestManager::isNotify()
{
    return _notify;
}
uint8_t ContestManager::getNotifyKey()
{
    _notify = false;
    return _notifyType;
}
uint32_t ContestManager::getNotifyValue()
{
    _notify = false;
    return _notifiValue;
}
Command ContestManager::getCommand()
{
    _notify = false;

    return command;
}
void ContestManager::setNotify(uint8_t type, uint32_t val)
{
    _notify = true;
    _notifyType = type;
    _notifiValue = val;
    command.key = type;
    command.value = val;
    sendCommandQueue(command);
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
void ContestManager::setStatus(uint8_t status)
{
    this->status = status;
    hardwareManager->serialLog.println("Status: " + String(status, HEX));
    setNotify(STATE_COMMAND, status);
}
uint8_t ContestManager::getStatus()
{
    return this->status;
}
void ContestManager::_runContest()
{
    // reset biến đếm và time
    hallCount = 0;
    startTime = millis();
    errOverrideStart = 0;
    errRunoutTimeStart = 0;
    errRunoutTimeContest = 0;
    errGoWrongWay = 0;
    errStopEngine = 0;
    errNoSignalLeft = 0;
    setStatus(STATE_READY_COMMAND);

    for (int i = 0; i < 4; i++)
    {
        switch (this->_contestIndex[i])
        {
        case CONTEST_1_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[0]));
            this->_contest1Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[0]));
            break;
        case CONTEST_2_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[1]));
            this->_contest2Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[1]));
            break;
        case CONTEST_3_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[2]));
            this->_contest3Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[2]));
            break;
        case CONTEST_4_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[3]));
            this->_contest4Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[3]));
            break;
        default:
            break;
        }
    }

    _logPath = "";
    this->stop();
}
// contest runer

bool ContestManager::_contest1Runer()
{
    addLog(CONTEST_1_ID, "start contest 1");

    bool lastSensorHall = false;
    hallCount = 0;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    startTime = millis();

    // addError(ERROR_NO_SIGNAL_LEFT_IN);

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    // chờ tín hiệu đi qua vạch xuất phát
    while (!motor.getSensorHall() && errRunoutTimeStart == 0)
    {
        if (millis() - startTime > 600000 && errRunoutTimeStart == 0)
        {
            addError(ERROR_RUNOUT_START_TIME);
            hardwareManager->serialLog.println("--> Contest1: ERROR_RUNOUT_START_TIME");
        }
        // loại bỏ lỗi đè vạch start
        if (errOverrideStart == 1)
        {
            errOverrideStart = 0;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    setStatus(STATE_CONTEST1_RUNNING);
    // bắt đầu tính giờ thi
    startTime = millis();

    // lỗi không xi nhan trái
    if (!motor.getSignelLeft() && errNoSignalLeft == 0)
    {
        addError(ERROR_NO_SIGNAL_LEFT_IN);
    }
    // lỗi đè vạch xuất phát
    if (errOverrideStart == 1)
    {
        addError(ERROR_OVERRIDE_START);
    }

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    // bắt đầu bài thi
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        if (curHall && !lastSensorHall && deltaDistance > 150)
        {
            lastDistance = motor.getDistance();
            if (inDeltaValue(deltaDistance, _distance[hallCount], 50 + _distance[hallCount] * 0.5))
            {
                // pass
            }
            else
            {
                addError(ERROR_GO_WRONG_WAY);
                hardwareManager->serialLog.println("--> Contest1: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest1: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = curHall;

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            errRunoutTimeContest = 1;
            hardwareManager->serialLog.println("--> Contest1: ERROR_RUNOUT_CONTEST_TIME");
            // hallCount = 4; // kết thúc bài thi
        }

        if (hallCount == 6)
        {
            return true;
        }
        if (hallCount > 6)
        {
            return false;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
bool ContestManager::_contest2Runer()
{
    addLog(CONTEST_2_ID, "start contest 2");
    setStatus(STATE_CONTEST2_RUNNING);
    bool lastSensorHall = false;
    hallCount = 6;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();
    // errRunoutTimeContest = 0;
    // startTime = millis();
    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        if (curHall && !lastSensorHall && deltaDistance > 50)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 50 + _distance[hallCount] * 0.5))
            {
                // pass
            }
            else
            {
                addError(ERROR_GO_WRONG_WAY);
                hardwareManager->serialLog.println("--> Contest2: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest2: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = curHall;

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            errRunoutTimeContest = 1;
            hardwareManager->serialLog.println("--> Contest2: ERROR_RUNOUT_CONTEST_TIME");
            // hallCount = 5; // kết thúc bài thi
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 8)
            return true;
        if (hallCount > 8)
            return false;
    }
}
bool ContestManager::_contest3Runer()
{
    addLog(CONTEST_3_ID, "start contest 3");
    setStatus(STATE_CONTEST3_RUNNING);
    bool lastSensorHall = false;
    hallCount = 8;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();
    // errRunoutTimeContest = 0;
    // startTime = millis();
    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        if (curHall && !lastSensorHall && deltaDistance > 150)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 50 + _distance[hallCount] * 0.5))
            {
                // pass
            }
            else
            {
                addError(ERROR_GO_WRONG_WAY);
                hardwareManager->serialLog.println("--> Contest3: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest3: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = curHall;

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            errRunoutTimeContest = 1;
            hardwareManager->serialLog.println("--> Contest3: ERROR_RUNOUT_CONTEST_TIME");
            // hallCount = 7; // kết thúc bài thi
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 10)
            return true;
        if (hallCount > 10)
            return false;
    }
}
bool ContestManager::_contest4Runer()
{

    addLog(CONTEST_4_ID, "start contest 4");
    setStatus(STATE_CONTEST4_RUNNING);
    bool lastSensorHall = false;
    hallCount = 10;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();
    // errRunoutTimeContest = 0;
    // startTime = millis();
    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        if (curHall && !lastSensorHall && deltaDistance > 150)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 50 + _distance[hallCount] * 0.5))
            {
                // pass
            }
            else
            {
                addError(ERROR_GO_WRONG_WAY);
                hardwareManager->serialLog.println("--> Contest4: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest4: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = curHall;

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            errRunoutTimeContest = 1;
            hardwareManager->serialLog.println("--> Contest4: ERROR_RUNOUT_CONTEST_TIME");
            // hallCount = 9; // kết thúc bài thi
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 11)
            return true;
        if (hallCount > 11)
            return false;
    }
}
void ContestManager::addLog(uint8_t eventID, String description)
{
    if (_isRunning && _logPath.length() > 0)
    {
        _logEntries.counter++;
        _logEntries.eventID = eventID;
        _logEntries.description = description;
        _logEntries.timestamp = hardwareManager->dateTime.getTimestamp();

        String logEntry = String(_logEntries.counter) + "," + String(_logEntries.eventID, HEX) + "," + _logEntries.description + "," + String(_logEntries.timestamp) + "\n";
        hardwareManager->serialLog.println(logEntry);
        // hardwareManager->microSD.addData(_logPath.c_str(), logEntry.c_str());
    }
}
bool ContestManager::inDeltaValue(uint32_t value, uint32_t target, uint32_t delta)
{
    return (value >= (target - delta)) && (value <= (target + delta));
}
// void ContestManager::