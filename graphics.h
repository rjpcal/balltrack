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

  // This gives a hook to allow movie frames to be recorded during the
  // delay
  void gfxWait(Timer& t, double delaySeconds);

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  void clearBackBuffer();

  void swapBuffers();

  void drawMessage(const std::string& word);
  void drawCross();
  void drawCross(int x, int y);

  void drawLine(double x1, double y1, double x2, double y2);

  void drawStrings(const std::string* strings, int nstrings,
                   double xpos, double ypos, double char_width);

  void writePixmap(int x, int y, unsigned char* ptr, int size);

  // To control starting, stopping of movie recording
  void startRecording(int width, int height);
  void stopRecording();

private:
  double computeFrameTime();

  const int itsWidth;
  const int itsHeight;

  XStuff itsXStuff;
  GLXContext itsGLXContext;

  bool itsUsingVsync;

  bool isItRecording;
};

static const char vcid_graphics_h[] = "$Header$";
#endif // !GRAPHICS_H_DEFINED
