#include "BMP180_ESP32.h"

// Constructor
BMP180_ESP32::BMP180_ESP32(TwoWire &i2c) : _i2c(&i2c) {}

bool BMP180_ESP32::begin() {
    _i2c->begin(); // Initialize I2C bus

    // Check Chip ID
    _i2c->beginTransmission(BMP180_I2C_ADDRESS);
    _i2c->write(BMP180_REG_CHIP_ID);
    _i2c->endTransmission();
    _i2c->requestFrom(BMP180_I2C_ADDRESS, 1);
    if (_i2c->available()) {
        uint8_t chipID = _i2c->read();
        if (chipID != 0x55) {
            return false; // Sensor not detected
        }
    }

    // Read calibration data
    AC1 = readInt(0xAA);
    AC2 = readInt(0xAC);
    AC3 = readInt(0xAE);
    AC4 = readInt(0xB0);
    AC5 = readInt(0xB2);
    AC6 = readInt(0xB4);
    B1 = readInt(0xB6);
    B2 = readInt(0xB8);
    MB = readInt(0xBA);
    MC = readInt(0xBC);
    MD = readInt(0xBE);

    return true;
}

float BMP180_ESP32::readTemperature() {
    int32_t UT = readRawTemperature();

    int32_t X1 = (UT - AC6) * AC5 >> 15;
    int32_t X2 = (MC << 11) / (X1 + MD);
    B5 = X1 + X2;

    return (B5 + 8) >> 4 / 10.0; // Temperature in °C
}

float BMP180_ESP32::readPressure() {
    int32_t UP = readRawPressure();

    int32_t B6 = B5 - 4000;
    int32_t X1 = (B2 * (B6 * B6 >> 12)) >> 11;
    int32_t X2 = AC2 * B6 >> 11;
    int32_t X3 = X1 + X2;
    int32_t B3 = (((AC1 * 4 + X3) << 0) + 2) >> 2; // OSS = 0
    X1 = AC3 * B6 >> 13;
    X2 = (B1 * (B6 * B6 >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    uint32_t B4 = AC4 * (uint32_t)(X3 + 32768) >> 15;
    uint32_t B7 = ((uint32_t)UP - B3) * 50000;
    int32_t P = (B7 < 0x80000000) ? (B7 * 2 / B4) : (B7 / B4 * 2);
    X1 = (P >> 8) * (P >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * P) >> 16;
    P += (X1 + X2 + 3791) >> 4;

    return P / 100.0; // Pressure in hPa
}

float BMP180_ESP32::readAltitude(float seaLevelPressure) {
    float pressure = readPressure();
    return 44330 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

int16_t BMP180_ESP32::readInt(uint8_t reg) {
    _i2c->beginTransmission(BMP180_I2C_ADDRESS);
    _i2c->write(reg);
    _i2c->endTransmission();
    _i2c->requestFrom(BMP180_I2C_ADDRESS, 2);
    return (_i2c->read() << 8) | _i2c->read();
}

void BMP180_ESP32::writeByte(uint8_t reg, uint8_t value) {
    _i2c->beginTransmission(BMP180_I2C_ADDRESS);
    _i2c->write(reg);
    _i2c->write(value);
    _i2c->endTransmission();
}

uint16_t BMP180_ESP32::readRawTemperature() {
    writeByte(BMP180_REG_CONTROL, BMP180_CMD_TEMP);
    delay(5); // Wait for measurement
    return readInt(BMP180_REG_RESULT);
}

uint32_t BMP180_ESP32::readRawPressure() {
    writeByte(BMP180_REG_CONTROL, BMP180_CMD_PRESS);
    delay(8); // Wait for measurement
    return ((readInt(BMP180_REG_RESULT) << 8) | _i2c->read()) >> (8 - 0); // OSS = 0
}
