///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:10:02 2000
// written: Tue Feb  3 16:56:40 2004
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "timing.h"
#include "xstuff.h"

#include <cstdlib>              // for srand48()
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Application member definitions
//
///////////////////////////////////////////////////////////////////////

Application::Application(XStuff& x) :
  itsXStuff(x)
{
DOTRACE("Application::Application");
}

Application::~Application()
{
DOTRACE("Application::~Application");
}

void Application::buttonPressLoop()
{
DOTRACE("Application::buttonPressLoop");

  XEvent event;

  while (XCheckMaskEvent(itsXStuff.display(),
                         ButtonPressMask | KeyPressMask,
                         &event))
    {
      if (event.type == ButtonPress || event.type == KeyPress)
        {
          timeButtonEvent(&event);
        }
    }
}

char Application::getKeystroke()
{
DOTRACE("Application::getKeystroke");

  return itsXStuff.getKeypress();
}

void Application::timeButtonEvent(XEvent* event)
{
DOTRACE("Application::timeButtonEvent");

  int nbutton = 0;

  if (event->type == ButtonPress)
    {
      if (event->xbutton.button == Button1)
        nbutton = LEFTBUTTON;
      else
        if (event->xbutton.button == Button2)
          nbutton = MIDDLEBUTTON;
        else
          if (event->xbutton.button == Button3)
            nbutton = RIGHTBUTTON;
          else
            nbutton = 0;

      Timing::addToResponseStack(double(event->xbutton.time), nbutton);
    }
  else if (event->type == KeyPress)
    {
      int keycode = event->xkey.keycode;

      DebugEvalNL(keycode);

      switch (keycode)
        {
        case 'a': case 'b': case 'c': case 'd':
        case 'A': case 'B': case 'C': case 'D':
          nbutton = LEFTBUTTON;
          break;
        case 'e': case 'f': case 'g': case 'h':
        case 'E': case 'F': case 'G': case 'H':
          nbutton = MIDDLEBUTTON;
          break;
        default:
          nbutton = 0;
          break;
        }

      Timing::addToResponseStack(long(event->xkey.subwindow) /* sec */,
                                 long(event->xkey.time) /* usec */,
                                 nbutton);
    }
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
