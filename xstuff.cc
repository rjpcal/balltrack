///////////////////////////////////////////////////////////////////////
//
// xstuff.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:55 2000
// written: Wed Sep  3 16:23:15 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_CC_DEFINED
#define XSTUFF_CC_DEFINED

#include "xstuff.h"

#include "defs.h"

#include <cstring>              // for strncpy
#include <cstdlib>              // for getenv
#include <cstdio>

#include "trace.h"
#include "debug.h"

namespace
{
  const char* visual_class_name(int c_class)
  {
    switch (c_class)
      {
#define ONE_CASE(x) case x: return #x
        ONE_CASE(StaticGray);
        ONE_CASE(GrayScale);
        ONE_CASE(StaticColor);
        ONE_CASE(PseudoColor);
        ONE_CASE(TrueColor);
        ONE_CASE(DirectColor);
#undef ONE_CASE
      }
    fprintf(stderr, "unknown visual class %d\n", c_class);
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////
//
// XStuff member definitions
//
///////////////////////////////////////////////////////////////////////

XStuff::XStuff(int width, int height) :
  itsWidth(width),
  itsHeight(height),
  itsDisplay(0),
  itsWindow(0),
  itsVisInfo()
{
DOTRACE("XStuff::XStuff");

  itsDisplay = XOpenDisplay(0);

  if (itsDisplay == 0)
    {
      if ( getenv( "DISPLAY" ) == 0 )
        fprintf( stdout, "You need to set the DISPLAY env var\n" );
      else
        fprintf( stdout, "Cannot open DISPLAY %s,\n", getenv( "DISPLAY" ) );
      exit(1);
    }

}

XStuff::~XStuff()
{
DOTRACE("XStuff::~XStuff");
  XDestroyWindow( itsDisplay, itsWindow );
  XCloseDisplay( itsDisplay );
}

void XStuff::openWindow(const char* winname,
                        const XVisualInfo* vinfo,
                        int width, int height, int depth)
{
DOTRACE("XStuff::openWindow");

  if (vinfo == 0)
    {
      fprintf(stderr, "XVisualInfo* was null in openWindow()!\n");
      exit(1);
    }

  itsVisInfo = *vinfo;

  const int screen = DefaultScreen( itsDisplay );

  Colormap cmap =
    XCreateColormap( itsDisplay,
                     RootWindow( itsDisplay, screen ),
                     itsVisInfo.visual, AllocNone );

  XColor mean_color;
  mean_color.flags = DoRed | DoGreen | DoBlue;

  const int MEANGREY = 127;

  mean_color.red = mean_color.green = mean_color.blue =
    ( unsigned long )(257*MEANGREY);

  XAllocColor( itsDisplay, cmap, &mean_color );


  XSetWindowAttributes winAttributes;

  winAttributes.event_mask = ExposureMask;
  winAttributes.colormap  = cmap;
  winAttributes.background_pixel = mean_color.pixel;
  winAttributes.border_pixel = mean_color.pixel;

  itsWindow = XCreateWindow( itsDisplay,
                             RootWindow(itsDisplay, screen),
                             0, 0, itsWidth, itsHeight, 2,
                             itsVisInfo.depth,
                             InputOutput,
                             itsVisInfo.visual,
                             ( CWBackPixel | CWColormap
                               | CWBorderPixel | CWEventMask ),
                             &winAttributes );

  XSelectInput( itsDisplay, itsWindow,
                ExposureMask|StructureNotifyMask|
                ButtonPressMask|KeyPressMask );

  XSync( itsDisplay, False );


  fprintf( stdout, " window with %s visual %d of depth %d\n",
           visual_class_name(itsVisInfo.c_class),
           itsVisInfo.visualid,
           itsVisInfo.depth );

  fflush( stdout );

  char temp_name[256];
  strncpy(temp_name, winname, 256);
  char* nameptr = &temp_name[0];

  XTextProperty window_name;
  XStringListToTextProperty( &nameptr, 1, &window_name );

  XSetWMName(itsDisplay, itsWindow, &window_name);

  XMapWindow( itsDisplay, itsWindow );
}

static const char vcid_xstuff_cc[] = "$Header$";
#endif // !XSTUFF_CC_DEFINED
