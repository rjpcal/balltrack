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

#include "xstuff.h"

#include <string>

#include <GL/gl.h>
#include <GL/glx.h>

class XStuff;
class XHints;
class SimpleMovie;
class Timer;

class Graphics
{
public:
  Graphics(const char* winname,
           int width, int height, int depth);

  ~Graphics();

  XStuff& xstuff() { return itsXStuff; }

  void getInt(int& i) const { itsXStuff.getInt(i); }
  void getDouble(double& d) const { itsXStuff.getDouble(d); }

  void initWindow();

  // This gives a hook to allow movie frames to be recorded during the
  // delay
  void gfxWait(Timer& t, double delaySeconds);

  // Returns the number of milliseconds per graphics frame
  double frameTime();

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  void clearFrontBuffer();
  void clearBackBuffer();

  void swapBuffers();

  void drawMessage(const std::string& word);
  void drawCross();
  void drawCross(int x, int y);

  void drawStrings(const std::string* strings, int nstrings,
                   double xpos, double ypos, double char_width);

  void writePixmap(int x, int y, unsigned char* ptr, int size);

  // To control starting, stopping of movie recording
  void startRecording(int width, int height);
  void stopRecording();

private:
  void waitFrameCount(int number);
  double computeFrameTime();

  int itsWidth;
  int itsHeight;
  double itsFrameTime;

  XStuff itsXStuff;
  GLXContext itsGLXContext;

  SimpleMovie* itsMovie;
  bool isItRecording;
};

static const char vcid_graphics_h[] = "$Header$";
#endif // !GRAPHICS_H_DEFINED
