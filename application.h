///////////////////////////////////////////////////////////////////////
//
// application.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:06:46 2000
// written: Wed Sep  3 14:00:28 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

#ifndef _XLIB_H_
typedef union _XEvent XEvent;
#endif

class XHints;

class XStuff;

class Graphics;

class Application
{
private:
  Application(const Application&);
  Application& operator=(const Application&);

public:
  Application(const XHints& hints);
  virtual ~Application();

  void run();
  void quit(int code);

  void buttonPressLoop();

  char getKeystroke();

  void ringBell(int duration);

  int argc() const { return itsArgc; }
  char* argv(int argn) const { return itsArgv[argn]; }

  Graphics* graphics() const { return itsGraphics; }

protected:
  virtual void wrap();
  virtual void onExpose();
  virtual void onMenuChoice(char c);

private:
  void whoAreYou();

  void keyPressAction( XEvent* event );

  void timeButtonEvent( XEvent* event );

  int itsArgc;
  char** itsArgv;

  int itsWidth;
  int itsHeight;

  XStuff* itsXStuff;

  Graphics* itsGraphics;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
