///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:10:02 2000
// written: Mon Feb 28 19:15:36 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include <cstdlib>				  // for exit
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "applic.h"
#include "starbasegfx.h"
#include "openglgfx.h"
#include "params.h"
#include "timing.h"
#include "xhints.h"
#include "xstuff.h"

#include "defs.h"

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
  itsXStuff(new XStuff(hints)),
#if defined(HP9000S700)
  itsGraphics(new StarbaseGfx(itsXStuff, itsWidth, itsHeight))
#elif defined(IRIX6)
  itsGraphics(new OpenglGfx(itsXStuff, hints, itsWidth, itsHeight))
#else
#  error No architecture macro.
#endif
{
DOTRACE("Application::Application");

  WhoAreYou( this );

  itsXStuff->openWindow(hints);
  itsXStuff->mapWindow(hints.name());
  itsXStuff->printWindowInfo();

  itsGraphics->makeCurrent();

  // seed the random number generator based on the time
  struct timeval tp;
  Timing::getTime(&tp);
  srand48( tp.tv_sec );
}

Application::~Application() {
DOTRACE("Application::~Application");
  delete itsXStuff;
  delete itsGraphics;
}

void Application::run() {
DOTRACE("Application::run");

  XEvent event;
    
  while( True )
    {

		XNextEvent( itsXStuff->display(), &event );

		switch( event.type )
		  {
		  case Expose:
			 if ( event.xexpose.count == 0 )
				if( event.xexpose.window == itsXStuff->window() )
				  onExpose();
			 break;

		  case ClientMessage:

			 if ( ( Atom )event.xclient.data.l[0] == 
					XInternAtom( itsXStuff->display(), "WM_DELETE_WINDOW", False ) )
				{
				  return;
				}

			 else if ( ( Atom )event.xclient.data.l[0] == 
						  XInternAtom( itsXStuff->display(), "WM_SAVE_YOURSELF", False ) )
				{
				  XSetCommand( itsXStuff->display(), itsXStuff->window(),
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
			 if ( event.xkey.window == itsXStuff->window() )
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

void Application::quit(int code) {
DOTRACE("Application::quit");

  wrap(); 

  itsXStuff->wrapX();

  exit(code);
}

void Application::buttonPressLoop() {
DOTRACE("Application::buttonPressLoop");

  XEvent event;

  while( XCheckMaskEvent( itsXStuff->display(), ButtonPressMask, &event ) ) {

	 if( event.type == ButtonPress &&
		  event.xbutton.window == itsXStuff->window() ) {
		timeButtonEvent( &event );
	 }
  }
} 

void Application::keyPressAction( XEvent* event ) {
DOTRACE("Application::keyPressAction");

  char buffer[10];
  KeySym keysym;
  XComposeStatus compose;

  int count = XLookupString( (XKeyEvent*) event, buffer, 9,
									  &keysym, &compose );
  buffer[ count ] = '\0';

  if( count > 1 || keysym == XK_Return ||
		keysym == XK_BackSpace || keysym == XK_Delete ) {

		ringBell(50);
		return;
  }

  if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
		 keysym >= XK_space    && keysym <= XK_asciitilde ) {

	 struct timeval tp;
	 struct timezone tzp;
	 gettimeofday( &tp, &tzp );

	 Timing::initTimeStack( (double) event->xkey.time, &tp );

	 onMenuChoice( buffer[0] );
  }
}

void Application::timeButtonEvent( XEvent* event ) {
DOTRACE("Application::timeButtonEvent");

  int nbutton;    

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

char Application::getKeystroke() {
DOTRACE("Application::getKeystroke");

  while( true ) {
	 XEvent event;
	 XNextEvent( itsXStuff->display(), &event );

	 if( event.type != KeyPress || event.xkey.window != itsXStuff->window() )
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
		  keysym == XK_BackSpace || keysym == XK_Delete ) {

		ringBell(50);
		continue;
	 }

	 if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
			keysym >= XK_space    && keysym <= XK_asciitilde ) {

  		DebugEvalNL(buffer[0]);
		return( buffer[0] );
	 }
  }

}

void Application::ringBell(int duration) {
DOTRACE("Application::ringBell");
  XBell( itsXStuff->display(), duration );
}

void Application::wrap() {
DOTRACE("Application::wrap");
}

void Application::onExpose() {
DOTRACE("Application::onExpose");
}

void Application::onMenuChoice(char) {
DOTRACE("Application::onMenuChoice");
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
