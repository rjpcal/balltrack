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

#include <cstring>              // for strncpy
#include <cstdlib>              // for getenv
#include <cstdio>
#include <iostream>
#include <X11/keysym.h>

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
    std::cerr << "unknown visual class " << c_class << "\n";
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
      if (getenv("DISPLAY") == 0)
        std::cerr << "You need to set the DISPLAY env var\n";
      else
        std::cerr << "Cannot open DISPLAY "
                  << getenv("DISPLAY") << "\n";
      exit(1);
    }

}

XStuff::~XStuff()
{
DOTRACE("XStuff::~XStuff");
  XDestroyWindow(itsDisplay, itsWindow);
  XCloseDisplay(itsDisplay);
}

void XStuff::openWindow(const char* winname,
                        const XVisualInfo* vinfo,
                        int width, int height, int depth)
{
DOTRACE("XStuff::openWindow");

  if (vinfo == 0)
    {
      std::cerr << "XVisualInfo* was null in openWindow()!\n";
      exit(1);
    }

  itsVisInfo = *vinfo;

  const int screen = DefaultScreen(itsDisplay);

  Colormap cmap =
    XCreateColormap(itsDisplay,
                    RootWindow(itsDisplay, screen),
                    itsVisInfo.visual, AllocNone);

  XColor mean_color;
  mean_color.flags = DoRed | DoGreen | DoBlue;

  const int MEANGREY = 127;

  mean_color.red = mean_color.green = mean_color.blue =
    (unsigned long)(257*MEANGREY);

  XAllocColor(itsDisplay, cmap, &mean_color);


  XSetWindowAttributes winAttributes;

  winAttributes.event_mask = ExposureMask;
  winAttributes.colormap  = cmap;
  winAttributes.background_pixel = mean_color.pixel;
  winAttributes.border_pixel = mean_color.pixel;

  itsWindow = XCreateWindow(itsDisplay,
                            RootWindow(itsDisplay, screen),
                            0, 0, itsWidth, itsHeight, 2,
                            itsVisInfo.depth,
                            InputOutput,
                            itsVisInfo.visual,
                            (CWBackPixel | CWColormap
                             | CWBorderPixel | CWEventMask),
                            &winAttributes);

  XSelectInput(itsDisplay, itsWindow,
               ExposureMask|StructureNotifyMask|
               ButtonPressMask|KeyPressMask);

  XSync(itsDisplay, False);


  std::cout << " window with " << visual_class_name(itsVisInfo.c_class)
            << " visual " << itsVisInfo.visualid
            << " of depth " << itsVisInfo.depth << std::endl;

  char temp_name[256];
  strncpy(temp_name, winname, 256);
  char* nameptr = &temp_name[0];

  XTextProperty window_name;
  XStringListToTextProperty(&nameptr, 1, &window_name);

  XSetWMName(itsDisplay, itsWindow, &window_name);

  XMapWindow(itsDisplay, itsWindow);
}

char XStuff::getKeypress() const
{
DOTRACE("XStuff::getKeypress");

  while (true)
    {
      XEvent event;
      XNextEvent(itsDisplay, &event);

      if (event.type != KeyPress || event.xkey.window != itsWindow)
        continue;

      KeySym keysym;
      char buffer[10];

      // For some reason, if we pass a non-null XComposeStatus* to this
      // function, the 'buffer' gets screwed up... very weird.
      int count = XLookupString((XKeyEvent *) &event, &buffer[0], 9,
                                &keysym, (XComposeStatus*) 0);

      buffer[ count ] = '\0';

      DebugEvalNL(buffer);
      if (count > 1 || keysym == XK_Return ||
          keysym == XK_BackSpace || keysym == XK_Delete)
        {
          continue;
        }

      if (keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
          keysym >= XK_space    && keysym <= XK_asciitilde)
        {
          DebugEvalNL(buffer[0]);
          return buffer[0];
        }
    }
}

void XStuff::getWord(char* buf, int sz) const
{
DOTRACE("XStuff::getWord");

  int n = 0;
  char c;

  while ((c = this->getKeypress()) != ' '  && n < sz)
    {
      buf[n++] = c;
    }

  buf[n] = '\0';
}

void XStuff::getInt(int* pi) const
{
DOTRACE("XStuff::getInt");

  char word[STRINGSIZE];
  this->getWord(word, STRINGSIZE);

  sscanf(word, "%d", pi);
}

void XStuff::getFloat(float* pf) const
{
DOTRACE("XStuff::getFloat");

  char word[STRINGSIZE];
  this->getWord(word, STRINGSIZE);

  sscanf(word, "%f", pf);
}

static const char vcid_xstuff_cc[] = "$Header$";
#endif // !XSTUFF_CC_DEFINED
