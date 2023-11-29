/*  ----------- GuiSetup.h -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: 
*
*
*
*
*/

#include "GuiClass/Gui.h"
#include <TFT_22_ILI9225.h>

void loadP2();
void loadP3();
void loadP5();
void loadMainPage();
void resetFileIndexCallback();
void restartEPSCallback();


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
TextBox flowValue(50, 90, 50, textH, "flowval", COLOR_LIGHTGREY);


//Page 3 Hcell
GuiPage p3(&gui, 6, 1);
Button back_p3(5, 5, 20, 20, "<", COLOR_RED);
TextBox serialnr(5, 30, textW, textH, "Seriennummer: ", COLOR_LIGHTGREY);
TextBox voltage(5, 50, textW, textH, "Spannung: ", COLOR_LIGHTGREY);
TextBox current(5, 70, textW, textH, "Strom: ", COLOR_LIGHTGREY);
TextBox h2temp(5, 90, textW, textH, "Stack-Temperatur: ", COLOR_LIGHTGREY);
TextBox h2pres(5, 110, textW, textH, "Stack-Druck: ", COLOR_LIGHTGREY);


//Page 5 Settings
GuiPage p5(&gui, 5, 5);
Button back_p5(5, 5, 20, 20, "<", COLOR_RED);
TextBox resetFileIndexText(5, 30, 1, textH, "File Index: ", COLOR_LIGHTGREY);
Button resetFileIndexButton(100, 30, 60, buttonH, "Reset", COLOR_RED);
Button restartESPButton(5, 55, 155, buttonH, "Neustart", COLOR_RED);


//Create statusbar page
GuiPage statusPage(&statusBar, 1, 5);
TextBox looptimeText(5, 5, 35, 20, "0us", COLOR_YELLOW);
TextBox usbStatus(45, 5, 45, 20, "no USB", COLOR_ORANGE);
TextBox recordingStatus(90, 5, 40, 20, "rec", COLOR_ORANGE);
TextBox hcellStatus(130, 5, 40, 20, "off", COLOR_ORANGE);
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

  //Add page 3 elements
  p3.setBGColor(COLOR_LIGHTGREY);
  p3.addElement(&back_p3, 0, 0);
  p3.addElement(&serialnr, 1, 0);
  p3.addElement(&voltage, 2, 0);
  p3.addElement(&current, 3, 0);
  p3.addElement(&h2temp, 4, 0);
  p3.addElement(&h2pres, 5, 0);
  p3.setCursor(0, 0);

  //Add page 5 elements
  p5.setBGColor(COLOR_LIGHTGREY);
  p5.addElement(&back_p5, 0, 0);
  p5.addElement(&resetFileIndexText, 1, 1);
  p5.addElement(&resetFileIndexButton, 1, 0);
  p5.addElement(&restartESPButton, 2, 0);
  p5.setCursor(0, 0);

  //callbacks
  a1.onClick(&loadP2);
  a2.onClick(&loadP3);
  a4.onClick(&loadP5);
  back_p2.onClick(&loadMainPage);
  back_p3.onClick(&loadMainPage);
  back_p5.onClick(&loadMainPage);
  resetFileIndexButton.onClick(&resetFileIndexCallback);
  restartESPButton.onClick(&restartEPSCallback);

  statusPage.setBGColor(COLOR_RED);
  statusPage.addElement(&looptimeText, 0, 0);
  statusPage.addElement(&usbStatus, 0, 1);
  statusPage.addElement(&recordingStatus, 0, 2);
  statusPage.addElement(&hcellStatus, 0, 3);
  statusPage.addElement(&sdStatus, 0, 4);


  gui.loadPage(&mainPage);
  statusBar.loadPage(&statusPage);
}

void resetFileIndexCallback() {
  resetFileIndex();
}

void restartEPSCallback() {
  ESP.restart();
}

void loadP2() {
  gui.loadPage(&p2);
}

void loadP3() {
  gui.loadPage(&p3);
}

void loadP5() {
  gui.loadPage(&p5);
}

void loadMainPage() {
  gui.loadPage(&mainPage);
}
