#define HWSerial Serial0

#define SPI_MOSI 41  // Display_SDI
#define SPI_MISO 42  // Display_RS
#define SPI_SCK 40
#define Display_SPI_CS 21

#define TFT_RST 48
#define Display_LED 0  // 0 if wired to +5V directly

#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include "GuiSetup.h"




SPIClass* Display_SPI = NULL;


void setup() {
  HWSerial.begin(115200);
  HWSerial.println("start");

  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

  tft_ili.begin(*Display_SPI);
  tft_ili.setOrientation(1);
  tft_ili.setFont(Terminal6x8);
  tft_ili.clear();
  HWSerial.println("Display setup");
  tft_ili.drawText(90, 30, "START", COLOR_RED);
  delay(1000);
  tft_ili.clear();
  setupGui();

  delay(1000);
  gui.moveCursor(RIGHT);
  delay(1000);
  gui.moveCursor(RIGHT);
  delay(1000);
  gui.selectCursor();
  delay(250);
  gui.clickCursor();
  delay(500);
}

void loop() {
  loopTimeMS();
  //delay(1000);
  gui.moveCursor(RIGHT);
  //delay(1000);
  gui.moveCursor(DOWN);
}


void loopTimeMS() {
  static int frameTime = 0;
  static int timeStamp = millis();
  static int avgCount = 0;
  int avgNum = 100;

  if (avgCount >= avgNum) {
    HWSerial.print("Accumulated frametime: ");
    HWSerial.println(frameTime);
    frameTime = (frameTime / avgNum);
    HWSerial.print("Average frametime: ");
    HWSerial.println(frameTime);
    String fps = String(frameTime) + " ms";
    t1.setText(fps);
    frameTime = 0;
    avgCount = 0;
  } else {
    frameTime += millis() - timeStamp;
    timeStamp = millis();
    avgCount++;
  }
}