

#include <Adafruit_BME280.h>
#include "Sensor.h"
#include "CustomSensor.h"


#define numSens 4
#define numData 6

#define I2C_SDA 6
#define I2C_SCL 7

HardwareSerial SerialH(2);

TwoWire I2C = TwoWire(0);


//create sensor objects
AnalogSensor h2flow(19, "h2flow", SENS_H2FLOW, 1, "ml/s");
AnalogSensor h2leak(19, "h2leak", SENS_H2LEAK, 1, "ppm");
AnalogSensor rndhum(19, "rndhum", SENS_HUM, 1, "rnd");
BmeSensor bme1(0x76, "bme1", &I2C);

//create sensor array
SensorArray sensorArray(numSens, numData);  //Amount of sensors, amount of captuered data

void setup() {
  SerialH.begin(115200, SERIAL_8N1, 16, 17);

  Serial0.begin(115200);
  Serial0.println("Start");

  I2C.begin(I2C_SDA, I2C_SCL, 100000);

  Serial0.println(sizeof(uint8_t));
  Serial0.println(sizeof(SensorData));

  sensorArray.addSensor(&h2flow);
  sensorArray.addSensor(&h2leak);
  sensorArray.addSensor(&rndhum);


  sensorArray.addSensor(&bme1);
  Serial0.println("Active sensors: ");

  String tmp = sensorArray.getSensorList();
  Serial0.println(tmp);



  for (int i = 0; i < numData; i++) {
    Serial0.println(sensorArray.getData(i)->sensorName);
  }
  sensorArray.updateSensorValues();

  Serial0.println("Setup end");
}

void loop() {}
