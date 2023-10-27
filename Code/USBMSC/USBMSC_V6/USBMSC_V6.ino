/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle
*   
*   Settings:
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
  // number of buttons
  #define numButtons 5     // max. 8 buttons
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

#include "file_handling.h"
#include "LogoHEK.h"
#include "USBMSC_init.h"
#include "Multiplexer_init.h"
#include "Display_init.h"

bool sd_inited = false;
bool testwrite = true;      // to write the testfiles only once
int counter = 0;            // to count 20 sec for the second write-test

bool inc_counter(void *){
  counter++;
  return true;
}

void setup() {
  HWSerial.begin(9600);
  HWSerial.setDebugOutput(true);

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

  // write a new test-file after 20 sec
  if( counter == 20 ){
    writeFile(SD, "/Test.txt", "Test\n1234");
    sd_changed = true;
    counter = 21;
  }

  refreshMS();        // refreshes if sd_changed == true
}