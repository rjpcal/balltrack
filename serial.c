#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <termios.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XHPlib.h>

#include "eventnames.h"

#define SERDEV "/dev/tty0p0"

#define MEANGREY                      127
#define STRINGSIZE                    128
#define DEPTH_HINT                    8
#define VISUAL_CLASS                  "PseudoColor"

static char *visual_class[]={
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};

Display *display;
int screen, depth, serial, width, height;
char name[STRINGSIZE];
Window window;
Visual *visual;
Colormap colormap;
XColor mean;
GC gc;

main( argc, argv )
          int argc;
          char **argv;
{
  width     = 50;
  height    = 50;
  strcpy( name, "SERIAL PORT" );

  OpenDisplay( &display, &screen );

  Create_visual( display, screen, &visual, &depth );

  Create_colormap( display, screen, visual, &colormap, &mean );

  Create_window( argc, argv, display, screen, visual, depth, colormap,
                                          mean, &window, width, height, name );

  XSelectInput( display, window, ExposureMask|KeyPressMask );

  XSync( display, False );

  Map_image_window( argc, argv, display, window, width, height,
                                                  name );

  EventLoop( argc, argv, display, screen, window );
}

#if 0
main( argc, argv )
int argc;
char **argv;
{
    char crctr;
    XEvent key_event, button_event;

    OpenDisplay( &display, &screen );

    button_event.xbutton.type = ButtonPressMask;
    button_event.xbutton.display = display;
    button_event.xbutton.send_event = False;

    key_event.xkey.type = KeyPressMask;
    key_event.xkey.display = display;
    key_event.xkey.send_event = False;

    OpenSerial();

    while( 1 )
      {
        ReadSerial( &crctr );

        printf( "%c\n", crctr );

        XSendEvent( display, InputFocus, False, KeyPressMask, &key_event );
        XSendEvent( display, PointerWindow, False, KeyPressMask, &key_event );
        XSendEvent( display, InputFocus, False, ButtonPressMask, &button_event );
        XSendEvent( display, PointerWindow, False, ButtonPressMask, &button_event );

      }

    CloseSerial();

}
#endif

OpenDisplay( display, screen )
          Display **display;
          int *screen;
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

Create_visual( display, screen, visual, depth )
          Display *display;
          int screen;
          Visual **visual;
          int *depth;
{
  int i, vnumber;
  XVisualInfo vtemp, *vlist;

  vtemp.screen = screen;
  vtemp.depth    = DEPTH_HINT;

  vlist = XGetVisualInfo( display, VisualScreenMask|VisualDepthMask,
                                                                  &vtemp, &vnumber );

  for( i=0; i<vnumber; i++ )
    {
      fprintf( stdout, " %s visual %d of depth %d mapsize %d\n",
               visual_class[vlist[i].class],
               vlist[i].visualid,
               vlist[i].depth,
               vlist[i].colormap_size );

      if( !strcmp( visual_class[vlist[i].class], VISUAL_CLASS ) )
        {
          *visual = vlist[i].visual;
          *depth  = vlist[i].depth;
          return;
        }
    }

  *visual = vlist[0].visual;
  *depth  = vlist[0].depth;
}

Create_colormap( display, screen, visual, colormap, mean )
          Display *display;
          int screen;
          Visual *visual;
          Colormap *colormap;
          XColor *mean;
{
  *colormap = XCreateColormap( display, RootWindow( display, screen ),
                               visual, AllocNone );

  mean->flags = DoRed | DoGreen | DoBlue;
  mean->red = mean->green = mean->blue = ( unsigned long )(257*MEANGREY);
  XAllocColor( display, *colormap, mean );
}

Create_window( argc, argv, display, screen, visual, depth, colormap, mean,
                                        window, width, height, name )
          int argc, depth, screen, width, height;
          char **argv, *name;
          Display *display;
          Visual *visual;
          Colormap colormap;
          XColor mean;
          Window *window;
{
  XSetWindowAttributes winAttributes;
  XSizeHints    hints;

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
  hints.width    = width;
  hints.height = height;
  XSetStandardProperties( display, *window, name, name,
                                                                  None, argv, argc, &hints );
}

Map_image_window( argc, argv, display, window, width, height, name )
          int argc, width, height;
          char **argv, *name;
          Display *display;
          Window window;
{
  char *device;

  device = ( char * ) make_X11_gopen_string( display, window );

  Set_wm_property( argc, argv, display, window, width, height, name );

  Set_wm_protocol( display, window );

  XSetCommand( display, window, argv, argc );

  XMapWindow( display, window );
}

Set_wm_property( argc, argv, display, window, width, height, name )
          int argc, width, height;
          char **argv, *name;
          Display *display;
          Window window;
{
  char *list[1];
  XSizeHints      *size_hints;
  XClassHint      *class_hint;
  XWMHints        *wm_hints;
  XTextProperty window_name, icon_name;

  class_hint = XAllocClassHint(  );
  class_hint->res_name  = "test";
  class_hint->res_class = "Test";

  size_hints = XAllocSizeHints(  );
  size_hints->flags       = USSize|PMinSize|PMaxSize;
  size_hints->min_width = width;
  size_hints->max_width = width;
  size_hints->min_height= height;
  size_hints->max_height= height;

  list[0] = name;
  XStringListToTextProperty( list, 1, &window_name );

  list[0] = name;
  XStringListToTextProperty( list, 1, &icon_name );

  wm_hints              = XAllocWMHints(         );
  wm_hints->flags         = InputHint;
  wm_hints->input         = False;

  XSetWMProperties( display, window, &window_name, &icon_name,
                                                  argv, argc, size_hints, wm_hints, class_hint );
}

Set_wm_protocol( display, window )
          Display *display;
          Window window;
{
  Atom wm_protocols[2];

  wm_protocols[0] = XInternAtom( display, "WM_DELETE_WINDOW", False );
  wm_protocols[1] = XInternAtom( display, "WM_SAVE_YOURSELF", False );
  XSetWMProtocols( display, window, wm_protocols, 2 );
}

EventLoop( argc, argv, display, screen, window )
          int argc;
          char **argv;
          Display *display;
          int screen;
          Window window;
{
  int n, stream;
  Window other_window;
  XEvent event, key_event;

  GetOtherWindow( &other_window );

  XNextEvent( display, &event );

  OpenSerial( &stream );

  while( 1 )
    {
      n = ReadSerial( stream );

      if( n >= 0 )
        {
          MakeKeyPressEvent( display, window, n, &key_event );

          XSendEvent( display, other_window, False, KeyPressMask, &key_event);

          XSync( display, False );
        }
    }

  CloseSerial();
}

GetOtherWindow( other_window )
          Window *other_window;
{
  FILE *fp;

  if( ( fp = fopen( "wdwptr", "r" ) ) == NULL )
    {
      printf( " cannot open file\n" );
      exit();
    }

  fscanf( fp, " %ld", other_window );
  printf( " window read %ld\n", *other_window );

  fclose( fp );
}

MakeKeyPressEvent( display, window, keynumber, event )
          Display *display;
          Window window;
          XEvent *event;
          int keynumber;
{
  struct timeval  tp;
  struct timezone tzp;
  long   sec_timer, usec_timer;

  gettimeofday( &tp, &tzp );
  sec_timer = (long) tp.tv_sec;
  usec_timer = (long) tp.tv_usec;

  event->type            = KeyPress;
  event->xkey.send_event = True;
  event->xkey.display    = display;
  event->xkey.window     = window;
  event->xkey.subwindow  = sec_timer;
  event->xkey.time       = usec_timer;
  event->xkey.keycode    = keynumber;
}

OpenSerial( stream )
          int *stream;
{
  struct termios ti;

  if( ( *stream = open(SERDEV, O_RDONLY) ) == -1 )
    {
      fprintf(stderr, "cannot open %s\n", SERDEV);
      exit(0);
    }

  tcgetattr( *stream, &ti );

  ti.c_iflag = IGNBRK | IGNPAR;
  ti.c_oflag = 0x0;
  ti.c_cflag = CS8 | CLOCAL | B9600 | CREAD;
  ti.c_lflag = 0;
  ti.c_cc[VTIME] = 0;
  ti.c_cc[VMIN]  = 1;

  tcsetattr( *stream, TCSANOW, &ti);
}

int ReadSerial( stream )
          int stream;
{
  int i, n;
  char c[STRINGSIZE];

  n = read( stream, c, STRINGSIZE );

  for( i=0; i<n; i++ )
    {
      if( 65 <= c[i] && c[i] <= 72 )
        {
          printf("c = %d\n", (int)c[i]);
          return( c[i] );
        }
    }

  return( -1 );
}

CloseSerial( stream )
          int stream;
{
  close(stream);
}
