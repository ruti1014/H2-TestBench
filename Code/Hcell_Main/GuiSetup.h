#include "Gui.h"
#include <TFT_22_ILI9225.h>



void a1Callback();
void backp2Callback();

TFTGraphics tft(&tft_ili);
int numPages = 1;
Gui statusBar(&tft, 0, 0, 220, 30, 1);
Gui gui(&tft, 0, 30, 220, 146, numPages);

//Create Pages
//Page 1 - MainPage
GuiPage mainPage(&gui, 5, 2);
//Create elements
int buttonW = 110;
int buttonH = 20;
TextBox t0(90, 5, 1, 20, "Start", COLOR_LIGHTGREY);  //Page Title
Button a1(10, 30, buttonW, buttonH, "Sensoren", COLOR_RED);
Button a2(10, 55, buttonW, buttonH, "Brennstoffzelle", COLOR_RED);
Button a3(10, 80, buttonW, buttonH, "Fehlercodes", COLOR_RED);
TextBox t1(130, 80, 70, buttonH, "No Errors", COLOR_GREEN);  //Maybe to indicate if errors are found
Button a4(10, 105, buttonW, buttonH, "Einstellungen", COLOR_RED);


//Page 2 SensorDataPage
int textH = 20;
int textW = 1;
GuiPage p2(&gui, 5, 4);
Button back_p2(5, 5, 20, 20, "<", COLOR_RED);
TextBox title_p2(90, 5, 50, 20, "Sensoren", COLOR_LIGHTGREY);  //Title
TextBox bme1_text(5, 30, textW, textH, "BME 1:", COLOR_LIGHTGREY);
TextBox bme2_text(5, 50, textW, textH, "BME 2:", COLOR_LIGHTGREY);
TextBox leaksens(5, 70, textW, textH, "Leak:", COLOR_LIGHTGREY);
TextBox flowsens(5, 90, textW, textH, "Flow:", COLOR_LIGHTGREY);
//Data
TextBox bme1_temp(50, 30, 50, textH, "temp", COLOR_LIGHTGREY);
TextBox bme1_hum(100, 30, 50, textH, "hum", COLOR_LIGHTGREY);
TextBox bme1_pres(150, 30, 50, textH, "pres", COLOR_LIGHTGREY);

TextBox bme2_temp(50, 50, 50, textH, "temp", COLOR_LIGHTGREY);
TextBox bme2_hum(100, 50, 50, textH, "hum", COLOR_LIGHTGREY);
TextBox bme2_pres(150, 50, 50, textH, "pres", COLOR_LIGHTGREY);

TextBox leakValue(50, 70, 50, textH, "leakval", COLOR_LIGHTGREY);
TextBox flowValue(50, 90, 50, textH, "leakval", COLOR_LIGHTGREY);

//Create statusbar page
GuiPage statusPage(&statusBar, 1, 4);
TextBox looptimeText(10, 5, 40, 20, "0us", COLOR_ORANGE);
TextBox recordingStatus(70, 5, 40, 20, "rec", COLOR_ORANGE);
TextBox hcellStatus(120, 5, 40, 20, "off", COLOR_ORANGE);
TextBox sdStatus(170, 5, 40, 20, "no sd", COLOR_ORANGE);

void setupGui() {
  //Add page 1 elements
  mainPage.setBGColor(COLOR_LIGHTGREY);
  mainPage.addElement(&t0, 0, 0);
  mainPage.addElement(&a1, 1, 0);
  mainPage.addElement(&a2, 2, 0);
  mainPage.addElement(&a3, 3, 0);
  mainPage.addElement(&t1, 3, 1);
  mainPage.addElement(&a4, 4, 0);
  mainPage.setCursor(1, 0);

  //Add page 2 elements
  p2.setBGColor(COLOR_LIGHTGREY);
  p2.addElement(&back_p2, 0, 0);
  p2.addElement(&title_p2, 0, 1);
  p2.addElement(&bme1_text, 1, 0);
  p2.addElement(&bme2_text, 2, 0);
  p2.addElement(&leaksens, 3, 0);
  p2.addElement(&flowsens, 4, 0);
  //Data
  p2.addElement(&bme1_temp, 1, 1);
  p2.addElement(&bme1_hum, 1, 2);
  p2.addElement(&bme1_pres, 1, 3);
  p2.addElement(&bme2_temp, 2, 1);
  p2.addElement(&bme2_hum, 2, 2);
  p2.addElement(&bme2_pres, 2, 3);
  p2.addElement(&leakValue, 3, 1);
  p2.addElement(&flowValue, 4, 1);

  p2.setCursor(0, 0);

  //callbacks
  a1.onClick(&a1Callback);
  back_p2.onClick(&backp2Callback);

  statusPage.setBGColor(COLOR_RED);
  statusPage.addElement(&looptimeText, 0, 0);
  statusPage.addElement(&recordingStatus, 0, 1);
  statusPage.addElement(&hcellStatus, 0, 2);
  statusPage.addElement(&sdStatus, 0, 3);


  gui.loadPage(&mainPage);
  statusBar.loadPage(&statusPage);
}


void a1Callback() {
  gui.loadPage(&p2);
}

void backp2Callback() {
  gui.loadPage(&mainPage);
}
