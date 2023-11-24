/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle, Tim Ruf
*   
*   ESP32-S3 Settings:
*       USB CDC On Boot: "Enabled"
*       USB Firmware MSC On Boot: "Disabled"
*       Upload Mode: UART0 / Hardware CDC
*       USB Mode: "USB-OTG (Tiny-USB)"
*/



//------------------------------------------------------------------------------------------------
#define DEBUG 1
#ifndef DEBUG
#define debugprint(x) HWSerial.print(x)
#define debugprintln(x) HWSerial.println(x)
#else
#define debugprint(x)
#define debugprintln(x)
#endif

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// pin define 
#define PIN_startHcell 4   
#define PIN_cutoff 5       
#define PIN_leaksensor 15  // leaksensor analog in
#define PIN_flowsensor 16  // flowsensor analog in
#define PIN_RX 18
#define PIN_TX 17
// SPI Pins:
#define SPI_MOSI 41  // Display_SDI
#define SPI_MISO 42  // Display_RS
#define SPI_SCK 40
#define MicroSD_SPI_CS 39
#define Display_SPI_CS 21

#define TFT_RS 37
#define TFT_RST 48
#define Display_LED 0  // 0 if wired to +5V directly

// Multiplexer Pins:   (ESP32-S3 Pins)
#define comPin 3
#define channelA 14
#define channelB 35
#define channelC 47

// I2C Pins:
#define I2C_SDA 6
#define I2C_SCL 7

//------------------------------------------------------------------------------------------------
#define numSensors 4  // number of connected sensors
#define numData 8     // number of data streams e.g. 1 bme280 = 3 data streams (temp, hum, pres)
#define sensorBufferSize 100
#define sampleRateMS 1000          //sensor sample rate in ms
#define storageInitFailed 5        //how many time the esp trys to connect to sd card
#define leakSensorUpperLimit 2804  //max value of H2 concentration -> 10 000 ppm (lower explosive limit)
#define leakSensorLowerLimit 2100  //safe value to start h-cell at least 25% lower than lower explosive limit
//------------------------------------------------------------------------------------------------

#include <SD.h>
#include <SPI.h>
#include <USB.h>
#include <USBMSC.h>
#include <TFT_22_ILI9225.h>
#include <Preferences.h>
#include <Wire.h>

#include "Sensor.h"
#include "CustomSensor.h"

#include "Globals.h"
#include "GuiSetup.h"
#include "file_handling.h"
#include "LogoHEK.h"
#include "USBMSC.h"
#include "Multiplexer.h"

#include "Utilities.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  HWSerial.begin(9600);
  HWSerial.setDebugOutput(true);
  HWSerial.println("Starting");

  SerialHCELL.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);

  USB.onEvent(usbEventCallback);
  setupSPI();
  USBSerial.begin();
  USB.begin();
  setup_pins();
  setupI2C();
  setupPreferences();
  setupSensors();
  setupDisplay();
  initSDCard();
  if (sd_inited) initMS();
  setupGui();


  // HWSerial.println(bme1.isAvailable() ? "bme1 rdy" : "bme1 not rdy");
  // HWSerial.println(bme2.isAvailable() ? "bme2 rdy" : "bme2 not rdy");

  HWSerial.println("READY");
}


//To-DO updateSensorValues time consumption
void loop() {
  loopTime();
  checkHlimits();
  checkSD();
  updateSensorArray();
  recording();
  multiplexerLoop();
  updateDisplay();
  addDataToBuffer();
  requestSerialInfo();
}


