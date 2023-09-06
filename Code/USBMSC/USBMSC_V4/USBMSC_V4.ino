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

#include "file_handling.h"
#include "USBMSC_init.h"

bool sd_inited = false;
bool testwrite = true;      // to write the testfiles only once
int counter = 0;            // to count 20 sec for the second write-test

void setup() {
  HWSerial.begin(115200);
  HWSerial.setDebugOutput(true);

  USB.onEvent(usbEventCallback);

  // long time = millis();
  // while (!HWSerial && ( millis() < time + 5000) ); // wait up to 5 seconds for Arduino HWSerial Monitor
  // HWSerial.println("");

  initSDCard();
  initMS();

  USBSerial.begin();
  USB.begin();  

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

  // write a new test-file after 20 sec
  if( counter == 20 ){
    writeFile(SD, "/Test.txt", "Test\n1234");
    sd_changed = true;
  }

  refreshMS();        // refreshes if sd_changed == true
  
  counter++;
  delay(1000);
}