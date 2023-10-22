#include "Sensor.h"


//Sensor class implementation
Sensor::Sensor(String name, int dataQuantity)
  : _sensorName(name), _dataQuantity(dataQuantity) {}

int Sensor::getDataQuantity() {
  return _dataQuantity;
}

String Sensor::getSensorName() {
  return _sensorName;
}

SensorData* Sensor::getSensorData(int index) {
  SensorData* dataPtr = NULL;
  if (_dataQuantity > 0) {
    dataPtr = _data + index;  //return pointer to element with index
  } else {
    dataPtr = _data;
  }

  return dataPtr;
}



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
  if ((_dataIndex < _dataAmount) && sensorAddSucces) {
    for (int i = 0; i < sensorDataAmount; i++) {
      //Todo: populate _dataList with SensorData structs from sensor
      SensorData* data = NULL;
      data = sensor->getSensorData(i);
      _dataList[_dataIndex + i] = *data;
    }
    _dataIndex += sensorDataAmount;
  }

  return (sensorAddSucces && dataAddSucces);
}

void SensorArray::updateSensorValues() {
  for (int i = 0; i < _sensorIndex; i++) {
    _sensorList[i]->update();
  }
}

String SensorArray::getSensorList() {
  String sensorString;
  for (int i = 0; i < _sensorIndex; i++) {
    if (i < _sensorIndex - 1) sensorString += _sensorList[i]->getSensorName() + ", ";
    else sensorString += _sensorList[i]->getSensorName();
  }
  return sensorString;
}
