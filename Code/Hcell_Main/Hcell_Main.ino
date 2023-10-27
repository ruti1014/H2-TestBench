/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle
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
// pin define     TO-DO PIN correction
#define PIN_startHcell 4   // Gate 1 MOSFET to start-pin H-Cell (12 V)
#define PIN_cutoff 5       // Gate 2 MOSFET to cut-off and flowsensor (24 V)
#define PIN_recording 23   // recording-switch
#define PIN_leaksensor 98  // leaksensor analog in
#define PIN_flowsensor 15  // flowsensor analog in
#define PIN_RX 16
#define PIN_TX 17
#define PIN_start 18  // start-stop-switch
// SPI Pins:
#define SPI_MOSI 41  // Display_SDI
#define SPI_MISO 42  // Display_RS
#define SPI_SCK 40
#define MicroSD_SPI_CS 39
#define Display_SPI_CS 21

#define TFT_RST 48
#define Display_LED 0  // 0 if wired to +5V directly

// Multiplexer Pins:   (ESP32-S3 Pins)
#define comPin 3
#define channelA 14
#define channelB 35
#define channelC 47

//------------------------------------------------------------------------------------------------
#define numSensors 5  // number of connected sensors
#define numData 14    // number of data streams e.g. 1 bme280 = 3 data streams (temp, hum, pres)
#define sensorBufferSize 100
//------------------------------------------------------------------------------------------------

#include <SD.h>
#include <SPI.h>
#include <USB.h>
#include <USBMSC.h>
#include <TFT_22_ILI9225.h>
#include <Preferences.h>

// TO-DO: move below Globals.h
#include "Sensor.h"
#include "CustomSensor.h"

#include "Globals.h"
#include "file_handling.h"
#include "LogoHEK.h"
#include "USBMSC.h"
#include "Multiplexer.h"
#include "Display_init.h"

#include "Utilities.h"



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  HWSerial.begin(9600);
  HWSerial.setDebugOutput(true);
  setup_pins();
  setupPreferences();

  initDisplay();
  initSDCard();
  initMS();

  USB.onEvent(usbEventCallback);
  USBSerial.begin();
  USB.begin();
}

void loop() {
  recording();
  multiplexerLoop();  // reads buttons every 100ms
}