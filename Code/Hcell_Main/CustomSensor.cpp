/*  ----------- CustomSensor.cpp -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: This File contains custom implementation of the Sensor Class
*/

#include "CustomSensor.h"

//Generic analogSensorClass implementation
AnalogSensor::AnalogSensor(int pin, String name, SensorType sensorType, int interval, String unit)
  : Sensor(name, 1) {
  _pin = pin;
  _sensorName = name;
  pinMode(_pin, INPUT);

  //Create SensorData struct
  _data = new SensorData;
  _data->sensorName = _sensorName;
  _data->unit = unit;
  _data->type = sensorType;
  _data->value = 0;
  _data->interval = interval;
}

void AnalogSensor::update() {
  _data->value = analogRead(_pin);
}

uint16_t AnalogSensor::getValue() {
  return _data->value;
}

//BmeSensor class implementation
BmeSensor::BmeSensor(int address, String name, TwoWire* theWire)
  : Sensor(name, 3) {

  //Create data Structs
  _data = new SensorData[6];
  _sensorName = name;
  _address = address;
  _theWire = theWire;

  //populate datastruct
  _data[0].sensorName = _sensorName;
  _data[0].unit = "°C";
  _data[0].type = SENS_TEMP;
  _data[0].value = 0;
  _data[0].interval = 1;

  _data[1].sensorName = _sensorName;
  _data[1].unit = "hPa";
  _data[1].type = SENS_PRESSURE;
  _data[1].value = 0;
  _data[1].interval = 1;

  _data[2].sensorName = _sensorName;
  _data[2].unit = "%";
  _data[2].type = SENS_HUM;
  _data[2].value = 0;
  _data[2].interval = 1;
}

bool BmeSensor::sensorInit() {

  _bme_available = _bme.begin(_address, _theWire);
  // HWSerial.println("Bme ptr address: ");
  // HWSerial.println((long int)_theWire,HEX);
  return _bme_available;
}

void BmeSensor::update() {
  if (_bme_available) {
    _data[0].value = (uint16_t)_bme.readTemperature();
    _data[1].value = (uint16_t)(_bme.readPressure() / 100.0);
    _data[2].value = (uint16_t)_bme.readHumidity();
  }
}

uint16_t BmeSensor::getValue(SensorType sensorType) {
  uint16_t tmp = 0;
  switch (sensorType) {
    case SENS_TEMP:
      tmp = _data[0].value;
      break;
    case SENS_PRESSURE:
      tmp = _data[1].value;
      break;
    case SENS_HUM:
      tmp = _data[2].value;
      break;
  }
  return tmp;
}

bool BmeSensor::isAvailable() {
  return _bme_available;
}


//Hcell_RS232 class implementation
Hcell_RS232::Hcell_RS232(HardwareSerial* serial, String name)
  : Sensor(name, 6), _hcellSerial(serial) {
  _data = new SensorData[6];
  _sensorName = name;
  //populate datastruct
  _data[0].sensorName = _sensorName;
  _data[0].unit = "";
  _data[0].type = SENS_SERIALID;
  _data[0].value = 0;
  _data[0].interval = -1;

  _data[1].sensorName = _sensorName;
  _data[1].unit = "°C";
  _data[1].type = SENS_TEMP;
  _data[1].value = 0;
  _data[1].interval = 1;

  _data[2].sensorName = _sensorName;
  _data[2].unit = "hPa";
  _data[2].type = SENS_PRESSURE;
  _data[2].value = 0;
  _data[2].interval = 1;

  _data[3].sensorName = _sensorName;
  _data[3].unit = "mA";
  _data[3].type = SENS_CURRENT;
  _data[3].value = 0;
  _data[3].interval = 1;

  _data[4].sensorName = _sensorName;
  _data[4].unit = "mV";
  _data[4].type = SENS_VOLTAGE;
  _data[4].value = 0;
  _data[4].interval = 1;

  _data[5].sensorName = _sensorName;
  _data[5].unit = "";
  _data[5].type = SENS_ERRCODE;
  _data[5].value = 0;
  _data[5].interval = 1;
}

void Hcell_RS232::update() {

  static int valueFlag = 0;
  static bool requestSerial = true;

  if (requestSerial) {  //request data from H-Cell and wait for answer
    switch (valueFlag) {
      case 0:
        _hcellSerial->print("*idn?\r\n");  //Maybe uint16 to small?!
        break;
      case 1:
        _hcellSerial->print("*temp?\r\n");
        break;
      case 2:
        _hcellSerial->print("*pres?\r\n");
        break;
      case 3:
        _hcellSerial->print("*cur?\r\n");
        break;
      case 4:
        _hcellSerial->print("*vol?\r\n");
        break;
      case 5:
        _hcellSerial->print("*error?\r\n");
        break;
    }
    requestSerial = false;
  }

  String answer;
  if (_hcellSerial->available()) {
    answer = _hcellSerial->readString();
    _data[valueFlag].value = (uint16_t)answer.toInt();
    if (valueFlag >= 5) valueFlag = 0;  //reset to 1 to skip serialnumber
    else valueFlag++;
    requestSerial = true;
  }
}

uint16_t Hcell_RS232::getValue(SensorType type) {
  uint16_t tmp = 0;
  switch (type) {
    case SENS_SERIALID:
      tmp = _data[0].value;
      break;
    case SENS_TEMP:
      tmp = _data[1].value;
      break;
    case SENS_PRESSURE:
      tmp = _data[2].value;
      break;
    case SENS_CURRENT:
      tmp = _data[3].value;
      break;
    case SENS_VOLTAGE:
      tmp = _data[4].value;
      break;
    case SENS_ERRCODE:
      tmp = _data[5].value;
      break;
  }

  return tmp;
}
