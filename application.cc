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

#include "defs.h"
#include "openglgfx.h"
#include "params.h"
#include "starbasegfx.h"
#include "timing.h"
#include "xhints.h"
#include "xstuff.h"

#include <cstdlib>              // for exit
#include <cstring>
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

Application::Application(const XHints& hints) :
  itsArgc(hints.argc()),
  itsArgv(hints.argv()),
  itsWidth(hints.width()),
  itsHeight(hints.height()),
#if defined(HP9000S700)
  itsGraphics(new StarbaseGfx(hints, itsWidth, itsHeight))
#elif defined(IRIX6) || defined(I686)
  itsGraphics(new OpenglGfx(hints, itsWidth, itsHeight))
#else
#  error No architecture macro.
#endif
{
DOTRACE("Application::Application");

  whoAreYou();

  itsGraphics->xstuff().openWindow(hints);
  itsGraphics->xstuff().mapWindow(hints.name());
  itsGraphics->xstuff().printWindowInfo();

  itsGraphics->initWindow();

  // seed the random number generator based on the time
  struct timeval tp;
  Timing::getTime(&tp);
  srand48( tp.tv_sec );
}

void Application::whoAreYou()
{
DOTRACE("Application::whoAreYou");
  if( argc() < 2 )
    {
      printf( " Who are you?\n" );
      quit(0);
    }

  strcpy( PROGRAM,  argv(0) );
  strcpy( OBSERVER, argv(1) );
  strcpy( FILENAME, argv(1) );
}

Application::~Application()
{
DOTRACE("Application::~Application");
  delete itsGraphics;
}

void Application::run()
{
DOTRACE("Application::run");

  XEvent event;

  while ( true )
    {
      XNextEvent( itsGraphics->xstuff().display(), &event );

      switch( event.type )
        {
        case Expose:
          if ( event.xexpose.count == 0 )
            if( event.xexpose.window == itsGraphics->xstuff().window() )
              onExpose();
          break;

        case ClientMessage:

          if ( ( Atom )event.xclient.data.l[0] ==
               XInternAtom( itsGraphics->xstuff().display(), "WM_DELETE_WINDOW", False ) )
            {
              return;
            }

          else if ( ( Atom )event.xclient.data.l[0] ==
                    XInternAtom( itsGraphics->xstuff().display(), "WM_SAVE_YOURSELF", False ) )
            {
              XSetCommand( itsGraphics->xstuff().display(), itsGraphics->xstuff().window(),
                           itsArgv, itsArgc );
            }
          break;

        case ButtonPress:
          if ( event.xbutton.button == Button3 )
            return;
          else
            onExpose();
          break;

        case KeyPress:
          if ( event.xkey.window == itsGraphics->xstuff().window() )
            {
              Timing::logTimer.set();
              Timing::mainTimer.set();

              keyPressAction( &event );
            }
          break;

        default:
          break;
        }
    }
}

void Application::quit(int code)
{
DOTRACE("Application::quit");

  wrap();

  itsGraphics->xstuff().wrapX();

  exit(code);
}

void Application::buttonPressLoop()
{
DOTRACE("Application::buttonPressLoop");

  XEvent event;

  while ( XCheckMaskEvent( itsGraphics->xstuff().display(),
                           ButtonPressMask | KeyPressMask,
                           &event ) )
    {
      if ( event.type == ButtonPress || event.type == KeyPress )
        {
          timeButtonEvent( &event );
        }
    }
}

void Application::keyPressAction( XEvent* event )
{
DOTRACE("Application::keyPressAction");

  char buffer[10];
  KeySym keysym;
  XComposeStatus compose;

  int count = XLookupString( (XKeyEvent*) event, buffer, 9,
                             &keysym, &compose );
  buffer[ count ] = '\0';

  if( count > 1 || keysym == XK_Return ||
      keysym == XK_BackSpace || keysym == XK_Delete )
    {
      ringBell(50);
      return;
    }

  if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
       keysym >= XK_space    && keysym <= XK_asciitilde )
    {
      struct timeval tp;
      struct timezone tzp;
      gettimeofday( &tp, &tzp );

      Timing::initTimeStack( (double) event->xkey.time, &tp );

      onMenuChoice( buffer[0] );
    }
}

void Application::timeButtonEvent( XEvent* event )
{
DOTRACE("Application::timeButtonEvent");

  int nbutton = 0;

  if (event->type == ButtonPress)
    {
      if( event->xbutton.button == Button1 )
        nbutton = LEFTBUTTON;
      else
        if( event->xbutton.button == Button2 )
          nbutton = MIDDLEBUTTON;
        else
          if( event->xbutton.button == Button3 )
            nbutton = RIGHTBUTTON;
          else
            nbutton = 0;

      Timing::addToResponseStack( (double) event->xbutton.time, nbutton );
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

      Timing::addToResponseStack( (long) event->xkey.subwindow /* sec */,
                                  (long) event->xkey.time /* usec */,
                                  nbutton );
    }
}

char Application::getKeystroke()
{
DOTRACE("Application::getKeystroke");

  while( true )
    {
      XEvent event;
      XNextEvent( itsGraphics->xstuff().display(), &event );

      if( event.type != KeyPress || event.xkey.window != itsGraphics->xstuff().window() )
        continue;

      KeySym keysym;
      char buffer[10];

      // For some reason, if we pass a non-null XComposeStatus* to this
      // function, the 'buffer' gets screwed up... very weird.
      int count = XLookupString( (XKeyEvent *) &event, &buffer[0], 9,
                                 &keysym, (XComposeStatus*) 0  );

      buffer[ count ] = '\0';

      DebugEvalNL(buffer);
      if( count > 1 || keysym == XK_Return ||
          keysym == XK_BackSpace || keysym == XK_Delete )
        {
          ringBell(50);
          continue;
        }

      if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
           keysym >= XK_space    && keysym <= XK_asciitilde )
        {
          DebugEvalNL(buffer[0]);
          return( buffer[0] );
        }
    }
}

void Application::ringBell(int duration)
{
DOTRACE("Application::ringBell");
  XBell( itsGraphics->xstuff().display(), duration );
}

void Application::wrap()
{
DOTRACE("Application::wrap");
}

void Application::onExpose()
{
DOTRACE("Application::onExpose");
}

void Application::onMenuChoice(char)
{
DOTRACE("Application::onMenuChoice");
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
