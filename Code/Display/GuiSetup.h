#include "Gui.h"
// define SPI Pins:
#define TFT_RST 26  // IO 26
#define TFT_RS 25   // IO 25
#define TFT_CLK 14  // HSPI-SCK
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS 15   // HSPI-SS0
#define TFT_LED 3   // 0 if wired to +5V directly
SPIClass hspi(HSPI);
TFT_22_ILI9225 tft_ili = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, 200);


TFTGraphics tft(&tft_ili);
int numPages = 1;
Gui statusBar(&tft, 0, 0, 220, 30, 1);
Gui gui(&tft, 0, 30, 220, 146, numPages);

//Create Pages
//Page 1 - MainPage
GuiPage mainPage(&gui, 2, 4);
//Create elements
Button a1(10, 10, 20, 20, COLOR_RED);
Button a2(40, 10, 20, 20, COLOR_RED);
Button a3(70, 10, 20, 20, COLOR_RED);
Button a4(100, 10, 20, 20, COLOR_RED);
Button a5(10, 40, 20, 20, COLOR_RED);
Button a6(40, 40, 20, 20, COLOR_RED);
Button a7(70, 40, 20, 20, COLOR_RED);
Button a8(100, 40, 20, 20, COLOR_RED);

//Create statusbar page
GuiPage statusPage(&statusBar, 1, 3);
Button b1(10, 5, 60, 20, COLOR_ORANGE);
Button b2(80, 5, 60, 20, COLOR_ORANGE);
Button b3(150, 5, 60, 20, COLOR_ORANGE);

void setupGui() {
  mainPage.setBGColor(COLOR_LIGHTGREY);
  mainPage.addElement(&a1, 0, 0);
  mainPage.addElement(&a2, 0, 1);
  mainPage.addElement(&a3, 0, 2);
  mainPage.addElement(&a4, 0, 3);
  mainPage.addElement(&a5, 1, 0);
  mainPage.addElement(&a6, 1, 1);
  mainPage.addElement(&a7, 1, 2);
  mainPage.addElement(&a8, 1, 3);
  mainPage.setCursor(0, 0);

  statusPage.setBGColor(COLOR_DARKBLUE);
  statusPage.addElement(&b1, 0, 0);
  statusPage.addElement(&b2, 0, 1);
  statusPage.addElement(&b3, 0, 2);

  gui.loadPage(&mainPage);
  statusBar.loadPage(&statusPage);
}


void loopTimeMS(){
  static int frameTime = 0;
  static int timeStamp = millis();
  static int avgCount = 0;
  int avgNum = 100;

  if (avgCount >= avgNum){
    frameTime = frameTime/avgNum;
    Serial.print("Average frametime: "); Serial.println(frameTime);
    frameTime = 0;
    avgCount = 0;
  }else{
    frameTime += millis() - timeStamp;
    timeStamp = millis();
    avgCount++;
  }

}
