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
  virtual ~GlxWindow();

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  char getKeypress() const;

  void getIntFromKeyboard(int& i) const;
  void getDoubleFromKeyboard(double& d) const;

  void getValueFromKeyboard(int& i) const { getIntFromKeyboard(i); }
  void getValueFromKeyboard(double& d) const { getDoubleFromKeyboard(d); }

  void eventLoop(void* cdata,
                 ExposeFunc* onExpose,
                 KeyFunc* onKey);

  bool getButtonPress(double& xtime, int& button_number);

protected:
  void swapBuffers();

private:
  std::string getWord() const;

  const int itsWidth;
  const int itsHeight;
  GLXContext itsGLXContext;
  Display* itsDisplay;
  Window itsWindow;
  bool itsUsingVsync;
};

static const char vcid_glxwindow_h[] = "$Header$";
#endif // !XSTUFF_H_DEFINED
