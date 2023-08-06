#include "Sensor.h"
//Dependencies
#include <Adafruit_BME280.h>

//Sensor class implementation
Sensor::Sensor(int dataQuantity)
  : _dataQuantity(dataQuantity) {}
int Sensor::getDataQuantity() {
  return _dataQuantity;
}

void Sensor::getSensorData(SensorData* dataPtr, int index) {
  if (_dataQuantity > 0) {
    dataPtr = _data + sizeof(SensorData) * _dataQuantity;  //return pointer to element with index
  } else {
    dataPtr = _data;
  }
}



//BmeSensor class implementation
BmeSensor::BmeSensor(int address, String name) {
  _dataQuantity = 3;  //BME280 is measuring temp/hum/pres -> 3 values

  //Create data Structs
  _data = new SensorData[3];
  //Todo fill in structs


  //Start
  _bme.begin(address);
}
double BmeSensor::getValue() {
  return _bme.readTemperature();
}
double BmeSensor::readValue() {
}
double BmeSensor::updateValue(double val) {}

//Generic analogSensorClass implementation
AnalogSensor::AnalogSensor(int pin) {}
double AnalogSensor::getValue() {}
double AnalogSensor::readValue() {}
double AnalogSensor::updateValue(double val) {}


//SensorArray
SensorArray::SensorArray(int sensorAmount, int dataAmount)
  : _sensorAmount(sensorAmount), _dataAmount(dataAmount) {
  _sensorList = new Sensor*[_sensorAmount];
  _dataList = new SensorData[_dataAmount];
  _sensorIndex = 0;
  _dataIndex = 0;
}

bool SensorArray::addSensor(Sensor* sensor) {
  bool sensorAddSucces = false;
  bool dataAddSucces = false;

  //Add sensor to List;
  if (_sensorIndex < _sensorAmount) {
    _sensorList[_sensorIndex] = sensor;
    _sensorIndex++;
    sensorAddSucces = true;
  }

  //Add sensor value struct to list
  int sensorDataAmount = sensor->getDataQuantity();

  if ((_dataIndex + _sensorAmount < _dataAmount) && sensorAddSucces) {
    for (int i = 0; i < sensorDataAmount; i++) {
      //Todo: populate _dataList with SensorData structs from sensor
    }
    _dataIndex + sensorDataAmount;
  }

  return (sensorAddSucces && dataAddSucces);
}

void SensorArray::updateSensorValues() {
}

double SensorArray::returnSensorValue(){};