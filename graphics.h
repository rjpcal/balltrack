///////////////////////////////////////////////////////////////////////
//
// graphics.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:00:48 2000
// written: Thu Feb 24 15:01:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_H_DEFINED
#define GRAPHICS_H_DEFINED

#include "defs.h"

class Application;

class XStuff;

class Graphics {
public:
  // Virtual destructor ensures proper destruction of base classes.
  virtual ~Graphics();

  virtual void wrapGraphics() = 0;

  virtual int width() const = 0;
  virtual int height() const = 0;

  virtual void writeUpperPlanes() = 0;
  virtual void writeLowerPlanes() = 0;
  virtual void writeAllPlanes() = 0;

  virtual void waitVerticalRetrace() = 0;

  virtual void waitFrameCount(int number) = 0;

  virtual void drawMessage(char word[]) = 0;
  virtual void drawCross() = 0;
  virtual void clearUpperPlanes() = 0;

  virtual void setOpaque() = 0;
  virtual void setTransparent() = 0;

  // called from Application::Application()
  virtual void initWindow() = 0;

  // called from many
  virtual void clearWindow() = 0;

  // called from MenuApplication::makeMenu() and <params.c>::SetParametersN()
  virtual void showMenu(char menu[][STRINGSIZE], int nmenu) = 0;

  // called from <params.c>::ListParams()
  virtual void showParams(char params[][STRINGSIZE], int nparams) = 0;

  // called from showMenu(), showParams(), setMessage()
  virtual void setText(int wd, int ht) = 0;

  // called from BallsExpt:runApplication()
  virtual void setMessage() = 0;

  // called from Ball::move()
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny) = 0;

  // called from Ball::draw()
  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size) = 0;
};

static const char vcid_graphics_h[] = "$Header$";
#endif // !GRAPHICS_H_DEFINED