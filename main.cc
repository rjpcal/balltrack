#include "main.h"

#include <cstring>				  // for strcpy
#include <cstdlib>				  // for exit
#include <starbase.c.h>
#include <X11/keysym.h>

#include "applic.h"
#include "image.h"
#include "params.h"
#include "timing.h"

#include "defs.h"

#define WINDOW_NAME		     "tracking"

#define WINDOW_X		     1280
#define WINDOW_Y		     1024

#define MEANGREY                     127

#define DEPTH_HINT		     8
#define VISUAL_CLASS		     "PseudoColor"

static char *visual_class[]={
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};

Display *display;
int screen, fildes, depth, width, height;
char name[STRINGSIZE];
Window window;
Visual *visual;
Colormap colormap;
XColor mean;
GC gc;

int main( int argc, char** argv )
{
  width  = WINDOW_X;
  height = WINDOW_Y;
  strcpy( name, WINDOW_NAME);

  WhoAreYou( argc, argv );

  OpenDisplay( &display, &screen );

  Create_visual( display, screen, &visual, &depth );

  Create_colormap( display, screen, visual, &colormap, &mean );

  Create_window( argc, argv, display, screen, visual, depth, colormap, 
					  mean, &window, width, height, name );

  XSelectInput( display, window, 
					 ExposureMask|StructureNotifyMask|
					 ButtonPressMask|KeyPressMask );

  XSync( display, False );

  fildes = Map_image_window( argc, argv, display, window, width, height,
									  name );

  Window_info( display, visual );

  InitWindow( width, height, fildes );

  InitApplication();

  Handle_events( argc, argv, display, screen, window, fildes );

  Exit(0);
}

void Exit(int code)
{
  WrapApplication();

  WrapWindow( display, window, fildes );

  XCloseDisplay( display );

  exit(code);
}

void OpenDisplay( Display** display, int* screen )
{

  *display = XOpenDisplay( NULL );

  if ( *display == NULL ) 
    {
		if ( ( char * ) getenv( "DISPLAY" ) == ( char * ) NULL ) 
		  fprintf( stdout,"You need to set the DISPLAY env var\n" );
		else 
		  fprintf( stdout,"Cannot open DISPLAY %s,\n",getenv( "DISPLAY" ) );
		exit( -1 );
    }

  *screen = DefaultScreen( *display );
}

void Create_visual( Display* display, int screen, Visual** visual, int* depth ) 
{
  int i, vnumber;
  XVisualInfo vtemp, *vlist;

  vtemp.screen = screen;
  vtemp.depth	 = DEPTH_HINT;
    
  vlist = XGetVisualInfo( display, VisualScreenMask|VisualDepthMask,
								  &vtemp, &vnumber );
    
  for( i=0; i<vnumber; i++ )
    {

		fprintf( stdout, " %s visual %d of depth %d mapsize %d\n", 
					visual_class[vlist[i].c_class], 
					vlist[i].visualid, 
					vlist[i].depth, 
					vlist[i].colormap_size );

		if( !strcmp( visual_class[vlist[i].c_class], VISUAL_CLASS ) )
		  {
			 *visual = vlist[i].visual;
			 *depth  = vlist[i].depth;
			 return;
		  }
    }

  *visual = vlist[0].visual;
  *depth  = vlist[0].depth;
}

void Create_colormap( Display* display, int screen, Visual* visual,
							 Colormap* colormap, XColor* mean )
{
  *colormap = XCreateColormap( display, RootWindow( display, screen ),
										 visual, AllocNone );

  mean->flags = DoRed | DoGreen | DoBlue;
  mean->red = mean->green = mean->blue = ( unsigned long )(257*MEANGREY);
  XAllocColor( display, *colormap, mean );
}

void Create_window( int argc, char** argv, Display* display, int screen,
						  Visual* visual, int depth, Colormap colormap, XColor mean, 
						  Window* window, int width, int height, char* name )
{
  XSetWindowAttributes winAttributes;
  XSizeHints	hints;

  winAttributes.event_mask = ExposureMask;
  winAttributes.colormap   = colormap;
  winAttributes.background_pixel = mean.pixel;
  winAttributes.border_pixel = mean.pixel;

  *window = XCreateWindow( display, 
									RootWindow( display, screen ),
									0, 0, width, height, 2,
									depth, InputOutput, visual,
									( CWBackPixel | CWColormap | CWBorderPixel | 
									  CWEventMask ),
									&winAttributes );

  hints.flags = ( USSize | USPosition );
  hints.x = 0;
  hints.y = 0;
  hints.width	 = width;
  hints.height = height;
  XSetStandardProperties( display, *window, name, name,
								  None, argv, argc, &hints );
}

int Map_image_window( int argc, char** argv, Display* display,
							 Window window, int width, int height, char* name )
{
  int fildes = 0;
  char *device;
  gescape_arg arg1, arg2;

  device = ( char * ) make_X11_gopen_string( display, window );
  fildes = gopen( device, OUTDEV, NULL, INT_XFORM|CMAP_FULL );

  arg1.i[0] = TRUE;
  gescape( fildes, READ_COLOR_MAP, &arg1, &arg2 );

  if ( fildes < 0 )
    {
		fprintf( stdout,"Could not gopen window.\n" );
		exit( -1 );
    }

  Set_wm_property( argc, argv, display, window, width, height, name );

  Set_wm_protocol( display, window );

  XSetCommand( display, window, argv, argc );

  XMapWindow( display, window );

  return( fildes );
}

void Set_wm_property( int argc, char** argv, Display* display, Window window,
							 int width, int height, char* name )
{
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
  size_hints->min_width = width;
  size_hints->max_width = width;
  size_hints->min_height= height;
  size_hints->max_height= height;

  list[0] = name;
  XStringListToTextProperty( list, 1, &window_name );

  list[0] = name;
  XStringListToTextProperty( list, 1, &icon_name );

  wm_hints   = XAllocWMHints(	 );
  wm_hints->flags	  = InputHint;
  wm_hints->input	  = False;

  XSetWMProperties( display, window, &window_name, &icon_name,
						  argv, argc, size_hints, wm_hints, class_hint );
}

void Set_wm_protocol( Display* display, Window window )
{
  Atom wm_protocols[2];

  wm_protocols[0] = XInternAtom( display, "WM_DELETE_WINDOW", False );
  wm_protocols[1] = XInternAtom( display, "WM_SAVE_YOURSELF", False );
  XSetWMProtocols( display, window, wm_protocols, 2 );
}

void Window_info( Display* display, Visual* visual )
{
  int items;
  XVisualInfo info;

  info.visualid = visual->visualid;
  info = *XGetVisualInfo( display, VisualIDMask, &info, &items );
  fprintf( stdout, " Window %d with %s visual %d of depth %d\n", 
			  0, visual_class[info.c_class], info.visualid, info.depth );
  fflush( stdout );
}

void Handle_events( int argc, char** argv, Display* display,
						  int screen, Window window, int fildes )
{
  XEvent event;
    
  while( True )
    {

		XNextEvent( display, &event );

		switch( event.type )
		  {
		  case Expose:
			 if ( event.xexpose.count == 0 )
				{
				  if( event.xexpose.window == window ) 
					 {
						ExposeWindow();
					 }
				}
			 break;

		  case ClientMessage:

			 if ( ( Atom )event.xclient.data.l[0] == 
					XInternAtom( display, "WM_DELETE_WINDOW", False ) )
				{
				  return;
				}

			 else if ( ( Atom )event.xclient.data.l[0] == 
						  XInternAtom( display, "WM_SAVE_YOURSELF", False ) )
				{
				  XSetCommand( display, window, argv, argc );
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
					 ExposeWindow();
				  }
			 break;

		  case KeyPress:
			 if ( event.xkey.window == window )
				{
				  SetLogTimer();
				  SetTimer();

				  KeyPressAction( &event );
				}
			 break;

		  default:
			 break;
		  }
    }
} 

void KeyPressAction( XEvent* event )
{
  int count;
  KeySym keysym;
  XComposeStatus compose;
  char buffer[10];
  struct timeval tp;
  struct timezone tzp;

  count = XLookupString( (XKeyEvent*) event, buffer, 10, &keysym, &compose );
  buffer[ count ] = '\0';

  if( count > 1 ||
		keysym == XK_Return || keysym == XK_BackSpace || keysym == XK_Delete )
    {
		XBell( display, 50 );
    }
  else
    if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
			keysym >= XK_space    && keysym <= XK_asciitilde )
		{
        gettimeofday( &tp, &tzp );

        InitTimeStack( (double) event->xkey.time, &tp );

        SwitchApplication( buffer[0] );
		}
}

void WrapWindow( Display* display, Window window, int fildes )
{
  gclose( fildes );
  XDestroyWindow( display, window );
}

char GetKeystroke()
{
  int count;
  KeySym keysym;
  XComposeStatus compose;
  char buffer[10];
  XEvent event;
    
  while( True )
    {
		XNextEvent( display, &event );

		if( event.type==KeyPress && event.xkey.window == window )
		  {
			 count = XLookupString( (XKeyEvent *) &event, buffer, 10, 
											&keysym, &compose );
			 buffer[ count ] = '\0';

			 if( count > 1 || keysym == XK_Return || 
				  keysym == XK_BackSpace || keysym == XK_Delete )
            {
				  XBell( display, 50 );
            }
			 else
            if ( keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
					  keysym >= XK_space    && keysym <= XK_asciitilde )
				  {
                return( buffer[0] );
				  }
        }
    }
}

void SmallLoop()
{
  XEvent event;

  while( XCheckMaskEvent( display, ButtonPressMask, &event ) )
    {

		if( event.type == ButtonPress &&
			 event.xbutton.window == window )
		  {
			 TimeButtonEvent( &event );
        }
    }
} 

void TimeButtonEvent( XEvent* event )
{
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

  AddToResponseStack( (double) event->xbutton.time, nbutton );
}

