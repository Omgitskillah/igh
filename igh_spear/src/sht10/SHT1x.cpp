/**
 * SHT1x Library
 *
 * Copyright 2017 Vincent Pang <wingshun.pang@gmail.com>
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 */
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "SHT1x.h"

SHT1x::SHT1x(uint8_t dataPin, uint8_t clockPin, Voltage voltage) :
  _dataPin(dataPin)
  , _clockPin(clockPin)
  , _voltage(voltage)
  , _tempResolution(TemperatureMeasurementResolution::Temperature_14bit)
  , _humidityResolution(HumidityMeasurementResolution::Humidity_12bit)
{
}


/* ================  Public methods ================ */

/**
 * Reads the current temperature in degrees Celsius
 */
float SHT1x::readTemperatureC() const
{
  // Conversion coefficients from datasheet
  const double D1 = getD1ForC(_voltage);
  const double D2 = getD2ForC(_tempResolution);

  // Fetch raw value
  uint16_t rawData = readRawData(ShtCommand::MeasureTemperature, _dataPin, _clockPin);

  // Convert raw value to degrees Celsius
  float temperature = D1 + D2 * rawData;

  return temperature;
}

/**
 * Reads the current temperature in degrees Fahrenheit
 */
float SHT1x::readTemperatureF() const
{
  // Conversion coefficients from datasheet
  const double D1 = getD1ForF(_voltage);
  const double D2 = getD2ForF(_tempResolution);

  // Fetch raw value
  uint16_t rawData = readRawData(ShtCommand::MeasureTemperature, _dataPin, _clockPin);

  // Convert raw value to degrees Fahrenheit
  float temperature = D1 + D2 * rawData;

  return temperature;
}

/**
 * Reads current temperature-corrected relative humidity
 */
float SHT1x::readHumidity() const
{
  // Conversion coefficients from datasheet
  const double C1 = getC1(_humidityResolution);
  const double C2 = getC2(_humidityResolution);
  const double C3 = getC3(_humidityResolution);
  const double T1 = getT1(_humidityResolution);
  const double T2 = getT2(_humidityResolution);

  // Fetch the value from the sensor
  uint16_t rawData = readRawData(ShtCommand::MeasureRelativeHumidity, _dataPin, _clockPin);

  // Apply linear conversion to raw value
  double linearHumidity = C1 + C2 * rawData + C3 * rawData * rawData;

  // Get current temperature for humidity correction
  float temperature = readTemperatureC();

  // Correct humidity value for current temperature
  float correctedHumidity = (temperature - 25.0) * (T1 + T2 * rawData) + linearHumidity;

  return correctedHumidity;
}


/* ================  Private methods ================ */

/**
 * Reads the raw data from sensor
 */
uint16_t SHT1x::readRawData(ShtCommand command, uint8_t dataPin, uint8_t clockPin) const
{
  sendCommandSHT(command, dataPin, clockPin);
  waitForResultSHT(dataPin);
  uint16_t result = getData16SHT(dataPin, clockPin);
  skipCrcSHT(dataPin, clockPin);

  return result;
}

/**
 */
bool SHT1x::sendCommandSHT(ShtCommand command, uint8_t dataPin, uint8_t clockPin) const
{
  // Transmission Start
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, LOW);

  // The command (3 msb are address and must be 000, and last 5 bits are command)
  shiftOut(dataPin, clockPin, MSBFIRST, static_cast<uint8_t>(command));

  // Verify we get the correct ack
  digitalWrite(clockPin, HIGH);
  pinMode(dataPin, INPUT);
  int ack = digitalRead(dataPin);
  if (ack != LOW)
  {
    // Serial.println("Ack Error 0");
    return false;
  }
  digitalWrite(clockPin, LOW);
  ack = digitalRead(dataPin);
  if (ack != HIGH)
  {
    // Serial.println("Ack Error 1");
    return false;
  }

  return true;
}

/**
 */
bool SHT1x::waitForResultSHT(uint8_t dataPin) const
{
  pinMode(dataPin, INPUT);

  // wait for at most 1000ms for the result
  // from the datasheet,
  // "This takes a maximum of 20/80/320 ms for a 8/12/14bit measurement.
  // The time varies with the speed of the internal oscillator and can be lower by up to 30%."
  int ack;
  for(uint8_t i= 0; i < 100; ++i)
  {
    delay(10);
    ack = digitalRead(dataPin);

    if (ack == LOW)
    {
      break;
    }
  }

  if (ack == HIGH)
  {
    // Serial.println("Ack Error 2"); // Can't do serial stuff here, need another way of reporting errors
    return false;
  }

  return true;
}

uint16_t SHT1x::getData16SHT(uint8_t dataPin, uint8_t clockPin) const
{
  uint8_t rawData[2];
  memset(rawData, 0, 2);

  // Get the most significant bits
  pinMode(dataPin, INPUT);
  rawData[0] = shiftIn(dataPin, clockPin, MSBFIRST);

  // Send the required ack
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  // Get the least significant bits
  pinMode(dataPin, INPUT);
  rawData[1] = shiftIn(dataPin, clockPin, MSBFIRST);

  // extract value from the big-endian raw data
  uint16_t result = (rawData[0] << 8) | (rawData[1] << 0);

  return result;
}

/**
 */
void SHT1x::skipCrcSHT(uint8_t dataPin, uint8_t clockPin) const
{
  // Skip acknowledge to end trans (no CRC)
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

double SHT1x::getC1(SHT1x::HumidityMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            -2.0468     // 12 bit
                            , -2.0468   // 8 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getC2(SHT1x::HumidityMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            0.0367    // 12 bit
                            , 0.5872  // 8 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getC3(SHT1x::HumidityMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            -1.5955e-6    // 12 bit
                            , -4.0845e-4  // 8 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getT1(SHT1x::HumidityMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            0.01    // 12 bit
                            , 0.01  // 8 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getT2(SHT1x::HumidityMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            0.00008   // 12 bit
                            , 0.00128 // 8 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getD1ForC(SHT1x::Voltage voltage) const
{
  const double VALUES[] = {
                            -40.1       // 5.0v
                            , -39.8     // 4.0v
                            , -39.7     // 3.5v
                            , -39.66    // 3.3v, linear interpolation
                            , -39.6     // 3.0v
                            , -39.4     // 2.5v
                          };

  return VALUES[static_cast<uint8_t>(voltage)];
}

double SHT1x::getD1ForF(SHT1x::Voltage voltage) const
{
  const double VALUES[] = {
                            -40.2       // 5.0v
                            , -39.6     // 4.0v
                            , -39.5     // 3.5v
                            , -39.42    // 3.3v, linear interpolation
                            , -39.3     // 3.0v
                            , -38.9     // 2.5v
                          };

  return VALUES[static_cast<uint8_t>(voltage)];
}

double SHT1x::getD2ForC(SHT1x::TemperatureMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            0.01    // 14 bit
                            , 0.04  // 12 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getD2ForF(SHT1x::TemperatureMeasurementResolution resolution) const
{
  const double VALUES[] = {
                            0.018   // 14 bit
                            , 0.072 // 12 bit
                          };

  return VALUES[static_cast<uint8_t>(resolution)];
}
