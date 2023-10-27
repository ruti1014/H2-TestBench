/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle
*   
*   ESP32-S3 Settings:
*       USB CDC On Boot: "Enabled"
*       USB Firmware MSC On Boot: "Disabled"
*       Upload Mode: UART0 / Hardware CDC
*       USB Mode: "USB-OTG (Tiny-USB)"
*/

// USBMSC + SPI-Display + Multiplexer

#include <SD.h>
#include <SPI.h>
#include <USB.h>
#include <USBMSC.h>
#include <TFT_22_ILI9225.h>
#include <arduino-timer.h>
#include <Preferences.h>

#define DEBUG 1
#ifndef DEBUG
  #define debugprint(x) HWSerial.print(x)
  #define debugprintln(x) HWSerial.println(x)
#else
  #define debugprint(x)
  #define debugprintln(x)
#endif

//------------------------------------------------------------------------------------------------
  // define SPI Pins:
  #define SPI_MOSI 41     // Display_SDI
  #define SPI_MISO 42     // Display_RS
  #define SPI_SCK 40
  #define MicroSD_SPI_CS 39
  #define Display_SPI_CS 21

  #define TFT_RST 48
  #define Display_LED 0   // 0 if wired to +5V directly
  //#define Display_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

  // ESP32-S3 standard SPI Pins:
  /*
  #define SPI_MOSI 11
  #define SPI_MISO 13
  #define SPI_SCK 12
  #define MicroSD_SPI_CS 10
  */

  // define Multiplexer Pins:   (ESP32-S3 Pins)
  #define comPin 3
  #define channelA 14
  #define channelB 35
  #define channelC 47

  //------------------------------------------------------------------------------------------------
  #define numSensors 14     // number of connected sensors  --> could be generated by SensorClass ?
  //------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
  #if ARDUINO_USB_CDC_ON_BOOT
  #define HWSerial Serial0
  #define USBSerial Serial
  #else
  #define HWSerial Serial
  USBCDC USBSerial;
  #endif
//------------------------------------------------------------------------------------------------

uint16_t SensorData[100][numSensors];

#include "file_handling.h"
#include "LogoHEK.h"
#include "USBMSC.h"
#include "Multiplexer.h"
#include "Display_init.h"
#include "Ringbuffer.h"

bool sd_inited = false;
bool testwrite = true;      // to write the testfiles only once
int counter = 0;            // to count 20 sec for the second write-test

bool button_pressed[8] = {false, false, false, false, false, false, false, false};

uint32_t recordingFileIndex = 0;   // index for .csv-files
const char recordingKeyName[] = "recStorageKey";      // max. 15 chars
Preferences preferences;

void resetFileIndex(){
  HWSerial.println("Resetting fileIndex...");
  recordingFileIndex = 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
  HWSerial.print("new fileIndex: ");
  HWSerial.println(preferences.getUInt(recordingKeyName, 0));
}

bool inc_counter(void *){
  counter++;
  return true;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  HWSerial.begin(9600);
  HWSerial.setDebugOutput(true);
  HWSerial.println();

  for(int i=0; i<numSensors; i++){
    for(int j=0; j<100; j++){
      SensorData[j][i] = j;
    }
  }

//----------------------------------------------------------------------------------------------------------------------------------------
  // permanent storage of recordingFileIndex
  preferences.begin("permaStorage", false);       // false --> read/write       true --> read-only

  // resetFileIndex();

  recordingFileIndex = preferences.getUInt(recordingKeyName, 0);    // get previous FileIndex (returns 0 if key doesn't exist)

  if( recordingFileIndex == 0 ){
    recordingFileIndex = 1;
    preferences.putUInt(recordingKeyName, recordingFileIndex);
  }
  else if( recordingFileIndex == 4294967295 ){
    resetFileIndex();
  }

  // preferences.end();
//----------------------------------------------------------------------------------------------------------------------------------------
  USB.onEvent(usbEventCallback);
  
  // initDisplay();
  initSDCard();
  initMS();

  USBSerial.begin();
  USB.begin(); 

  initMultiplexer(); 

  // list initial contents of SD-Card
  HWSerial.println();
  listDir(SD, "/", 0);
  HWSerial.println();

  timer.every(1000, inc_counter);

  delay(5000);
}

void loop() {
  timer.tick();

  if( testwrite ){
    writeFile(SD, "/Hello_World.txt", "Hello World!");
    
    sd_changed = true;
    testwrite = false;
  }

//---- Button 0 -----------------------------------------------------
  if( (CHvalue[0] == 1) && (button_pressed[0] == false) ){
    button_pressed[0] = true;
    writeCSV(SD, &recordingFileIndex);
    preferences.putUInt(recordingKeyName, recordingFileIndex);
    sd_changed = true;
  }
  if( (CHvalue[0] == 0) && (button_pressed[0] == true) ){
    button_pressed[0] = false;
  }

//---- Button 1 -----------------------------------------------------
  if( (CHvalue[1] == 1) && (button_pressed[1] == false) ){
    button_pressed[1] = true;

  }
  if( (CHvalue[1] == 0) && (button_pressed[1] == true) ){
    button_pressed[1] = false;
  }

//---- Button 2 -----------------------------------------------------
  if( (CHvalue[2] == 1) && (button_pressed[2] == false) ){
    button_pressed[2] = true;
    // appendCSV(SD, &recordingFileIndex, SensorData, 100);
    // sd_changed = true;
  }
  if( (CHvalue[2] == 0) && (button_pressed[2] == true) ){
    button_pressed[2] = false;
  }

  multiplexerLoop();    // reads buttons every 100ms
  refreshMS();          // refreshes if sd_changed == true
}