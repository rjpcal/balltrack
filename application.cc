///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:10:02 2000
// written: Wed Feb 23 14:36:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include <cstring>				  // for strcpy
#include <cstdlib>				  // for exit
#include <starbase.c.h>
#include <X11/keysym.h>

#include "application.h"
#include "applic.h"
#include "image.h"
#include "params.h"
#include "timing.h"

#include "defs.h"

#include "trace.h"
#include "debug.h"

#define WINDOW_NAME		     "tracking"

#define WINDOW_X		     1280
#define WINDOW_Y		     1024

#define MEANGREY                     127

#define DEPTH_HINT		     8
#define VISUAL_CLASS		     "PseudoColor"

static char *visual_class[]= {
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};

///////////////////////////////////////////////////////////////////////
//
// Application member definitions
//
///////////////////////////////////////////////////////////////////////

Application::Application(int argc, char** argv) {
DOTRACE("Application::Application");

  itsArgc = argc;
  itsArgv = argv;

  itsWidth  = WINDOW_X;
  itsHeight = WINDOW_Y;
  strcpy(itsName, WINDOW_NAME);

  openDisplay();
  createVisual();
  createColormap();
  createWindow();
  selectInput();
  mapImageWindow();
  windowInfo();

  WhoAreYou( this );
}

Application::~Application() {
DOTRACE("Application::~Application");
}

void Application::openDisplay() {
DOTRACE("Application::openDisplay");

  itsDisplay = XOpenDisplay( NULL );

  if ( itsDisplay == NULL ) 
    {
		if ( ( char * ) getenv( "DISPLAY" ) == ( char * ) NULL ) 
		  fprintf( stdout,"You need to set the DISPLAY env var\n" );
		else 
		  fprintf( stdout,"Cannot open DISPLAY %s,\n",getenv( "DISPLAY" ) );
		exit( -1 );
    }

  itsScreen = DefaultScreen( itsDisplay );
}

void Application::createVisual() {
DOTRACE("Application::createVisual");

  XVisualInfo vtemp;
  vtemp.screen = itsScreen;
  vtemp.depth	 = DEPTH_HINT;
    
  int vnumber;

  XVisualInfo* vlist = XGetVisualInfo( itsDisplay,
													VisualScreenMask|VisualDepthMask,
													&vtemp, &vnumber );
    
  for( int i=0; i<vnumber; i++ )
    {

		fprintf( stdout, " %s visual %d of depth %d mapsize %d\n", 
					visual_class[vlist[i].c_class], 
					vlist[i].visualid, 
					vlist[i].depth, 
					vlist[i].colormap_size );

		if( !strcmp( visual_class[vlist[i].c_class], VISUAL_CLASS ) )
		  {
			 itsVisual = vlist[i].visual;
			 itsDepth  = vlist[i].depth;
			 return;
		  }
    }

  itsVisual = vlist[0].visual;
  itsDepth  = vlist[0].depth;
}

void Application::createColormap() {
DOTRACE("Application::createColormap");

  itsColormap = XCreateColormap( itsDisplay, RootWindow( itsDisplay, itsScreen ),
											itsVisual, AllocNone );

  itsMeanColor.flags = DoRed | DoGreen | DoBlue;

  itsMeanColor.red = itsMeanColor.green = itsMeanColor.blue =
	 ( unsigned long )(257*MEANGREY);

  XAllocColor( itsDisplay, itsColormap, &itsMeanColor );
}

void Application::createWindow() {
DOTRACE("Application::createWindow");

  XSetWindowAttributes winAttributes;
  XSizeHints	hints;

  winAttributes.event_mask = ExposureMask;
  winAttributes.colormap   = itsColormap;
  winAttributes.background_pixel = itsMeanColor.pixel;
  winAttributes.border_pixel = itsMeanColor.pixel;

  itsWindow = XCreateWindow( itsDisplay, 
									  RootWindow( itsDisplay, itsScreen ),
									  0, 0, itsWidth, itsHeight, 2,
									  itsDepth, InputOutput, itsVisual,
									  ( CWBackPixel | CWColormap | CWBorderPixel | 
										 CWEventMask ),
									  &winAttributes );

  hints.flags = ( USSize | USPosition );
  hints.x = 0;
  hints.y = 0;
  hints.width	 = itsWidth;
  hints.height = itsHeight;

  XSetStandardProperties( itsDisplay, itsWindow, itsName, itsName,
								  None, itsArgv, itsArgc, &hints );
}

void Application::selectInput() {
DOTRACE("Application::selectInput");

  XSelectInput( itsDisplay, itsWindow, 
					 ExposureMask|StructureNotifyMask|
					 ButtonPressMask|KeyPressMask );

  XSync( itsDisplay, False );
}

void Application::mapImageWindow() {
DOTRACE("Application::mapImageWindow");

  char* device = ( char * ) make_X11_gopen_string( itsDisplay, itsWindow );
  itsFildes = gopen( device, OUTDEV, NULL, INT_XFORM|CMAP_FULL );

  gescape_arg arg1, arg2;

  arg1.i[0] = TRUE;
  gescape( itsFildes, READ_COLOR_MAP, &arg1, &arg2 );

  if ( itsFildes < 0 )
    {
		fprintf( stdout,"Could not gopen window.\n" );
		exit( -1 );
    }

  setWmProperty();
  setWmProtocol();

  XSetCommand( itsDisplay, itsWindow, itsArgv, itsArgc );

  XMapWindow( itsDisplay, itsWindow );
}

void Application::setWmProperty() {
DOTRACE("Application::setWmProperty");

  char *list[1];
  XSizeHints	  *size_hints;
  XClassHint	  *class_hint;
  XWMHints	  *wm_hints;
  XTextProperty window_name, icon_name;

  class_hint = XAllocClassHint(  );
  class_hint->res_name  = "test";
  class_hint->res_class = "Test";

  size_hints = XAllocSizeHints(  );
  size_hints->flags	  = USSize|PMinSize|PMaxSize;
  size_hints->min_width = itsWidth;
  size_hints->max_width = itsWidth;
  size_hints->min_height= itsHeight;
  size_hints->max_height= itsHeight;

  list[0] = itsName;
  XStringListToTextProperty( list, 1, &window_name );

  list[0] = itsName;
  XStringListToTextProperty( list, 1, &icon_name );

  wm_hints   = XAllocWMHints(	 );
  wm_hints->flags	  = InputHint;
  wm_hints->input	  = False;

  XSetWMProperties( itsDisplay, itsWindow, &window_name, &icon_name,
						  itsArgv, itsArgc, size_hints, wm_hints, class_hint );
}

void Application::setWmProtocol() {
DOTRACE("Application::setWmProtocol");

  Atom wm_protocols[2];

  wm_protocols[0] = XInternAtom( itsDisplay, "WM_DELETE_WINDOW", False );
  wm_protocols[1] = XInternAtom( itsDisplay, "WM_SAVE_YOURSELF", False );
  XSetWMProtocols( itsDisplay, itsWindow, wm_protocols, 2 );
}

void Application::windowInfo() {
DOTRACE("Application::windowInfo");

  int items;
  XVisualInfo info;

  info.visualid = itsVisual->visualid;
  info = *XGetVisualInfo( itsDisplay, VisualIDMask, &info, &items );
  fprintf( stdout, " Window %d with %s visual %d of depth %d\n", 
			  0, visual_class[info.c_class], info.visualid, info.depth );
  fflush( stdout );
}

void Application::wrapWindow() {
DOTRACE("Application::wrapWindow");

  gclose( itsFildes );
  XDestroyWindow( itsDisplay, itsWindow );
}

void Application::run() {
DOTRACE("Application::run");

  XEvent event;
    
  while( True )
    {

		XNextEvent( itsDisplay, &event );

		switch( event.type )
		  {
		  case Expose:
			 if ( event.xexpose.count == 0 )
				{
				  if( event.xexpose.window == itsWindow ) 
					 {
						ExposeWindow(this);
					 }
				}
			 break;

		  case ClientMessage:

			 if ( ( Atom )event.xclient.data.l[0] == 
					XInternAtom( itsDisplay, "WM_DELETE_WINDOW", False ) )
				{
				  return;
				}

			 else if ( ( Atom )event.xclient.data.l[0] == 
						  XInternAtom( itsDisplay, "WM_SAVE_YOURSELF", False ) )
				{
				  XSetCommand( itsDisplay, itsWindow, itsArgv, itsArgc );
				}
			 break;

		  case ButtonPress:
			 if ( event.xbutton.button == Button3 )
				{
				  return;
				}
			 else
				if ( event.xbutton.button == Button2 )
				  {
					 ExposeWindow(this);
				  }
			 break;

		  case KeyPress:
			 if ( event.xkey.window == itsWindow )
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

  wrapWindow();

  XCloseDisplay( itsDisplay );

  exit(code);
}

void Application::buttonPressLoop() {
DOTRACE("Application::buttonPressLoop");

  XEvent event;

  while( XCheckMaskEvent( itsDisplay, ButtonPressMask, &event ) ) {

	 if( event.type == ButtonPress && event.xbutton.window == itsWindow ) {
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

	 SwitchApplication( this, buffer[0] );
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

  Timing::addToResponseStack( this, (double) event->xbutton.time, nbutton );
}

char Application::getKeystroke() {
DOTRACE("Application::getKeystroke");

  while( true ) {
	 XEvent event;
	 XNextEvent( itsDisplay, &event );

	 if( event.type != KeyPress || event.xkey.window != itsWindow )
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
  XBell( itsDisplay, duration );
}


static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
