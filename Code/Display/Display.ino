#define HWSerial Serial0

#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include "GuiSetup.h"



int Buttons[5] = { 0, 0, 0, 0, 0 };

SPIClass* Display_SPI = NULL;


void setup() {
  HWSerial.begin(115200);
  HWSerial.println("start");

  //hspi.begin();
  //tft_ili.begin(hspi);

  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

  tft_ili.drawText(90, 30, "START", COLOR_RED);
  delay(1000);
  tft_ili.clear();
  setupGui();
}

void loop() {
  delay(1000);
  gui.moveCursor(RIGHT);
  delay(1000);
  gui.moveCursor(DOWN);
  loopTimeMS();
}
