#ifndef MOTORBIKE_H
#define MOTORBIKE_H

#include <Myconfig.h>
#include <Arduino.h>

#define MOTORBITE_SPEED
#define MOTORBITE_DIRECTION
#define MOTORBITE_SIGNEL
#define MOTORBITE_SENSOR

#define ONE_SIGNEL_ENCODER_MODE 0
#define TWO_SIGNEL_ENCODER_MODE 1
#define ENCODER_SCALE 10
#define TIMING_UPDATE_SPEED 200

#define HALL_PIN 41
#define SIGNEL_LEFT_PIN 42
#define ENGINE_PIN 2
#define ENCODER_A_PIN 21
#define ENCODER_B_PIN 48
#define ENCODER_I_PIN 45

struct MotorSignel
{
    bool attachLine;                // tisn hieu cam bien ong hoi (tu may tinh)
    bool signelLeft;                // tin hieu cam bien xi nhan trai
    bool sensorHall;                // tin hieu cam bien tu
    bool signelEngine;              // tin hieu may no cua xe
    volatile uint32_t encoderCount; // pulse
    uint32_t distance;              // cm
    uint32_t speed;                 // cm/s
};

class Motorbike
{
private:
    uint8_t hall_pin;
    uint8_t signel_left_pin;
    uint8_t engine_pin;
    uint8_t encoder_a_pin;
    uint8_t encoder_b_pin;
    bool mode_encoder;
    bool attachLine;                // tisn hieu cam bien ong hoi (tu may tinh)
    bool signelLeft;                // tin hieu cam bien xi nhan trai
    bool sensorHall;                // tin hieu cam bien tu
    bool signelEngine;              // tin hieu may no cua xe
    volatile uint32_t encoderCount; // pulse
    uint32_t distance;              // cm
    uint32_t speed;                 // cm/s
    uint32_t lastTime;
    uint32_t lastTimeSignalLeft;
    uint32_t lastTimeEngine;
    uint32_t LastTimeEncoder;

    static void IRAM_ATTR attachPhaseA(void *arg);
    static void IRAM_ATTR attachPhaseB(void *arg);

public:
    uint32_t DeltaTimeEncoder;
    uint32_t DeltaTimeENC;
    MotorSignel lastMotorSignel;
    Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t ENA_PIN, uint8_t ENB_PIN);
    Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t F_PIN);
    ~Motorbike();

    void init();
    void update();
    void setAttachLine(bool attach);

    bool getSignelLeft();
    bool getSensorHall();
    bool getSignelEngine();

    uint32_t getEncoderCount();
    uint32_t getDistance();
    uint32_t getSpeed();
};

extern Motorbike motor;
#endif // MOTORBIKE_H