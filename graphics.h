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

#include "glxwindow.h"
#include "timepoint.h"

#include <string>

#include <GL/gl.h>
#include <GL/glx.h>

class Timepoint;

struct TextLine
{
  TextLine(const std::string& s, int w=1) :
    text(s), r(1.0), g(1.0), b(1.0), stroke(w) {}

  TextLine(const std::string& s, double rr, double gg, double bb,
           int w=1) :
    text(s), r(rr), g(gg), b(bb), stroke(w) {}

  std::string text;
  double r;
  double g;
  double b;
  int stroke;
};

class Graphics : public GlxWindow
{
public:
  Graphics(const char* winname,
           int width, int height, int depth);

  ~Graphics();

  // This gives a hook to allow movie frames to be recorded during the
  // delay
  void gfxWait(const Timepoint& t, double delaySeconds);

  void clearBackBuffer();

  // Returns the msec elapsed since the last swapBuffers(), or the
  // last resetFrameTimer().
  double swapBuffers();

  void drawMessage(const std::string& word);
  void drawCross();
  void drawCross(int x, int y);

  void drawLine(double x1, double y1, double x2, double y2);

  void drawStrings(const std::string* strings, int nstrings,
                   double xpos, double ypos, double char_width,
                   int stroke_width);

  void drawText(const TextLine* text, int ntext,
                double xpos, double ypos, double char_width);

  void writePixmap(int x, int y, unsigned char* ptr, int size);

  // To control starting, stopping of movie recording
  void startRecording();
  void stopRecording();

  void resetFrameTimer() { itsFrameTimer.reset(); }

private:
  void dumpFrames(int count);

  bool isItRecording;
  int itsFrameCounter;
  Timepoint itsFrameTimer;
};

static const char vcid_graphics_h[] = "$Id$ $HeadURL$";
#endif // !GRAPHICS_H_DEFINED
