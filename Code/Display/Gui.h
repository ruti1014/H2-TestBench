#ifndef GUI_H
#define GUI_H
#include <Arduino.h>
#include "GraphicsAdapter.h"

class Element;
class GuiPage;

enum Direction{
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
  void moveCursor(Direction d);
  int getX();
  int getY();
  int getWidth();
  int getHeight();

private:

  int _pageNum, _pageCount;
  GraphicsAdapter* _ga;
  GuiPage* _staticPage;
  GuiPage* _currentPage;
  GuiPage** _pageList;
  int _posX, _posY, _width, _height;
};


class GuiPage {
public:
  GuiPage(Gui* gui, int row, int column);
  void drawPage();
  void fillPage(uint16_t color);
  void addElement(Element* element, int row, int column);
  Element* getSelectedElement();
  void setCursor(int row, int column);
  void moveCursor(Direction d);
  void setBGColor(uint16_t color);
private:
  uint16_t _BGcolor = 0x00;
  int _rows, _columns;
  Gui* _gui;
  GuiPage* _lastPage;
  //Element* _cursor;
  int _cursorR = 0, _cursorC = 0;
  Element*** _elementMatrix;  //pointer matrix to elements _elementMatrix[row][column]
};





class Element {
public:
  Element(int posX, int posY, int width, int length, int color);
  virtual void drawElement() = 0;
  virtual void hoverElement() = 0;
  virtual void selectElement() = 0;
  virtual void resetElement() = 0;
  void onClick();
  void setGui(Gui* gui);
protected:
  Gui* _gui;
  int _posX;
  int _posY;
  int _length;
  int _width;
  uint16_t _color;
  bool _selectable;
};

class Button : public Element {
public:
  Button(int posX, int posY, int width, int length, int color);
  void drawElement() override;
  void hoverElement() override;
  void selectElement() override;
  void resetElement() override;
  void onClick();
};




#endif