///////////////////////////////////////////////////////////////////////
//
// graphics.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:00:48 2000
// written: Wed Sep  3 12:53:14 2003
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
  Graphics(int wid, int hgt);

  // Virtual destructor ensures proper destruction of base classes.
  virtual ~Graphics();

  virtual void initWindow() = 0;

  virtual void wrapGraphics() = 0;

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  virtual bool isRgba() = 0;
  virtual bool isDoubleBuffered() = 0;

  virtual void clearFrontBuffer() = 0;
  virtual void clearBackBuffer() = 0;
  virtual void clearUpperPlanes() = 0;

  virtual void waitVerticalRetrace() = 0;
  virtual void swapBuffers() = 0;

  virtual void writeUpperPlanes() = 0;
  virtual void writeLowerPlanes() = 0;
  virtual void writeAllPlanes() = 0;

  virtual void drawMessage(char word[]) = 0;
  virtual void drawCross() = 0;
  virtual void drawCross(int x, int y) = 0;

  virtual void loadColormap(float colors[][3], int ncolors) = 0;

  virtual void showMenu(char menu[][STRINGSIZE], int nmenu) = 0;
  virtual void showParams(char params[][STRINGSIZE], int nparams) = 0;

  virtual void writeBitmap(unsigned char* ptr, int x, int y, int size) = 0;
  virtual void writeTrueColorMap(unsigned char* ptr, int x, int y, int size) = 0;
  virtual void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny) = 0;

  // To control starting, stopping of movie recording
  virtual void startRecording();
  virtual void stopRecording();

  // This gives a hook to allow movie frames to be recorded during the delay
  virtual void gfxWait(double delaySeconds);

  // Returns the number of milliseconds per graphics frame
  double frameTime();

protected:
  virtual void waitFrameCount(int number) = 0;

private:
  double computeFrameTime();

  int itsWidth;
  int itsHeight;
  double itsFrameTime;
};

static const char vcid_graphics_h[] = "$Header$";
#endif // !GRAPHICS_H_DEFINED
