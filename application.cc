///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:10:02 2000
// written: Wed Feb 23 16:51:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include <cstring>				  // for strcpy
#include <cstdlib>				  // for exit
#include <starbase.c.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "applic.h"
#include "params.h"
#include "timing.h"

#include "defs.h"

#include "trace.h"
#include "debug.h"

const char* WINDOW_NAME = "tracking";

const int WINDOW_X = 1280;
const int WINDOW_Y = 1024;

const int MEANGREY = 127;

const int DEPTH_HINT = 8;

const char* VISUAL_CLASS = "PseudoColor";

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

struct Application::XStuff {
public:
  XStuff(int argc, char** argv, int width, int height, const char* name) :
	 itsArgc(argc),
	 itsArgv(argv),
	 itsWidth(width),
	 itsHeight(height)
  {
	 openDisplay();
	 createVisual();
	 createColormap();
	 createWindow(name);
	 selectInput();
  }

  void openDisplay();
  void createVisual();
  void createColormap();
  void createWindow(const char* name);
  void selectInput();
  void setWmProperty(char* name);
  void setWmProtocol();
  void mapWindow(const char* name);
  void printWindowInfo();
  void wrapX();

  Display* display() { return itsDisplay; }
  Window window() const { return itsWindow; }

private:
  int itsArgc;
  char** itsArgv;

  int itsWidth;
  int itsHeight;

  Display* itsDisplay;
  int itsScreen;
  Window itsWindow;
  Visual* itsVisual;
  int itsDepth;
  Colormap itsColormap;
  XColor itsMeanColor;
};

Application::Application(int argc, char** argv) :
  itsArgc(argc),
  itsArgv(argv),
  itsWidth(WINDOW_X),
  itsHeight(WINDOW_Y),
  itsFildes(0),
  itsXStuff(new XStuff(itsArgc, itsArgv, itsWidth, itsHeight, WINDOW_NAME))
{
DOTRACE("Application::Application");

  WhoAreYou( this );

  mapImageWindow();
  itsXStuff->printWindowInfo();

  // seed the random number generator based on the time
  struct timeval tp;
  Timing::getTime(&tp);
  srand48( tp.tv_sec );
}

Application::~Application() {
DOTRACE("Application::~Application");
  delete itsXStuff; 
}

void Application::mapImageWindow() {
DOTRACE("Application::mapImageWindow");

  char* device = ( char * ) make_X11_gopen_string( itsXStuff->display(),
																	itsXStuff->window() );
  itsFildes = gopen( device, OUTDEV, NULL, INT_XFORM|CMAP_FULL );

  gescape_arg arg1, arg2;

  arg1.i[0] = TRUE;
  gescape( itsFildes, READ_COLOR_MAP, &arg1, &arg2 );

  if ( itsFildes < 0 )
    {
		fprintf( stdout,"Could not gopen window.\n" );
		exit( -1 );
    }

  itsXStuff->mapWindow(WINDOW_NAME);
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

  gclose( itsFildes );

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

  Timing::addToResponseStack( this, (double) event->xbutton.time, nbutton );
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

///////////////////////////////////////////////////////////////////////
//
// Application::XStuff member definitions
//
///////////////////////////////////////////////////////////////////////

void Application::XStuff::openDisplay() {
DOTRACE("Application::XStuff::openDisplay");

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

void Application::XStuff::createVisual() {
DOTRACE("Application::XStuff::createVisual");

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

void Application::XStuff::createColormap() {
DOTRACE("Application::XStuff::createColormap");

  itsColormap = 
	 XCreateColormap( itsDisplay,
							RootWindow( itsDisplay, itsScreen ),
							itsVisual, AllocNone );

  itsMeanColor.flags = DoRed | DoGreen | DoBlue;

  itsMeanColor.red = itsMeanColor.green = itsMeanColor.blue =
	 ( unsigned long )(257*MEANGREY);

  XAllocColor( itsDisplay, itsColormap, &itsMeanColor );
}

void Application::XStuff::createWindow(const char* name) {
DOTRACE("Application::XStuff::createWindow");

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

  XSetStandardProperties( itsDisplay, itsWindow, name, name,
								  None, itsArgv, itsArgc, &hints );
}

void Application::XStuff::selectInput() {
DOTRACE("Application::XStuff::selectInput");

  XSelectInput( itsDisplay, itsWindow, 
					 ExposureMask|StructureNotifyMask|
					 ButtonPressMask|KeyPressMask );

  XSync( itsDisplay, False );
}

void Application::XStuff::setWmProperty(char* name) {
DOTRACE("Application::XStuff::setWmProperty");

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

  list[0] = name;
  XStringListToTextProperty( list, 1, &window_name );

  list[0] = name;
  XStringListToTextProperty( list, 1, &icon_name );

  wm_hints   = XAllocWMHints(	 );
  wm_hints->flags	  = InputHint;
  wm_hints->input	  = False;

  XSetWMProperties( itsDisplay, itsWindow,
						  &window_name, &icon_name,
						  itsArgv, itsArgc,
						  size_hints, wm_hints, class_hint );
}

void Application::XStuff::setWmProtocol() {
DOTRACE("Application::XStuff::setWmProtocol");

  Atom wm_protocols[2];

  wm_protocols[0] = XInternAtom( itsDisplay, "WM_DELETE_WINDOW", False );
  wm_protocols[1] = XInternAtom( itsDisplay, "WM_SAVE_YOURSELF", False );
  XSetWMProtocols( itsDisplay, itsWindow, wm_protocols, 2 );
}

void Application::XStuff::mapWindow(const char* name) {
DOTRACE("Application::XStuff::mapWindow");

  char temp_name[STRINGSIZE];
  strcpy(temp_name, name);

  setWmProperty(temp_name);
  setWmProtocol();

  XSetCommand( itsDisplay, itsWindow, itsArgv, itsArgc );

  XMapWindow( itsDisplay, itsWindow );
}

void Application::XStuff::printWindowInfo() {
DOTRACE("Application::XStuff::printWindowInfo");

  int items;
  XVisualInfo info;

  info.visualid = itsVisual->visualid;
  info = *XGetVisualInfo( itsDisplay, VisualIDMask, &info, &items );
  fprintf( stdout, " Window %d with %s visual %d of depth %d\n", 
			  0, visual_class[info.c_class], info.visualid, info.depth );
  fflush( stdout );
}

void Application::XStuff::wrapX() {
DOTRACE("Application::XStuff::wrapX");

  XDestroyWindow( itsDisplay, itsWindow );
  XCloseDisplay( itsDisplay );
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
