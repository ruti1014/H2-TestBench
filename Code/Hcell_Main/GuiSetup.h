#include "Gui.h"
#include <TFT_22_ILI9225.h>



void a4Callback();
void p2b1Callback();
void a8Callback();

TFTGraphics tft(&tft_ili);
int numPages = 1;
Gui statusBar(&tft, 0, 0, 220, 30, 1);
Gui gui(&tft, 0, 30, 220, 146, numPages);

//Create Pages
//Page 1 - MainPage
GuiPage mainPage(&gui, 3, 4);
//Create elements
TextBox t0(70, 5, 1, 20, "Start", COLOR_LIGHTGREY);
Button a1(10, 30, 20, 20, "A1", COLOR_RED);
Button a2(40, 30, 20, 20, "A2", COLOR_RED);
//Button a3(70, 10, 20, 20, "A3", COLOR_RED);
TextBox t1(70, 30, 20, 20, "T0", COLOR_GREEN);
Button a4(100, 30, 20, 20, ">", COLOR_RED);
Button a5(10, 60, 20, 20, "A5", COLOR_RED);
Button a6(40, 60, 20, 20, "A6", COLOR_RED);
Button a7(70, 60, 20, 20, "A7", COLOR_RED);
Button a8(100, 60, 20, 20, "A8", COLOR_RED);


GuiPage p2(&gui, 4, 2);
Button p2b1(5, 5, 20, 20, "<", COLOR_RED);
TextBox p2t0(70, 5, 1, 20, "Daten", COLOR_LIGHTGREY);
TextBox p2t1(30, 30, 1, 20, "Temperatur:", COLOR_LIGHTGREY);
TextBox p2t2(30, 45, 1, 20, "Feuchtigkeit:", COLOR_LIGHTGREY);
TextBox p2t3(30, 60, 1, 20, "Druck:", COLOR_LIGHTGREY);


//Create statusbar page
GuiPage statusPage(&statusBar, 1, 4);
TextBox looptimeText(10, 5, 40, 20, "0us", COLOR_ORANGE);
TextBox recordingStatus(70, 5, 40, 20, "rec", COLOR_SIENNA);
TextBox hcellStatus(120, 5, 40, 20, "off", COLOR_ORANGE);
TextBox sdStatus(170, 5, 40, 20, "no sd", COLOR_ORANGE);


void setupGui() {
  mainPage.setBGColor(COLOR_LIGHTGREY);
  mainPage.addElement(&a1, 0, 0);
  mainPage.addElement(&a2, 0, 1);
  //mainPage.addElement(&a3, 0, 2);
  mainPage.addElement(&a4, 0, 3);
  mainPage.addElement(&a5, 1, 0);
  mainPage.addElement(&a6, 1, 1);
  mainPage.addElement(&a7, 1, 2);
  mainPage.addElement(&a8, 1, 3);
  mainPage.addElement(&t0, 0, 2);
  mainPage.addElement(&t1, 2, 0);
  mainPage.setCursor(0, 0);

  p2.setBGColor(COLOR_LIGHTGREY);
  p2.addElement(&p2b1, 0, 0);
  p2.addElement(&p2t0, 0, 1);
  p2.addElement(&p2t1, 1, 1);
  p2.addElement(&p2t2, 2, 1);
  p2.addElement(&p2t3, 3, 1);

  p2.setCursor(0, 0);

  //callbacks
  a4.onClick(&a4Callback);
  p2b1.onClick(&p2b1Callback);
  a8.onClick(&a8Callback);

  statusPage.setBGColor(COLOR_RED);
  statusPage.addElement(&looptimeText, 0, 0);
  statusPage.addElement(&recordingStatus, 0, 1);
  statusPage.addElement(&hcellStatus, 0, 2);
  statusPage.addElement(&sdStatus, 0, 3);


  gui.loadPage(&mainPage);
  statusBar.loadPage(&statusPage);
}


void a4Callback() {
  gui.loadPage(&p2);
}

void p2b1Callback() {
  gui.loadPage(&mainPage);
}

void a8Callback() {
  resetFileIndex();
}