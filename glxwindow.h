///////////////////////////////////////////////////////////////////////
//
// glxwindow.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:27 2000
// written: Wed Sep  3 16:12:22 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_H_DEFINED
#define XSTUFF_H_DEFINED

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include <string>

typedef void ExposeFunc(void*);
typedef bool KeyFunc(void*, double xtime, char c);

class GlxWindow
{
public:
  GlxWindow(const char* winname, int width, int height, int depth);
  ~GlxWindow();

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  void swapBuffers();

  char getKeypress() const;

  void getInt(int& i) const;
  void getDouble(double& d) const;

  void eventLoop(void* cdata,
                 ExposeFunc* onExpose,
                 KeyFunc* onKey);

  bool getButtonPress(double& xtime, int& button_number);

private:
  std::string getWord() const;

  int itsWidth;
  int itsHeight;
  GLXContext itsGLXContext;
  Display* itsDisplay;
  Window itsWindow;
  bool itsUsingVsync;
};

static const char vcid_glxwindow_h[] = "$Header$";
#endif // !XSTUFF_H_DEFINED