///////////////////////////////////////////////////////////////////////
//
// openglgfx.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 15:03:44 2000
// written: Tue Feb 29 10:43:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OPENGLGFX_H_DEFINED
#define OPENGLGFX_H_DEFINED

#include "graphics.h"

#include <GL/gl.h>
#include <GL/glx.h>

class Application;

class XStuff;

class XHints;

class OpenglGfx : public Graphics {
public:
  OpenglGfx(XStuff* xinfo, const XHints& hints, int aWidth, int aHeight);

  virtual void makeCurrent();

  virtual void wrapGraphics();

  virtual void writeUpperPlanes();
  virtual void writeLowerPlanes();
  virtual void writeAllPlanes();

  virtual void waitVerticalRetrace();
  virtual void swapBuffers();

  virtual void drawMessage(char word[]);
  virtual void drawCross();
  virtual void clearUpperPlanes();

  virtual void setTransparent();

  // called from Application::Application()
  virtual void initWindow();

  // called from many
  virtual void clearWindow();

  virtual void clearBackBuffer();

  // called from MenuApplication::makeMenu() and <params.c>::SetParametersN()
  virtual void showMenu(char menu[][STRINGSIZE], int nmenu);

  // called from <params.c>::ListParams()
  virtual void showParams(char params[][STRINGSIZE], int nparams);

  // called from Ball::move()
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny);

  // called from Ball::draw()
  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size);

  virtual void writeTrueColorMap(unsigned char* ptr, int x, int y, int size);

private:
  void waitFrameCount(int number);

  // called from initWindow()
  void checkFrameTime();
  void sizeColormap();
  void saveColormap();
  void newColormap();
  void restoreColormap();	// called from MenuApplication::wrap()

  XStuff* itsXStuff;
  GLXContext itsGLXContext;

  int itsClearIndex;
};

static const char vcid_openglgfx_h[] = "$Header$";
#endif // !OPENGLGFX_H_DEFINED
