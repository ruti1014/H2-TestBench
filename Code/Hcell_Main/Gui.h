/*  ----------- Gui.h -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: 
*
*
*
*
*/

#ifndef GUI_H
#define GUI_H
#include <Arduino.h>
#include "GraphicsAdapter.h"

class Element;
class GuiPage;

enum Direction {
  LEFT,
  RIGHT,
  UP,
  DOWN
};


class Gui {
public:
  Gui(GraphicsAdapter* ga, int posX, int posY, int width, int height, int pageNum);
  void addPage(GuiPage* page);
  GuiPage* getCurrentPage();
  void loadPage(GuiPage* page);
  void loadLastPage();
  GraphicsAdapter* getGraphicsAdapter();
  void clickCursor();
  void selectCursor();
  void moveCursor(Direction d);
  int getX();
  int getY();
  int getWidth();
  int getHeight();

private:

  int _pageNum, _pageCount;
  GraphicsAdapter* _ga;
  GuiPage* _currentPage;
  GuiPage** _pageList;
  int _posX, _posY, _width, _height;
};


class GuiPage {
public:
  GuiPage(Gui* gui, int row, int column);
  void drawPage();
  void fillPage(uint16_t color);
  void addElement(Element* element, int row, int _columns);
  Element* getSelectedElement();
  void setCursor(int row, int column);
  void moveCursor(Direction d);
  void setBGColor(uint16_t color);
  int getBGColor();
private:
  uint16_t _BGcolor = 0xFF;
  int _rows, _columns;
  Gui* _gui;
  GuiPage* _lastPage;
  //Element* _cursor;
  int _cursorR, _cursorC;
  Element*** _elementMatrix;  //pointer matrix to elements _elementMatrix[row][column]
};





class Element {
public:
  Element(int posX, int posY, int width, int height, int color);
  virtual void drawElement() = 0;
  virtual void hoverElement() = 0;
  virtual void selectElement() = 0;
  void setSelectable(bool selectable);
  bool isSelectable();
  void click();
  void onClick(void (*clickCallback)(void));
  void setGui(Gui* gui);

  //element Customization
  void setBGColor(int color);
  void setHoverColor(int color);
  void setSelectColor(int color);
  void setSelectTextColor(int color);

protected:
  Gui* _gui;
  int _posX;
  int _posY;
  int _width;
  int _height;
  uint16_t _color;
  uint16_t _bgColor;
  uint16_t _hoverColor;
  uint16_t _selectColor;
  uint16_t _selectTextColor;
  bool _selectable = true;
  void (*_clickCallback)(void) = NULL;  //Callback function for click event
};


//Custom Elements
class Button : public Element {
public:
  Button(int posX, int posY, int width, int height, String text, int color);
  void drawElement() override;
  void hoverElement() override;
  void selectElement() override;
  //Element customization
  void setText(String text);
  void setTextColor(int color);
private:
  void drawButton(int textColor, int bgColor, int lineColor);
  String _text;
  uint16_t _textColor;
};

class TextBox : public Element {
public:
  TextBox(int posX, int posY, int width, int height, String text, int color);
  void drawElement() override;
  void hoverElement() override;
  void selectElement() override;

  void setText(String text, int color = -1); //-1 will use default _textcolor
  void setTextColor(int color);
private:
  String _text;
  uint16_t _textColor;
};


#endif