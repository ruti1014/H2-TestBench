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
  #define numSensors 5     // number of connected sensors
  #define numData 14       // number of data streams e.g. 1 bme280 = 3 data streams (temp, hum, pres)
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

#include "Globals.h"
#include "file_handling.h"
#include "Utilities.h"
#include "LogoHEK.h"
#include "USBMSC.h"
#include "Multiplexer.h"
#include "Display_init.h"



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
  
  initDisplay();
  initSDCard();
  initMS();

  USBSerial.begin();
  USB.begin(); 

  initMultiplexer(); 

  // list initial contents of SD-Card
  HWSerial.println();
  listDir(SD, "/", 0);
  HWSerial.println();
}

void loop() {

  if( testwrite ){
    writeFile(SD, "/Hello_World.txt", "Hello World!");
    
    sd_changed = true;
    testwrite = false;
  }

  multiplexerLoop();    // reads buttons every 100ms
  refreshMS();          // refreshes if sd_changed == true
}