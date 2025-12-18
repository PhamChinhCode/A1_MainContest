
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
    _delta[0] = 13.9;
    _delta[1] = 10.6;
    _delta[2] = 27.2;
    _delta[3] = 13.1;
    _delta[4] = 28.3;
    _delta[5] = 7.7;
    _delta[6] = 26.9;
    _delta[7] = 16.7;
    _delta[8] = 10.8;
    _delta[9] = 11.9;
    _delta[10] = 6.1;
    _delta[11] = 82.4;
    _delta[12] = 27.8;
    _delta[13] = 6.3;
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

    for (int i = 0; i < TOTAL_DISTANCE + 1; i++)
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
    // if (errorID != ERROR_GO_WRONG_WAY)
    setNotify(ERROR_COMMAND, errorID);
    switch (errorID)
    {
    case ERROR_OVERRIDE_START:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_OVERRIDE_START");
        errOverrideStart++;
        break;
    case ERROR_RUNOUT_START_TIME:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_RUNOUT_START_TIME");
        errRunoutTimeStart++;
        break;
    case ERROR_RUNOUT_CONTEST_TIME:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_RUNOUT_CONTEST_TIME");
        errRunoutTimeContest++;
        break;
    case ERROR_GO_WRONG_WAY:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_GO_WRONG_WAY");
        errGoWrongWay++;
        break;
    case ERROR_STOP_ENGINE:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_STOP_ENGINE");
        errStopEngine++;
        break;
    case ERROR_NO_SIGNAL_LEFT_IN:
        hardwareManager->serialLog.println("\t\t!> Error: ERROR_NO_SIGNAL_LEFT_IN");
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
    if (type != ERROR_COMMAND)
        hardwareManager->serialScreen.sendCommand(&command);
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
            hardwareManager->serialLog.println("\n==>: Start " + String(this->_contestName[0]));
            this->_contest1Runer();
            hardwareManager->serialLog.println("<==: Stop " + String(this->_contestName[0]));
            break;
        case CONTEST_2_ID:
            hardwareManager->serialLog.println("\n==>: Start " + String(this->_contestName[1]));
            this->_contest2Runer();
            hardwareManager->serialLog.println("<==: Stop " + String(this->_contestName[1]));
            break;
        case CONTEST_3_ID:
            hardwareManager->serialLog.println("\n==>: Start " + String(this->_contestName[2]));
            this->_contest3Runer();
            hardwareManager->serialLog.println("<==: Stop " + String(this->_contestName[2]));
            break;
        case CONTEST_4_ID:
            hardwareManager->serialLog.println("\n==>: Start " + String(this->_contestName[3]));
            this->_contest4Runer();
            hardwareManager->serialLog.println("<==: Stop " + String(this->_contestName[3]));
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
    // addLog(CONTEST_1_ID, "start contest 1");

    bool lastSensorHall = false;
    bool lastEngine = true;
    int lastHallCount = -1;
    int32_t maxDistance = 0;
    int32_t minDistance = 0;

    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;

    startTime = millis();
    timerRunContest = millis();
    hallCount = 0;
    maxSpeed = 0;

    // addError(ERROR_NO_SIGNAL_LEFT_IN);

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    // chờ tín hiệu đi qua vạch xuất phát
    while (!motor.getSensorHall())
    {
        if (millis() - timerRunContest > 120000)
        {
            addError(ERROR_RUNOUT_START_TIME);
            timerRunContest = millis();
        }
        // loại bỏ lỗi đè vạch start
        if (errOverrideStart == 1)
        {
            errOverrideStart = 0;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    setStatus(STATE_CONTEST1_RUNNING);

    // lỗi không xi nhan trái
    if (!motor.getSignelLeft())
    {
        addError(ERROR_NO_SIGNAL_LEFT_IN);
    }
    // lỗi đè vạch xuất phát
    if (errOverrideStart == 1)
    {
        errOverrideStart = 0;
        addError(ERROR_OVERRIDE_START);
    }

    lastSensorHall = motor.getSensorHall();
    lastEngine = motor.getSignelEngine();
    lastDistance = motor.getDistance();
    // bắt đầu tính giờ thi
    timerRunContest = millis();

    maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    hardwareManager->serialLog.println("\t--> B1 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
    // bắt đầu bài thi
    while (1)
    {
        if (motor.getSpeed() > maxSpeed)
            maxSpeed = motor.getSpeed();
        bool curHall = motor.getSensorHall();
        deltaDistance = motor.getDistance() - lastDistance;
        // check error way always
        if (lastHallCount != hallCount && deltaDistance > maxDistance)
        {
            addError(ERROR_GO_WRONG_WAY);
            lastHallCount = hallCount;
        }
        // check distance while in hal trigger
        if (curHall && !lastSensorHall && deltaDistance > 20)
        {
            if (deltaDistance < minDistance || deltaDistance > maxDistance)
            {
                addError(ERROR_GO_WRONG_WAY);
            }
            hardwareManager->serialLog.println("\t<-- B1 => dD" + String(hallCount) + ": " + String(deltaDistance) + "\tMaxSp: " + String(maxSpeed) + "\n");
            // update new ampli of distance
            hallCount++;
            // reset for new Distance
            if (hallCount < 9)
            {
                maxSpeed = 0;
                lastDistance = motor.getDistance();
                maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                hardwareManager->serialLog.println("\t--> B1 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
            }
        }
        lastSensorHall = curHall;

        // lỗi chết máy
        if (!motor.getSignelEngine() && lastEngine)
        {
            addError(ERROR_STOP_ENGINE);
        }
        lastEngine = motor.getSignelEngine();

        // lỗi quá thời gian bài thi
        if (millis() - timerRunContest > 600000)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            timerRunContest = millis();
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
        if (hallCount == 9)
            return true;
        if (hallCount > 9)
            return false;
    }
}
bool ContestManager::_contest2Runer()
{
    // addLog(CONTEST_2_ID, "start contest 2");
    setStatus(STATE_CONTEST2_RUNNING);
    bool lastSensorHall = false;
    bool lastEngine = true;
    int lastHallCount = -1;
    int32_t maxDistance = 0;
    int32_t minDistance = 0;

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
    hallCount = 9;
    maxSpeed = 0;

    lastSensorHall = motor.getSensorHall();
    lastEngine = motor.getSignelEngine();
    lastDistance = motor.getDistance();
    maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));

    hardwareManager->serialLog.println("\t--> B2 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
    while (1)
    {
        if (motor.getSpeed() > maxSpeed)
            maxSpeed = motor.getSpeed();
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        // check error way always
        if (lastHallCount != hallCount && deltaDistance > maxDistance)
        {
            addError(ERROR_GO_WRONG_WAY);
            lastHallCount = hallCount;
        }
        // check distance while in hal trigger
        if (curHall && !lastSensorHall && deltaDistance > 20)
        {
            if (deltaDistance < minDistance || deltaDistance > maxDistance)
            {
                addError(ERROR_GO_WRONG_WAY);
            }
            hardwareManager->serialLog.println("\t<-- B2 => dD" + String(hallCount) + ": " + String(deltaDistance) + "\tMaxSp: " + String(maxSpeed));
            // update new ampli of distance
            hallCount++;
            // reset for new Distance
            if (hallCount < 11)
            {
                maxSpeed = 0;
                lastDistance = motor.getDistance();
                maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                hardwareManager->serialLog.println("\t--> B2 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
            }
        }
        lastSensorHall = curHall;

        // lỗi chết máy
        if (!motor.getSignelEngine() && lastEngine)
        {
            addError(ERROR_STOP_ENGINE);
        }
        lastEngine = motor.getSignelEngine();

        // lỗi quá thời gian bài thi
        if (millis() - timerRunContest > 600000)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            timerRunContest = millis();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 11)
            return true;
        if (hallCount > 11)
            return false;
    }
}
bool ContestManager::_contest3Runer()
{
    // addLog(CONTEST_3_ID, "start contest 3");
    setStatus(STATE_CONTEST3_RUNNING);
    bool lastSensorHall = false;
    bool lastEngine = true;
    int lastHallCount = -1;
    int32_t maxDistance = 0;
    int32_t minDistance = 0;

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
    hallCount = 11;
    maxSpeed = 0;
    lastSensorHall = motor.getSensorHall();
    lastEngine = motor.getSignelEngine();
    lastDistance = motor.getDistance();
    maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    hardwareManager->serialLog.println("\t--> B3 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
    while (1)
    {
        if (motor.getSpeed() > maxSpeed)
            maxSpeed = motor.getSpeed();
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        // check error way always
        if (lastHallCount != hallCount && deltaDistance > maxDistance)
        {
            addError(ERROR_GO_WRONG_WAY);
            lastHallCount = hallCount;
        }
        // check distance while in hal trigger
        if (curHall && !lastSensorHall && deltaDistance > 20)
        {
            if (deltaDistance < minDistance || deltaDistance > maxDistance)
            {
                addError(ERROR_GO_WRONG_WAY);
            }
            hardwareManager->serialLog.println("\t<-- B3 => dD" + String(hallCount) + ": " + String(deltaDistance) + "\tMaxSp: " + String(maxSpeed));
            // update new ampli of distance
            hallCount++;
            // reset for new Distance
            if (hallCount < 13)
            {
                maxSpeed = 0;
                lastDistance = motor.getDistance();
                maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                hardwareManager->serialLog.println("\t--> B3 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
            }
        }
        lastSensorHall = curHall;

        // lỗi chết máy
        if (!motor.getSignelEngine() && lastEngine)
        {
            addError(ERROR_STOP_ENGINE);
        }
        lastEngine = motor.getSignelEngine();

        // lỗi quá thời gian bài thi
        if (millis() - timerRunContest > 600000)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            timerRunContest = millis();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 13)
            return true;
        if (hallCount > 13)
            return false;
    }
}
bool ContestManager::_contest4Runer()
{
    // addLog(CONTEST_4_ID, "start contest 4");
    setStatus(STATE_CONTEST4_RUNNING);
    bool lastSensorHall = false;
    bool lastEngine = true;
    int lastHallCount = -1;
    int32_t maxDistance = 0;
    int32_t minDistance = 0;

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
    hallCount = 13;
    maxSpeed = 0;
    lastSensorHall = motor.getSensorHall();
    lastEngine = motor.getSignelEngine();
    lastDistance = motor.getDistance();
    maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
    hardwareManager->serialLog.println("\t--> B4 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
    while (1)
    {
        if (motor.getSpeed() > maxSpeed)
            maxSpeed = motor.getSpeed();
        deltaDistance = motor.getDistance() - lastDistance;
        bool curHall = motor.getSensorHall();
        // check error way always
        if (lastHallCount != hallCount && deltaDistance > maxDistance)
        {
            addError(ERROR_GO_WRONG_WAY);
            lastHallCount = hallCount;
        }
        // check distance while in hal trigger
        if (curHall && !lastSensorHall && deltaDistance > 20)
        {
            if (deltaDistance < minDistance || deltaDistance > maxDistance)
            {
                addError(ERROR_GO_WRONG_WAY);
            }
            hardwareManager->serialLog.println("\t<-- B4 => dD" + String(hallCount) + ": " + String(deltaDistance) + "\tMaxSp: " + String(maxSpeed));
            // update new ampli of distance
            hallCount++;
            // reset for new Distance
            if (hallCount < 14)
            {
                maxSpeed = 0;
                lastDistance = motor.getDistance();
                maxDistance = _distance[hallCount] + (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                minDistance = _distance[hallCount] - (20 + (_distance[hallCount] * _delta[hallCount] / 100.0));
                hardwareManager->serialLog.println("\t--> B4 => sD" + String(hallCount) + ": " + String(_distance[hallCount]));
            }
        }
        lastSensorHall = curHall;

        // lỗi chết máy
        if (!motor.getSignelEngine() && lastEngine)
        {
            addError(ERROR_STOP_ENGINE);
        }
        lastEngine = motor.getSignelEngine();

        // lỗi quá thời gian bài thi
        if (millis() - timerRunContest > 600000)
        {
            addError(ERROR_RUNOUT_CONTEST_TIME);
            timerRunContest = millis();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 14)
            return true;
        if (hallCount > 14)
            return false;
    }
    // hardwareManager->serialLog.println("Kết thúc cuộc thi 4: Max speed = " + String(maxSpeed) + " m/s");
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