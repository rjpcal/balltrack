///////////////////////////////////////////////////////////////////////
//
// starbasegfx.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:52:52 2000
// written: Tue Feb 29 14:52:15 2000
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

  virtual void initWindow();

  virtual void wrapGraphics();

  virtual void clearFrontBuffer();
  virtual void clearBackBuffer();
  virtual void clearUpperPlanes();

  virtual void waitVerticalRetrace();
  virtual void swapBuffers();

  virtual void writeUpperPlanes();
  virtual void writeLowerPlanes();
  virtual void writeAllPlanes();

  virtual void drawMessage(char word[]);
  virtual void drawCross();

  virtual void setTransparent();

  virtual void showMenu(char menu[][STRINGSIZE], int nmenu);
  virtual void showParams(char params[][STRINGSIZE], int nparams);

  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size);
  virtual void writeTrueColorMap(unsigned char* ptr, int x, int y, int size);
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny);

private:
  int fildes() const { return itsFildes; }

  void setOpaque();

  // called from showMenu(), showParams()
  void setText(int wd, int ht);

  void waitFrameCount(int number);

  // called from initWindow()
  void checkFrameTime();
  void sizeColormap();
  void saveColormap();
  void newColormap();
  void restoreColormap();	// called from MenuApplication::wrap()

  XStuff* itsXStuff;

  int itsFildes;
};

static const char vcid_starbasegfx_h[] = "$Header$";
#endif // !STARBASEGFX_H_DEFINED
