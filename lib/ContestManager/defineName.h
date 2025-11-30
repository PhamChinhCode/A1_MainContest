#pragma once

// Contest IDs
#define DEFAULT_SSID "PC_NET_MOBILE"
#define DEFAULT_PASSWORD "1234567890"

#define HEADER_LENGTH 10 // 9 byte Header (từ START đến CRC)
#define BYTE_START 0x30
#define BYTE_STOP 0x31
#define BYTE_SET 0x32
#define BYTE_GET 0x33
#define BYTE_PAYLOAD 0x34

#define SOCKET_SERVER_PORT 123
#define FRAME_BUFFER_SIZE 10

#define NOTIFY_RUN_STATE 0x60
#define NOTIFY_RUN_STATE_ON 0x61
#define NOTIFY_RUN_STATE_OFF 0x62

#define CONTEST_1_ID 0x41
#define CONTEST_2_ID 0x42
#define CONTEST_3_ID 0x43
#define CONTEST_4_ID 0x44

#define KEY_NULL 0xff

// Contest Names
#define CONTEST_1_NAME "Đi qua hình số 8"
#define CONTEST_2_NAME "Đi qua vạch đường thẳng"
#define CONTEST_3_NAME "Đi qua đường có vạch cản"
#define CONTEST_4_NAME "Đi qua đường gồ ghề"

// Error IDs
#define ERROR_COMMAND 0xE0

#define ERROR_OVERRIDE_START 0xE1      // đè vạch xuất phát
#define ERROR_RUNOUT_START_TIME 0xEB   // quá thời gian xuất phát
#define ERROR_RUNOUT_CONTEST_TIME 0xE4 // quá thời gian quy định
#define ERROR_GO_WRONG_WAY 0xE5        // đi sai đường
#define ERROR_STOP_ENGINE 0xE8         // chết máy giữa đường thi
#define ERROR_NO_SIGNAL_LEFT_IN 0xEA   // khoong signal trai khi vafo bafi

#define ERROR_OVERRIDE_LINE 0xE2  // đè vạch chướng ngại vật
#define ERROR_FOOT_AT_GROUND 0xE3 // chân chạm đất
#define ERROR_FALL_OF_BIKE 0xE6   // đổ xe
#define ERROR_GO_OUT_LINE 0xE7    // đi ra ngoài vạch
#define ERROR_NO_HELMET 0xE9      // Không đội mũ bảo hiểm

// Contest Control
#define CONTROL_COMMAND 0xA0
#define START_CONTEST_COMMAND 0xA1
#define STOP_CONTEST_COMMAND 0xA2
#define READY_CONTEST_COMMAND 0xA3

// mark off contest
#define MARK_COMMAND 0xB0

// Camera command
#define CAMERA_COMMAND 0xB1

// Contest Status
#define STATE_COMMAND 0xc0
#define STATE_READY_COMMAND 0xC1
#define STATE_RUNNING_COMMAND 0xC2
#define STATE_FREE_COMMAND 0xC3
#define STATE_CONTEST1_RUNNING 0xC4
#define STATE_CONTEST2_RUNNING 0xC5
#define STATE_CONTEST3_RUNNING 0xC6
#define STATE_CONTEST4_RUNNING 0xC7
