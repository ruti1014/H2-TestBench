#ifndef USBMSC_INIT_H
#define USBMSC_INIT_H

/*  Description:      contains USB callback functions and SD Card init-function
*   Author:           Jonas Geckle
*/


enum states {
  PLUGGED, UNPLUGGED, SUSPENDED, RESUMED
};

states USB_state;

//---------------------------------------------------------------------------------------------------------------
// USB callback functions:

  static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
    // triggered when PC writes to MSC
    HWSerial.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
    return SD.writeRAW( (uint8_t*) buffer, lba) ? bufsize : -1 ;
  }

  static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
    // HWSerial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
    return SD.readRAW( (uint8_t*) buffer, lba) ? 512 : -1 ;
  }

  static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
    // triggered when PC ejects MSC
    HWSerial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
    return true;
  }

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

//---------------------------------------------------------------------------------------------------------------
// SD Card SPI init:

  void initSDCard(){
    // new SPI Class Object:
    static SPIClass* spi = NULL;
    spi = new SPIClass(FSPI);
    spi->begin(SPI_SCK, SPI_MISO, SPI_MOSI, MicroSD_SPI_CS);

    // init SD Class with SPI Class Object:
    if ( !SD.begin( MicroSD_SPI_CS, *spi, 20000000 ) ){
      HWSerial.println(F("Storage initialization failed"));
      HWSerial.println("Stopped");
      while(1);
    }
    else{
      HWSerial.println(F("Storage initialization success"));
    }

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
//---------------------------------------------------------------------------------------------------------------
// SD Card refresh:

  void refreshSDCard(){       // refreshes SD Card after new file has been written from ESP32
    
  }

#endif