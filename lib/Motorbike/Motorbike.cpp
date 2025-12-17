#include "Motorbike.h"

Motorbike::Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t ENA_PIN, uint8_t ENB_PIN)
{
    this->hall_pin = H_PIN;
    this->signel_left_pin = SL_PIN;
    this->engine_pin = EN_PIN;
    this->encoder_a_pin = ENA_PIN;
    this->encoder_b_pin = ENB_PIN;
    this->mode_encoder = TWO_SIGNEL_ENCODER_MODE;
}
Motorbike::Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t F_PIN)
{
    this->hall_pin = H_PIN;
    this->signel_left_pin = SL_PIN;
    this->engine_pin = EN_PIN;
    this->encoder_a_pin = F_PIN;
    this->mode_encoder = ONE_SIGNEL_ENCODER_MODE;
}

Motorbike::~Motorbike()
{
}
void Motorbike::attachPhaseA(void *arg)
{
    Motorbike *m = static_cast<Motorbike *>(arg);
    m->DeltaTimeEncoder = micros() - m->LastTimeEncoder;
    if (m->DeltaTimeEncoder > 1000)
    {
        m->encoderCount++;
        m->speed = (ENCODER_SCALE * 10000ULL) * 1.0 / m->DeltaTimeEncoder * 1.0;
        m->DeltaTimeENC = m->DeltaTimeEncoder;
        m->LastTimeEncoder = micros();
    }
}
void Motorbike::attachPhaseB(void *arg)
{
    Motorbike *m = static_cast<Motorbike *>(arg);
    m->encoderCount--;
}
void Motorbike::init()
{
    pinMode(this->hall_pin, INPUT_PULLUP);
    pinMode(this->signel_left_pin, INPUT_PULLDOWN);
    pinMode(this->engine_pin, INPUT_PULLDOWN);

    pinMode(this->encoder_a_pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(this->encoder_a_pin), Motorbike::attachPhaseA, this, RISING);
    if (this->mode_encoder == TWO_SIGNEL_ENCODER_MODE)
    {
        pinMode(this->encoder_b_pin, INPUT_PULLUP);
        attachInterruptArg(digitalPinToInterrupt(this->encoder_b_pin), Motorbike::attachPhaseB, this, RISING);
    }
    this->encoderCount = 0;
    this->distance = 0;
    this->speed = 0;
    this->signelLeft = false;
    this->sensorHall = false;
    this->signelEngine = false;
    this->attachLine = false;
}
void Motorbike::reset()
{
    this->encoderCount = 0;
    this->distance = 0;
    this->speed = 0;
}
void Motorbike::update() // update sensor sate of motor
{

    this->sensorHall = !digitalRead(this->hall_pin);

    this->distance = this->encoderCount * ENCODER_SCALE;

    // if (micros() - this->lastTimeus > TIMING_UPDATE_SPEED * 1000)
    // {
    //     uint32_t deltaTime = micros() - this->lastTimeus;
    //     this->speed = this->distance * 1000.0 / deltaTime;
    //     this->lastTimeus = micros();
    // }
    // process signal Engine
    if (!digitalRead(this->engine_pin))
    {
        this->signelEngine = true;
        this->lastTimeEngine = millis();
    }
    if ((millis() - this->lastTimeEngine > 500) && this->signelEngine)
    {
        this->signelEngine = false;
    }
    // process signal left
    if (!digitalRead(this->signel_left_pin))
    {
        this->signelLeft = true;
        this->lastTimeSignalLeft = millis();
    }
    if ((millis() - this->lastTimeSignalLeft > 1000) && this->signelLeft)
    {
        this->signelLeft = false;
    }
}
void Motorbike::setAttachLine(bool attach) // set attach line of motor
{
    this->attachLine = attach;
}
bool Motorbike::getSignelLeft() // return signel left of motor
{
    return this->signelLeft;
}
bool Motorbike::getSensorHall() // return sensor hall of motor
{
    return this->sensorHall;
}
bool Motorbike::getSignelEngine() // return signel engine of motor
{
    return this->signelEngine;
}
uint32_t Motorbike::getEncoderCount() // return encoder count
{
    return this->encoderCount;
}
uint32_t Motorbike::getDistance() // return distance of motor
{
    return this->distance;
}
uint32_t Motorbike::getSpeed() // return speed of motor
{
    return this->speed;
}
Motorbike motor(HALL_PIN, SIGNEL_LEFT_PIN, ENCODER_A_PIN, ENCODER_B_PIN);
