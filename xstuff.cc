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

  char extractKey(XEvent* event)
  {
    DOTRACE("<xstuff.cc>::extractKey");

    char buffer[10];
    KeySym keysym;

    int count = XLookupString(&event->xkey, buffer, 9,
                              &keysym, (XComposeStatus*)0);
    buffer[ count ] = '\0';

    if (count > 1 || keysym == XK_Return ||
        keysym == XK_BackSpace || keysym == XK_Delete)
      {
        return '\0';
      }

    if (keysym >= XK_KP_Space && keysym <= XK_KP_9 ||
        keysym >= XK_space    && keysym <= XK_asciitilde)
      {
        return buffer[0];
      }

    return '\0';
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
  itsWindow(0)
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
                        int width, int height)
{
DOTRACE("XStuff::openWindow");

  if (vinfo == 0)
    {
      std::cerr << "XVisualInfo* was null in openWindow()!\n";
      exit(1);
    }

  const int screen = DefaultScreen(itsDisplay);

  Colormap cmap =
    XCreateColormap(itsDisplay,
                    RootWindow(itsDisplay, screen),
                    vinfo->visual, AllocNone);

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
                            vinfo->depth,
                            InputOutput,
                            vinfo->visual,
                            (CWBackPixel | CWColormap
                             | CWBorderPixel | CWEventMask),
                            &winAttributes);

  XSelectInput(itsDisplay, itsWindow,
               ExposureMask|StructureNotifyMask|
               ButtonPressMask|KeyPressMask);

  XSync(itsDisplay, False);


  std::cout << " window with " << visual_class_name(vinfo->c_class)
            << " visual " << vinfo->visualid
            << " of depth " << vinfo->depth << std::endl;

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

      const char key = extractKey(&event);

      if (key != '\0')
        return key;
    }
}

void XStuff::getInt(int& i) const
{
DOTRACE("XStuff::getInt");

  const std::string buf = this->getWord();

  sscanf(&buf[0], "%d", &i);
}

void XStuff::getDouble(double& d) const
{
DOTRACE("XStuff::getDouble");

  const std::string buf = this->getWord();

  sscanf(&buf[0], "%lf", &d);
}

void XStuff::eventLoop(void* cdata,
                       ExposeFunc* onExpose,
                       KeyFunc* onKey)
{
DOTRACE("XStuff::eventLoop");

  XEvent event;

  while (true)
    {
      XNextEvent(itsDisplay, &event);

      switch (event.type)
        {
        case Expose:
          if (event.xexpose.count == 0)
            if (event.xexpose.window == itsWindow)
              (*onExpose)(cdata);
          break;

        case ButtonPress:
          if (event.xbutton.button == Button3)
            return;
          else
            (*onExpose)(cdata);
          break;

        case KeyPress:
          if (event.xkey.window == itsWindow)
            {
              char key = extractKey(&event);
              if ((*onKey)(cdata, double(event.xkey.time), key)
                  == true)
                return;
            }
          break;

        default:
          break;
        }
    }
}

std::string XStuff::getWord() const
{
DOTRACE("XStuff::getWord");

  char c;

  std::string result;

  while ((c = this->getKeypress()) != ' ')
    {
      result += c;
    }

  return result;
}

bool XStuff::getButtonPress(double& xtime,
                            int& button_number)
{
DOTRACE("XStuff::getButtonPress");

  XEvent event;

  while (XCheckMaskEvent(itsDisplay, ButtonPressMask, &event))
    {
      Assert(event.type == ButtonPress);

      if (event.xbutton.window != itsWindow)
        continue;

      button_number = 0;

      if (event.xbutton.button == Button1)
        button_number = 1;
      else if (event.xbutton.button == Button2)
        button_number = 2;
      else if (event.xbutton.button == Button3)
        button_number = 3;

      xtime = double(event.xbutton.time);
      return true;
    }

  return false;
}

static const char vcid_xstuff_cc[] = "$Header$";
#endif // !XSTUFF_CC_DEFINED
