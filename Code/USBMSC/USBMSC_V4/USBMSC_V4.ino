
/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle
*   
*   Settings:
*       USB CDC On Boot: "Enabled"
*       USB Firmware MSC On Boot: "Disabled"
*       Upload Mode: UART0 / Hardware CDC
*       USB Mode: "USB-OTG (Tiny-USB)"
*/

/* todo:
    - SD-card refresh after new file has been written
*/

#include "SD.h"
#include "SPI.h"
#include "USB.h"
#include "USBMSC.h"

//------------------------------------------------------------------------------------------------
  // define SPI Pins:

  #define SPI_MOSI 11
  #define SPI_MISO 13
  #define SPI_SCK 12
  #define MicroSD_SPI_CS 10
  //#define Display_SPI_CS xx       // maybe use same SPI-Bus for Display

//------------------------------------------------------------------------------------------------
  // use USBSerial and HWSerial as in: Examples > Examples for ESP32S3 Dev Module > USB > USBMSC
  #if ARDUINO_USB_CDC_ON_BOOT
  #define HWSerial Serial0
  #define USBSerial Serial
  #else
  #define HWSerial Serial
  USBCDC USBSerial;
  #endif
//------------------------------------------------------------------------------------------------

USBMSC msc;     // mass storage object

#include "file_handling.h"
#include "USBMSC_init.h"

bool sd_inited = false;
bool testwrite = true;

int counter = 0;


void setup() {
  HWSerial.begin(115200);
  HWSerial.setDebugOutput(true);

  USB.onEvent(usbEventCallback);

  long time = millis();
  while (!HWSerial && ( millis() < time + 5000) ); // wait up to 5 seconds for Arduino HWSerial Monitor
  HWSerial.println("");

  initSDCard();
  initMSC();

  USBSerial.begin();
  USB.begin();

  // list initial contents of SD-Card
  HWSerial.println();
  listDir(SD, "/", 0);
  HWSerial.println();
}

void loop() {

  if( USB_state == PLUGGED ){
    // do nothing and wait for disconnect
  }
  else if( USB_state == UNPLUGGED ){
    // write new file(s) to SD-card and reconnect USB
  }



  if( testwrite ){
    writeFile(SD, "/Hello_World.txt", "Hello World!");
    writeFile(SD, "/Hello_World2.txt", "Hello World2!");
    appendFile(SD, "/Hello_World.txt", "\nTest");
    testwrite = false;
  }

  // write a new test-file after 20 sec
  if(counter == 20){
    writeFile(SD, "/Test.txt", "Test\n1234");

    refreshMSC();   // contains a total delay of 1 second !
  }
  
  counter++;
  delay(1000); // refresh every 1 second
}