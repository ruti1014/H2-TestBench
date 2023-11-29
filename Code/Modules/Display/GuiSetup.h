#include "Gui.h"
#include <TFT_22_ILI9225.h>



TFT_22_ILI9225 tft_ili = TFT_22_ILI9225(TFT_RST, SPI_MISO, Display_SPI_CS, Display_LED);


TFTGraphics tft(&tft_ili);
int numPages = 1;
Gui statusBar(&tft, 0, 0, 220, 30, 1);
Gui gui(&tft, 0, 30, 220, 146, numPages);

//Create Pages
//Page 1 - MainPage
GuiPage mainPage(&gui, 2, 4);
//Create elements
Button a1(10, 10, 20, 20, "A1", COLOR_RED);
Button a2(40, 10, 20, 20, "A2", COLOR_RED);
//Button a3(70, 10, 20, 20, "A3", COLOR_RED);
Button a4(100, 10, 20, 20, "A4", COLOR_RED);
Button a5(10, 40, 20, 20, "A5", COLOR_RED);
Button a6(40, 40, 20, 20, "A6", COLOR_RED);
Button a7(70, 40, 20, 20, "A7", COLOR_RED);
Button a8(100, 40, 20, 20, "A8", COLOR_RED);
TextBox t0(70, 10, 20, 20, "T0", COLOR_GREEN);

//Create statusbar page
GuiPage statusPage(&statusBar, 1, 3);
TextBox t1(10, 5, 60, 20, "T1", COLOR_ORANGE);
TextBox t2(80, 5, 60, 20, "T2", COLOR_ORANGE);
TextBox t3(150, 5, 60, 20, "T3", COLOR_ORANGE);

void setupGui() {
  mainPage.setBGColor(COLOR_LIGHTGREY);
  mainPage.addElement(&a1, 0, 0);
  mainPage.addElement(&a2, 0, 1);
  //mainPage.addElement(&a3, 0, 2);
  mainPage.addElement(&t0, 0, 2);
  mainPage.addElement(&a4, 0, 3);
  mainPage.addElement(&a5, 1, 0);
  mainPage.addElement(&a6, 1, 1);
  mainPage.addElement(&a7, 1, 2);
  mainPage.addElement(&a8, 1, 3);
  mainPage.setCursor(0, 0);

  statusPage.setBGColor(COLOR_RED);
  statusPage.addElement(&t1, 0, 0);
  statusPage.addElement(&t2, 0, 1);
  statusPage.addElement(&t3, 0, 2);

  gui.loadPage(&mainPage);
  statusBar.loadPage(&statusPage);
}
