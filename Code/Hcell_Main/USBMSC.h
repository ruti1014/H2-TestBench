/*  ----------- USBMSC.h -----------
*   Author: Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: initializes an SD-Card (connected via SPI) as a USB Mass Storage Device that can be accessed from a PC via USB
*                refreshMS() disconnects and reconnects the Mass Storage to the PC (if sd_changed == true)
*/

#ifndef USBMSC_INIT_H
#define USBMSC_INIT_H

enum states {
  PLUGGED,
  UNPLUGGED,
  SUSPENDED,
  RESUMED
};

states USB_state;
USBMSC MS;                    // Mass Storage Object
bool sd_changed = false;      // set to true after writing new content to SD-Card
int timestamp_refreshMS = 0;  // to measure a delay of 500 ms   --    '0' indicates that no refresh process is active

// ######## USB callback functions ################################
// triggered when PC writes to MSC
static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  //Serial0.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);

  return SD.writeRAW((uint8_t*)buffer, lba) ? bufsize : -1;
}

// triggered when PC reads from MSC
static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  // Serial0.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  sd_busy = true;
  sd_timeout = millis();
  return SD.readRAW((uint8_t*)buffer, lba) ? 512 : -1;
}

// triggered when PC ejects MSC
static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  Serial0.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
  return true;
}

// triggered by USB-Event
static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t* data = (arduino_usb_event_data_t*)event_data;
    switch (event_id) {
      case ARDUINO_USB_STARTED_EVENT:
        Serial0.println("USB PLUGGED");
        USB_state = PLUGGED;
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        Serial0.println("USB UNPLUGGED");
        USB_state = UNPLUGGED;
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        Serial0.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        USB_state = SUSPENDED;
        break;
      case ARDUINO_USB_RESUME_EVENT:
        Serial0.println("USB RESUMED");
        USB_state = RESUMED;
        break;

      default:
        break;
    }
  }
}

// ######## SD Card SPI init ################################
void initSDCard() {

  int retryNum = 5;
  static int retryCount = 0;

  Serial0.print("Attempting to connect SD-Card ");
  // init SD Class with SPI Class Object:
  while ((!sd_inited) && (retryCount < retryNum)) {
    sd_inited = SD.begin(MicroSD_SPI_CS, *MicroSD_SPI, 20000000);
    Serial0.print(". ");
    retryCount++;
    delay(50);
  }


  //while(1); //

  // Serial0.print(restartCounter);
  // Serial0.print("), ");
  // // restart automatically (up to 5 times)
  // if (restartCounter > storageInitFailed) {
  //   Serial0.println("too many automatic restarts, connect storage and restart manually !");

  //   restartCounter = 0;
  //   preferences.putInt(restartKeyName, restartCounter);
  //   return;

  // } else {
  //   Serial0.println("restarting...");
  //   restartCounter++;  // stored in preferences (permanant storage)
  //   preferences.putInt(restartKeyName, restartCounter);
  //   ESP.restart();
  // }
  if (sd_inited) {
    Serial0.println("SD-Card successfully connected.");
    // print SD Card Info:
    Serial0.print("card size = ");
    Serial0.print(SD.cardSize() / (1024 * 1024));
    Serial0.print(" MB");
    Serial0.print(", used: ");
    Serial0.print(SD.usedBytes() / (1024 * 1024));
    Serial0.print(" MB");
    Serial0.print(", numSectors = ");
    Serial0.print(SD.numSectors());
    Serial0.print(", bytes per sector = ");
    Serial0.print(SD.cardSize() / SD.numSectors());
    Serial0.print(", total bytes = ");
    Serial0.print(SD.totalBytes());
    Serial0.print(", usedBytes = ");
    Serial0.print(SD.usedBytes());
    Serial0.print(", SD Card Type: ");
    switch (SD.cardType()) {
      case CARD_MMC:
        Serial0.println("MMC");
        break;
      case CARD_SD:
        Serial0.println("SDSC");
        break;
      case CARD_SDHC:
        Serial0.println("SDHC");
        break;
      case CARD_NONE:
        Serial0.println("No SD card attached");
        break;
      default:
        Serial0.println("UNKNOWN");
    }
  } else {
    Serial0.println("SD-Card connection failed!");
  }
}

// ######## Mass Storage init ################################
void initMS() {
  MS.vendorID("ESP32S3");
  MS.productID("USB_MSC");
  MS.productRevision("1.0");
  MS.onRead(onRead);
  MS.onWrite(onWrite);
  MS.onStartStop(onStartStop);
  MS.mediaPresent(true);
  if (SD.numSectors() != 0) MS.begin(SD.numSectors(), SD.cardSize() / SD.numSectors());
  else sd_inited = false;
  // delay(15000);
}

// ######## Mass Storage connect ################################
void connectMS(bool connect) {
  if (connect) {
    MS.mediaPresent(true);
  } else {
    MS.mediaPresent(false);
  }
}



#endif