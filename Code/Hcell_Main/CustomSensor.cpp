/*  ----------- CustomSensor.cpp -----------
    Author: Tim Ruf
    Institution: Hochschule Karlsruhe
    Description: This File contains custom implementation of the Sensor Class
*/
#include "CustomSensor.h"

//Generic analogSensorClass implementation
AnalogSensor::AnalogSensor(int pin, String name, SensorType sensorType)
  : Sensor(name, 1) {
  _pin = pin;
  _sensorName = name;
  pinMode(_pin, INPUT);

  //Create SensorData struct
  _data = new SensorData;
  _data->sensorName = _sensorName;
  _data->type = sensorType;
  _data->value = 0;
}

void AnalogSensor::update() {
  _data->value = analogRead(_pin);
}

double AnalogSensor::getValue() {
  return _data->value;
}

//BmeSensor class implementation
BmeSensor::BmeSensor(int address, String name)
  : Sensor(name, 3) {

  //Create data Structs
  _data = new SensorData[3];
  _sensorName = name;

  //populate datastruct
  _data[0].sensorName = _sensorName;
  _data[0].type = SENS_TEMP;
  _data[0].value = 0;
  _data[2].inverval = 1;
  _data[1].sensorName = _sensorName;
  _data[1].type = SENS_PRESSURE;
  _data[1].value = 0;
  _data[2].inverval = 1;
  _data[2].sensorName = _sensorName;
  _data[2].type = SENS_HUM;
  _data[2].value = 0;
  _data[2].inverval = 1;

  //Start
  //_bme.begin(address); //To-do: Handle connection loss or connection timeout
}

void BmeSensor::update() {
  _data[0].value = _bme.readTemperature();
  _data[1].value = _bme.readPressure();
  _data[2].value = _bme.readHumidity();
}

double BmeSensor::getValue(SensorType sensorType) {
  double tmp;
  switch (sensorType) {
    case SENS_TEMP:
      tmp = _data[0].value;
    case SENS_PRESSURE:
      tmp = _data[1].value;
    case SENS_HUM:
      tmp = _data[2].value;
  }
}


// HCell class implementation
HCell::HCell(HardwareSerial* SerialH, String name)
  : Sensor(name, 4) {
    
}

void HCell::update() {
}
