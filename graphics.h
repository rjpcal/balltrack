///////////////////////////////////////////////////////////////////////
//
// graphics.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:00:48 2000
// written: Wed Sep  3 16:07:58 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_H_DEFINED
#define GRAPHICS_H_DEFINED

#include "defs.h"
#include "xstuff.h"

#include <GL/gl.h>
#include <GL/glx.h>

class XStuff;
class XHints;
class SimpleMovie;

class Graphics
{
public:
  Graphics(const char* winname,
           int width, int height, int depth);

  ~Graphics();

  XStuff& xstuff() { return itsXStuff; }

  void initWindow();

  void wrapGraphics();

  // This gives a hook to allow movie frames to be recorded during the delay
  void gfxWait(double delaySeconds);

  // Returns the number of milliseconds per graphics frame
  double frameTime();

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  void clearFrontBuffer();
  void clearBackBuffer();

  void waitVerticalRetrace();
  void swapBuffers();

  void drawMessage(char word[]);
  void drawCross();
  void drawCross(int x, int y);

  void showMenu(char menu[][STRINGSIZE], int nmenu);
  void showParams(char params[][STRINGSIZE], int nparams);

  void writeTrueColorMap(unsigned char* ptr, int x, int y, int size);
  void moveBlock(int x, int y, int xsz, int ysz, int nx, int ny);

  // To control starting, stopping of movie recording
  void startRecording();
  void stopRecording();

private:
  void waitFrameCount(int number);
  double computeFrameTime();
  void clearBackBufferRegion(bool use_scissor);

  int itsWidth;
  int itsHeight;
  double itsFrameTime;

  XStuff itsXStuff;
  GLXContext itsGLXContext;

  int itsClearIndex;

  SimpleMovie* itsMovie;
  bool isItRecording;
};

static const char vcid_graphics_h[] = "$Header$";
#endif // !GRAPHICS_H_DEFINED
