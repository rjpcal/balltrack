///////////////////////////////////////////////////////////////////////
//
// application.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:06:46 2000
// written: Wed Feb 23 15:54:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "defs.h"

class Application {
public:
  Application(int argc, char** argv);
  virtual ~Application();

  void run();
  void quit(int code);
  
  void buttonPressLoop();

  char getKeystroke();

  void ringBell(int duration);

  int argc() const { return itsArgc; }
  char* argv(int argn) const { return itsArgv[argn]; }

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  int fildes() const { return itsFildes; }

protected:
  virtual void wrap();
  virtual void onExpose();
  virtual void onMenuChoice(char c);

private:
  void openDisplay();
  void createVisual();
  void createColormap();
  void createWindow();
  void selectInput();
  void mapImageWindow();
  void setWmProperty();
  void setWmProtocol();
  void windowInfo();
  void wrapWindow();

  void keyPressAction( XEvent* event );

  void timeButtonEvent( XEvent* event );

  int itsArgc;
  char** itsArgv;

  Display* itsDisplay;
  int itsScreen;
  int itsFildes;
  int itsDepth;
  int itsWidth;
  int itsHeight;
  char itsName[STRINGSIZE];
  Window itsWindow;
  Visual* itsVisual;
  Colormap itsColormap;
  XColor itsMeanColor;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
