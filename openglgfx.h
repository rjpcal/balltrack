///////////////////////////////////////////////////////////////////////
//
// openglgfx.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 15:03:44 2000
// written: Tue Feb 29 16:47:24 2000
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

  virtual void initWindow();

  virtual void wrapGraphics();

  virtual bool isRgba();

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

  virtual void loadColormap(float colors[][3], int ncolors);

  virtual void showMenu(char menu[][STRINGSIZE], int nmenu);
  virtual void showParams(char params[][STRINGSIZE], int nparams);

  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size);
  virtual void writeTrueColorMap(unsigned char* ptr, int x, int y, int size);
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny);

private:
  void waitFrameCount(int number);

  // called from initWindow()
  void checkFrameTime();
  void sizeColormap();

  XStuff* itsXStuff;
  GLXContext itsGLXContext;

  int itsClearIndex;
};

static const char vcid_openglgfx_h[] = "$Header$";
#endif // !OPENGLGFX_H_DEFINED
