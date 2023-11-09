#include <stdexcept>
#include "Gui.h"




//########### GUI ###########
Gui::Gui(GraphicsAdapter* ga, int posX, int posY, int width, int height, int pageNum)
  : _ga(ga), _posX(posX), _posY(posY), _width(width), _height(height), _pageNum(pageNum) {

  _pageCount = 0;
  _pageList = new GuiPage*[_pageNum];
  _currentPage = NULL;
}

void Gui::addPage(GuiPage* page) {
  _pageList[_pageCount] = page;
}

GuiPage* Gui::getCurrentPage() {
  return _currentPage;
}

void Gui::loadPage(GuiPage* page) {
  _currentPage = page;
  _currentPage->drawPage();
}

void Gui::loadLastPage() {}

GraphicsAdapter* Gui::getGraphicsAdapter() {
  return _ga;
}

void Gui::moveCursor(Direction d) {
  _currentPage->getSelectedElement()->drawElement();
  _currentPage->moveCursor(d);
  _currentPage->getSelectedElement()->hoverElement();
}

  int Gui::getX(){
    return _posX;
  }
  int Gui::getY(){
    return _posY;
  }
  int Gui::getWidth(){
    return _width;
  }
  int Gui::getHeight(){
    return _height;
  }



//########### GUI Page ###########
GuiPage::GuiPage(Gui* gui, int row, int column)
  : _gui(gui), _rows(row), _columns(column) {

  //allocate memory for element matrix
  _elementMatrix = new Element**[_rows];
  for (int i = 0; i < _rows; i++) {
    _elementMatrix[i] = new Element*[_columns];
    //initialize matrix
    for (int j = 0; j < _columns; j++) {
      _elementMatrix[i][j] = NULL;
    }
  }
}

void GuiPage::drawPage() {
  fillPage(_BGcolor);
  for (int r = 0; r < _rows; r++) {
    for (int c = 0; c < _columns; c++) {
      if (_elementMatrix[r][c] != NULL) _elementMatrix[r][c]->drawElement();
      else Serial.println("Failed to load element");
    }
  }
}

void GuiPage::fillPage(uint16_t color){
  _gui->getGraphicsAdapter()->drawFillRect(_gui->getX(), _gui->getY(),_gui->getX() + _gui->getWidth(), _gui->getY()+_gui->getHeight(), color);
}

void GuiPage::addElement(Element* element, int row, int column) {
  _elementMatrix[row][column] = element;
  _elementMatrix[row][column]->setGui(_gui);
}

Element* GuiPage::getSelectedElement() {

  return _elementMatrix[_cursorR][_cursorC];
}

void GuiPage::setCursor(int row, int column) {
  getSelectedElement()->drawElement();
  if (row < _rows) _cursorR = row;
  if (column < _columns) _cursorC = column;
  getSelectedElement()->hoverElement();
}

void GuiPage::moveCursor(Direction d) {

  switch (d) {
    case LEFT:
      _cursorC--;
      if (_cursorC < 0) _cursorC = _columns;
      break;
    case RIGHT:
      _cursorC++;
      if (_cursorC >= _columns) _cursorC = 0;
      break;
    case UP:
      _cursorR++;
      if (_cursorC >= _rows) _cursorC = 0;
      break;
    case DOWN:
      _cursorR--;
      if (_cursorR < 0) _cursorR = _rows;
      break;
  }
}

void GuiPage::setBGColor(uint16_t color){
  _BGcolor = color;
}


//########### Elements ###########
Element::Element(int posX, int posY, int width, int length, int color)
  : _posX(posX), _posY(posY), _width(width), _length(length), _color(color) {  }

void Element::onClick() {}

void Element::setGui(Gui* gui) {
  _gui = gui;
  _posX = _posX + gui->getX();
  _posY = _posY + gui->getY();
}


Button::Button(int posX, int posY, int length, int width, int color)
  : Element(posX, posY, length, width, color) {
  _gui = NULL;
}

void Button::drawElement() {
  if (_gui != NULL) _gui->getGraphicsAdapter()->drawRect(_posX, _posY, _posX + _width, _posY + _length, _color);
  else Serial.println("Button could not identify its gui");
}
void Button::hoverElement() {
  _gui->getGraphicsAdapter()->drawRect(_posX, _posY, _posX + _width, _posY + _length, _color / 2);
}
void Button::selectElement() {}
void Button::resetElement() {}
void Button::onClick() {}