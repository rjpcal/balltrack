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
#include "graphics.h"

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

Application::Application(Graphics& x, void* cdata,
                         ExposeFunc* e, KeyFunc* k) :
  itsGraphics(x),
  itsCdata(cdata),
  itsOnExpose(e),
  itsOnKey(k)
{
DOTRACE("Application::Application");

  // seed the random number generator based on the time
  struct timeval tp;
  Timing::getTime(&tp);
  srand48(tp.tv_sec);
}

Application::~Application()
{
DOTRACE("Application::~Application");
}

void Application::run()
{
DOTRACE("Application::run");

  XEvent event;

  while (true)
    {
      XNextEvent(itsGraphics.display(), &event);

      switch (event.type)
        {
        case Expose:
          if (event.xexpose.count == 0)
            if (event.xexpose.window == itsGraphics.window())
              (*itsOnExpose)(itsCdata);
          break;

        case ButtonPress:
          if (event.xbutton.button == Button3)
            return;
          else
            (*itsOnExpose)(itsCdata);
          break;

        case KeyPress:
          if (event.xkey.window == itsGraphics.window())
            {
              Timing::logTimer.set();
              Timing::mainTimer.set();

              char key = keyPressAction(&event);
              if ((*itsOnKey)(itsCdata, key) == true)
                return;
            }
          break;

        default:
          break;
        }
    }
}

void Application::buttonPressLoop()
{
DOTRACE("Application::buttonPressLoop");

  XEvent event;

  while (XCheckMaskEvent(itsGraphics.display(),
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

  return itsGraphics.getKeypress();
}

// void Application::exposeCallback(void* cdata)
// {
// DOTRACE("Application::exposeCallback");
//   Application* p = static_cast<Application*>(cdata);
//   p->onExpose();
// }

// bool Application::keyCallback(void* cdata, char c)
// {
// DOTRACE("Application::keyCallback");
//   Application* p = static_cast<Application*>(cdata);
//   return p->onKey(c);
// }

char Application::keyPressAction(XEvent* event)
{
DOTRACE("Application::keyPressAction");

  char buffer[10];
  KeySym keysym;
  XComposeStatus compose;

  int count = XLookupString((XKeyEvent*) event, buffer, 9,
                            &keysym, &compose);
  buffer[ count ] = '\0';

  if (count > 1 || keysym == XK_Return ||
      keysym == XK_BackSpace || keysym == XK_Delete)
    {
      return '\0';
    }

  if (keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
      keysym >= XK_space    && keysym <= XK_asciitilde)
    {
      struct timeval tp;
      struct timezone tzp;
      gettimeofday(&tp, &tzp);

      Timing::initTimeStack(double(event->xkey.time), &tp);

      return buffer[0];
    }

  return '\0';
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
