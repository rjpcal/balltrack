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

class Application
{
private:
  Application(const Application&);
  Application& operator=(const Application&);

public:

  Application(XStuff& x);
  virtual ~Application();

  void buttonPressLoop();

  char getKeystroke();

private:
  static void timeButtonEvent(XEvent* event);

  XStuff& itsXStuff;
};

static const char vcid_application_h[] = "$Header$";
#endif // !APPLICATION_H_DEFINED
