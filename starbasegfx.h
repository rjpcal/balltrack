///////////////////////////////////////////////////////////////////////
//
// starbasegfx.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:52:52 2000
// written: Thu Feb 24 15:04:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STARBASEGFX_H_DEFINED
#define STARBASEGFX_H_DEFINED

#include "graphics.h"

class Application;

class XStuff;

class StarbaseGfx : public Graphics {
public:
  StarbaseGfx(XStuff* xinfo, int aWidth, int aHeight);

  void wrapGraphics();

  virtual int width() const;
  virtual int height() const;

  virtual void writeUpperPlanes();
  virtual void writeLowerPlanes();
  virtual void writeAllPlanes();

  virtual void waitVerticalRetrace();

  virtual void waitFrameCount(int number);

  virtual void drawMessage(char word[]);
  virtual void drawCross();
  virtual void clearUpperPlanes();

  virtual void setOpaque();
  virtual void setTransparent();

  // called from Application::Application()
  virtual void initWindow();

  // called from many
  virtual void clearWindow();

  // called from MenuApplication::makeMenu() and <params.c>::SetParametersN()
  virtual void showMenu(char menu[][STRINGSIZE], int nmenu);

  // called from <params.c>::ListParams()
  virtual void showParams(char params[][STRINGSIZE], int nparams);

  // called from showMenu(), showParams(), setMessage()
  virtual void setText(int wd, int ht);

  // called from BallsExpt:runApplication()
  virtual void setMessage();

  // called from Ball::move()
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny);

  // called from Ball::draw()
  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size);

private:
  int fildes() const { return itsFildes; }

  // called from initWindow()
  void checkFrameTime();
  void sizeColormap();
  void saveColormap();
  void newColormap();
  void restoreColormap();	// called from MenuApplication::wrap()

  int itsFildes;
  int itsWidth;
  int itsHeight;
};


static const char vcid_starbasegfx_h[] = "$Header$";
#endif // !STARBASEGFX_H_DEFINED
