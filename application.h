///////////////////////////////////////////////////////////////////////
//
// application.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:06:46 2000
// written: Wed Sep  3 16:05:30 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H_DEFINED
#define APPLICATION_H_DEFINED

#ifndef _XLIB_H_
typedef union _XEvent XEvent;
#endif

class XStuff;
class Graphics;

class Application
{
private:
  Application(const Application&);
  Application& operator=(const Application&);

public:
  Application(Graphics& gfx);
  virtual ~Application();

  void run();
  void quit(int code);

  void buttonPressLoop();

  char getKeystroke();

  Graphics& graphics() const { return itsGraphics; }

protected:
  virtual void wrap() = 0;
  virtual void onExpose() = 0;
  virtual void onKey(char c) = 0;

private:
  void keyPressAction( XEvent* event );

  void timeButtonEvent( XEvent* event );

  Graphics& itsGraphics;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
