
/*  Description:     This file connects to an SD Card over SPI and uses this SD Card to create a Mass Storage Device which can be accessed via USB from a Computer.
*   Author:          Jonas Geckle
*/

/* todo:
    - change device name shown in windows explorer
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
  // -- might not be necessary --
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

USBMSC msc;

bool sd_changed = false;
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

  // pinMode(MicroSD_SPI_CS, OUTPUT);
  // digitalWrite(MicroSD_SPI_CS, LOW);

  initSDCard();

  msc.vendorID("ESP32S3");
  msc.productID("USB_MSC");
  msc.productRevision("1.0");
  msc.onRead(onRead);
  msc.onWrite(onWrite);
  msc.onStartStop(onStartStop);
  msc.mediaPresent(true);
  msc.begin(SD.numSectors(), 512);     //msc.begin(SD.numSectors(), SD.cardSize() / SD.numSectors() );

  USBSerial.begin();
  USB.begin();


  HWSerial.println();
  listDir(SD, "/", 0);
  HWSerial.println();
  sd_changed = false;
}

void loop() {

  if( USB_state == PLUGGED ){
    // do nothing and wait for disconnect
    // options to disconnect:
    // - physical disconnect of cable
    // - disconnect interrupt because of new file to write
    // - disconnect interrupt because mesuarement has started
    // ???
  }
  else if( USB_state == UNPLUGGED ){
    // write new file(s) to SD-card and reconnect USB
  }





  if ( sd_changed ){
    HWSerial.println();
    listDir(SD, "/", 0);
    HWSerial.println();

    sd_changed = false;
  }

  if( testwrite ){
    writeFile(SD, "/Hello_World.txt", "Hello World!");
    writeFile(SD, "/Hello_World2.txt", "Hello World2!");
    appendFile(SD, "/Hello_World.txt", "\nTest");
    sd_changed = true;
    testwrite = false;
  }

  // write a new test-file after 20 sec
  if(counter == 20){
    msc.mediaPresent(false);
    writeFile(SD, "/Test.txt", "Test\n1234");
    sd_changed = true;
    msc.mediaPresent(true);
  }
  
  counter++;
  delay(1000); // refresh every 1 second
}