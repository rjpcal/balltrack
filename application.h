///////////////////////////////////////////////////////////////////////
//
// application.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:06:46 2000
// written: Wed Feb 23 16:44:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

#ifndef _XLIB_H_
typedef union _XEvent XEvent;
#endif

class Application {
private:
  Application(const Application&);
  Application& operator=(const Application&);

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
  void mapImageWindow();

  void keyPressAction( XEvent* event );

  void timeButtonEvent( XEvent* event );

  int itsArgc;
  char** itsArgv;

  int itsWidth;
  int itsHeight;

  int itsFildes;

  struct XStuff;
  XStuff* itsXStuff;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
