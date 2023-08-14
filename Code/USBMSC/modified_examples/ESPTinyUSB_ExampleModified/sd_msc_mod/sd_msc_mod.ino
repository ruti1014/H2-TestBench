/**
 * Simple MSC device with SD card
 * author: chegewara
 */
#include "sdusb.h"
//#if CFG_TUD_MSC

#define SD_MISO  13
#define SD_MOSI  11
#define SD_SCK   12
#define SD_CS    10

SDCard2USB dev;

void setup()
{
  Serial.begin(9600);
  Serial.println("Start setup");

  // if(dev.initSD(SD_SCK, SD_MISO, SD_MOSI, SD_CS))
  // {
  //   if(dev.begin()) {
  //     Serial.println("MSC lun 1 begin");
  //   } 
  //   else 
  //   {
  //     Serial.println("MSC lun 1 begin failed !!");
  //     log_e("LUN 1 failed");
  //   }
  // } else Serial.println("Failed to init SD");


  // test();

  // USB.begin();
}

void loop()
{
  delay(1000);
}

//#endif
