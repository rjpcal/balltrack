///////////////////////////////////////////////////////////////////////
//
// xstuff.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:55 2000
// written: Wed Mar  1 15:42:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_CC_DEFINED
#define XSTUFF_CC_DEFINED

#include "xstuff.h"

#include <cstring>				  // for strcpy
#include <cstdlib>				  // for getenv
#include <cstdio>
#include <X11/Xutil.h>

#include "defs.h"
#include "xhints.h"

#include "trace.h"
#include "debug.h"

class VisualClass {
public:
  VisualClass(int class_id) : itsId(class_id), itsName("")
	 {
		for (int i = 0; i < 6; ++i)
		  if ( ids[i] == itsId ) {
			 itsName = names[i];
			 break;
		  }
	 }

  bool matches(int class_id)
	 { return (itsId == class_id); }

  bool matches(const char* class_name)
	 { return (strcmp(itsName, class_name) == 0); }

  const char* name() { return itsName; }
  int id() { return itsId; }

private:
  int itsId;
  const char* itsName;

  static const int ids[];
  static const char* const names[];
};

const int VisualClass::ids[] = {
  StaticGray,
  GrayScale,
  StaticColor,
  PseudoColor,
  TrueColor,
  DirectColor,
};

const char* const VisualClass::names[] = {
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};

///////////////////////////////////////////////////////////////////////
//
// XStuff member definitions
//
///////////////////////////////////////////////////////////////////////

XStuff::XStuff(const XHints& hints) :
  itsArgc(hints.argc()),
  itsArgv(hints.argv()),
  itsWidth(hints.width()),
  itsHeight(hints.height()),
  itsHasPrefVisInfo(false)
{
DOTRACE("XStuff::XStuff");
  openDisplay();
}

void XStuff::setPrefVisInfo(const XVisualInfo* vinfo) {
DOTRACE("XStuff::setPrefVisInfo");
  if (vinfo != 0) {
	 itsPrefVisInfo = *vinfo;
	 itsHasPrefVisInfo = true;
  }
}

void XStuff::openWindow(const XHints& hints) {
DOTRACE("XStuff::openWindow");
  createVisual(hints);
  createColormap(hints);
  createWindow(hints.name());
  selectInput();
}

void XStuff::openDisplay() {
DOTRACE("XStuff::openDisplay");

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

void XStuff::createVisual(const XHints& hints) {
DOTRACE("XStuff::createVisual");

  if ( !itsHasPrefVisInfo ) {
	 XVisualInfo vtemp;
	 vtemp.screen = itsScreen;
	 vtemp.depth	 = hints.depth();

	 int vnumber;

	 XVisualInfo* vlist = XGetVisualInfo( itsDisplay,
													  VisualScreenMask|VisualDepthMask,
													  &vtemp, &vnumber );
    
	 itsVisInfo = vlist[0];

	 for( int i=0; i<vnumber; i++ ) {

		VisualClass vclass(vlist[i].c_class);

		fprintf( stdout, " %s visual %d of depth %d mapsize %d\n", 
					vclass.name(),
					vlist[i].visualid, 
					vlist[i].depth, 
					vlist[i].colormap_size );

		if( vclass.matches(hints.visualClass()) ) {
		  itsVisInfo = vlist[i];
		  break;
		}
	 }
  }
  else {
	 itsVisInfo = itsPrefVisInfo;
  }

  itsVisual = itsVisInfo.visual;
  itsDepth  = itsVisInfo.depth;
}

void XStuff::createColormap(const XHints& hints) {
DOTRACE("XStuff::createColormap");

  int alloc = AllocNone;

  if (hints.privateCmap()) {
	 alloc = AllocAll;
  }

  itsColormap = 
	 XCreateColormap( itsDisplay,
							RootWindow( itsDisplay, itsScreen ),
							itsVisual, alloc );

  itsMeanColor.flags = DoRed | DoGreen | DoBlue;

  const int MEANGREY = 127;

  itsMeanColor.red = itsMeanColor.green = itsMeanColor.blue =
	 ( unsigned long )(257*MEANGREY);

  XAllocColor( itsDisplay, itsColormap, &itsMeanColor );
}

void XStuff::createWindow(const char* name) {
DOTRACE("XStuff::createWindow");

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
}

void XStuff::selectInput() {
DOTRACE("XStuff::selectInput");

  XSelectInput( itsDisplay, itsWindow, 
					 ExposureMask|StructureNotifyMask|
					 ButtonPressMask|KeyPressMask );

  XSync( itsDisplay, False );
}

void XStuff::setWmProperty(char* name) {
DOTRACE("XStuff::setWmProperty");

  char *list[1];
  XSizeHints	  *size_hints;
  XClassHint	  *class_hint;
  XWMHints	  *wm_hints;
  XTextProperty window_name, icon_name;

  class_hint = XAllocClassHint(  );
  class_hint->res_name  = "test";
  class_hint->res_class = "Test";

  size_hints = XAllocSizeHints(  );
  size_hints->flags	  = USSize|USPosition|PMinSize|PMaxSize;
  size_hints->x = 500;
  size_hints->y = 500;
  size_hints->width = itsWidth;
  size_hints->height = itsHeight;
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

void XStuff::setWmProtocol() {
DOTRACE("XStuff::setWmProtocol");

  Atom wm_protocols[2];

  wm_protocols[0] = XInternAtom( itsDisplay, "WM_DELETE_WINDOW", False );
  wm_protocols[1] = XInternAtom( itsDisplay, "WM_SAVE_YOURSELF", False );
  XSetWMProtocols( itsDisplay, itsWindow, wm_protocols, 2 );
}

void XStuff::mapWindow(const char* name) {
DOTRACE("XStuff::mapWindow");

  char temp_name[STRINGSIZE];
  strcpy(temp_name, name);

  setWmProperty(temp_name);
  setWmProtocol();

  XSetCommand( itsDisplay, itsWindow, itsArgv, itsArgc );

  XMapWindow( itsDisplay, itsWindow );
}

void XStuff::printWindowInfo() {
DOTRACE("XStuff::printWindowInfo");

  int items;
  XVisualInfo info;

  info.visualid = itsVisual->visualid;
  info = *XGetVisualInfo( itsDisplay, VisualIDMask, &info, &items );
  fprintf( stdout, " Window %d with %s visual %d of depth %d\n", 
			  0, VisualClass(info.c_class).name(), info.visualid, info.depth );
  fflush( stdout );
}

void XStuff::wrapX() {
DOTRACE("XStuff::wrapX");

  XDestroyWindow( itsDisplay, itsWindow );
  XCloseDisplay( itsDisplay );
}

void XStuff::storeColor(unsigned int index,
								double red, double green, double blue) {
DOTRACE("XStuff::storeColor");
  XColor col;
  col.flags = DoRed | DoGreen | DoBlue;
  col.pixel = index;
  col.red =   (unsigned short)(0xffff * red);
  col.green = (unsigned short)(0xffff * green);
  col.blue =  (unsigned short)(0xffff * blue);
  XStoreColor(itsDisplay, itsColormap, &col);
}

static const char vcid_xstuff_cc[] = "$Header$";
#endif // !XSTUFF_CC_DEFINED
