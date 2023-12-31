/*  ----------- USBMSC.h -----------
*   Author: Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: initializes an SD-Card (connected via SPI) as a USB Mass Storage Device that can be accessed from a PC via USB
*                refreshMS() disconnects and reconnects the Mass Storage to the PC (if sd_changed == true)
*/

#ifndef USBMSC_INIT_H
#define USBMSC_INIT_H

enum states {
  PLUGGED, UNPLUGGED, SUSPENDED, RESUMED
};

states USB_state;
USBMSC MS;                      // Mass Storage Object
bool sd_changed = false;        // set to true after writing new content to SD-Card
int timestamp_refreshMS = 0;    // to measure a delay of 500 ms   --    '0' indicates that no refresh process is active

// ######## USB callback functions ################################
  // triggered when PC writes to MSC
  static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
    // HWSerial.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
    return SD.writeRAW( (uint8_t*) buffer, lba) ? bufsize : -1 ;
  }

  // triggered when PC reads from MSC
  static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
    // HWSerial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
    return SD.readRAW( (uint8_t*) buffer, lba) ? 512 : -1 ;
  }

  // triggered when PC ejects MSC
  static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
    HWSerial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
    return true;
  }

  // triggered by a USB-Event
  static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if(event_base == ARDUINO_USB_EVENTS){
      arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
      switch (event_id){
        case ARDUINO_USB_STARTED_EVENT:
          HWSerial.println("USB PLUGGED");
          USB_state = PLUGGED;
          break;
        case ARDUINO_USB_STOPPED_EVENT:
          HWSerial.println("USB UNPLUGGED");
          USB_state = UNPLUGGED;
          break;
        case ARDUINO_USB_SUSPEND_EVENT:
          HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
          USB_state = SUSPENDED;
          break;
        case ARDUINO_USB_RESUME_EVENT:
          HWSerial.println("USB RESUMED");
          USB_state = RESUMED;
          break;
        
        default:
          break;
      }
    }
  }

// ######## SD Card SPI init ################################
void initSDCard(){
  // new SPI Class Object:
  static SPIClass* MicroSD_SPI = NULL;
  MicroSD_SPI = new SPIClass(HSPI);
  MicroSD_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, MicroSD_SPI_CS);

  // init SD Class with SPI Class Object:
  while ( !SD.begin( MicroSD_SPI_CS, *MicroSD_SPI, 4000000 ) ){
    //                                           4000000
    HWSerial.println("retry");
    delay(100);

    // restart manually with reset button
    // HWSerial.println("stopped.");
    // while(1);
  }
  // else{       
  //   HWSerial.println(F("Storage initialization success"));
  // }

  // print SD Card Info:
  HWSerial.print( "card size = " );
  HWSerial.print( SD.cardSize() / (1024 * 1024) );
  HWSerial.print( " MB" );
  HWSerial.print( ", used: " );
  HWSerial.print( SD.usedBytes() / (1024 * 1024) );
  HWSerial.print( " MB" );
  // HWSerial.print(", numSectors = " );
  // HWSerial.print( SD.numSectors() );
  // HWSerial.print( ", bytes per sector = " );
  // HWSerial.print( SD.cardSize() / SD.numSectors() );
  // HWSerial.print( ", total bytes = " );
  // HWSerial.print( SD.totalBytes() );
  HWSerial.print( ", usedBytes = " );
  HWSerial.print( SD.usedBytes() );
  HWSerial.print(", SD Card Type: ");
  switch(SD.cardType()){
    case CARD_MMC:
      HWSerial.println("MMC");
      break;
    case CARD_SD:
      HWSerial.println("SDSC");
      break;
    case CARD_SDHC:
      HWSerial.println("SDHC");
      break;
    case CARD_NONE:
      HWSerial.println("No SD card attached");
      break;
    default:
      HWSerial.println("UNKNOWN");
  }
}

// ######## Mass Storage init ################################
void initMS(){
  MS.vendorID("ESP32S3");
  MS.productID("USB_MSC");
  MS.productRevision("1.0");
  MS.onRead(onRead);
  MS.onWrite(onWrite);
  MS.onStartStop(onStartStop);
  MS.mediaPresent(true);
  MS.begin(SD.numSectors(), SD.cardSize() / SD.numSectors() );
}

// ######## Mass Storage refresh ################################
void refreshMS(){       
  /*  refreshes SD Card after new content has been written from ESP32
  *   process for a newly written file to appear on the connected device (e.g. PC):
  *   - write the file when Mass Storage is connected
  *   - disconnect MS by calling MS.mediaPresent(false)
  *   - keep disconnected for at least 500 ms
  *   - reconnect MS by calling MS.mediaPresent(true)
  */
  // disconnect Mass Storage
  if( sd_changed && (timestamp_refreshMS == 0) ){
    HWSerial.println("resfresh start...");
    MS.mediaPresent(false);
    timestamp_refreshMS = micros();
    sd_changed = false;
  }
  // reconnect Mass Storage after 500 ms
  if( (timestamp_refreshMS != 0) && micros() >= (timestamp_refreshMS + 500000) ){
    HWSerial.println("resfresh stop...");
    MS.mediaPresent(true);
    timestamp_refreshMS = 0;        // to indicate that refresh process is finished
  }
}

#endif