///////////////////////////////////////////////////////////////////////
//
// glxwindow.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:55 2000
// written: Wed Sep  3 16:23:15 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_CC_DEFINED
#define XSTUFF_CC_DEFINED

#include "glxwindow.h"

#include <cstring>              // for strncpy
#include <cstdlib>              // for getenv
#include <cstdio>
#include <iostream>
#include <sstream>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "trace.h"
#include "debug.h"

extern "C"
{
  extern int glXGetVideoSyncSGI (unsigned int *);
  extern int glXWaitVideoSyncSGI (int, int, unsigned int *);
}

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
    std::cerr << "ERROR: unknown visual class " << c_class << "\n";
    exit(1);
  }

  char extractKey(XEvent* event)
  {
    DOTRACE("<glxwindow.cc>::extractKey");

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
// GlxWindow member definitions
//
///////////////////////////////////////////////////////////////////////

GlxWindow::GlxWindow(const char* winname,
                     int width, int height, int depth) :
  itsWidth(width),
  itsHeight(height),
  itsGLXContext(0),
  itsDisplay(0),
  itsWindow(0),
  itsUsingVsync(false)
{
DOTRACE("GlxWindow::GlxWindow");

  itsDisplay = XOpenDisplay(0);

  if (itsDisplay == 0)
    {
      if (getenv("DISPLAY") == 0)
        std::cerr << "ERROR: you need to set the DISPLAY environment variable\n";
      else
        std::cerr << "ERROR: couldn't open DISPLAY "
                  << getenv("DISPLAY") << "\n";
      exit(1);
    }

  int attribList[] =
    {
      GLX_DOUBLEBUFFER,
      GLX_RGBA,
      GLX_BUFFER_SIZE,
      depth,
      None
    };

  XVisualInfo* vinfo = glXChooseVisual(itsDisplay,
                                       DefaultScreen(itsDisplay),
                                       &attribList[0]);

  if (vinfo == 0)
    {
      std::cerr << "ERROR: couldn't find a matching XVisualInfo* for depth "
                << depth << "\n"
        "   (look at the output of 'glxinfo' to see which visual\n"
        "   contexts are available on your system, and then\n"
        "   try passing a different value to '--depth')\n";
      exit(1);
    }

  itsGLXContext = glXCreateContext(itsDisplay, vinfo, 0, GL_TRUE);

  if (itsGLXContext == 0)
    {
      std::cerr << "ERROR: couldn't get an OpenGL graphics context for depth "
                << depth << "\n"
        "   (look at the output of 'glxinfo' to see which visual\n"
        "   contexts are available on your system, and then\n"
        "   try passing a different value to '--depth')\n";
      exit(1);
    }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  const int screen = DefaultScreen(itsDisplay);

  const Window parent = RootWindow(itsDisplay, screen);

  Colormap cmap = XCreateColormap(itsDisplay,
                                  parent,
                                  vinfo->visual, AllocNone);

  XSetWindowAttributes winAttributes;

  winAttributes.event_mask =
    ExposureMask | StructureNotifyMask |
    ButtonPressMask | KeyPressMask;
  winAttributes.colormap  = cmap;

  itsWindow = XCreateWindow(itsDisplay,
                            parent,
                            0, 0, itsWidth, itsHeight, 2,
                            vinfo->depth,
                            InputOutput,
                            vinfo->visual,
                            (CWColormap | CWEventMask),
                            &winAttributes);

  XSync(itsDisplay, False);


  std::cout << " window with " << visual_class_name(vinfo->c_class)
            << " visual " << vinfo->visualid
            << " of depth " << vinfo->depth << std::endl;

  XStoreName(itsDisplay, itsWindow, winname);

  XMapWindow(itsDisplay, itsWindow);

  XFree(vinfo);

  glXMakeCurrent(itsDisplay, itsWindow, itsGLXContext);

  glViewport(0, 0, itsWidth, itsHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, itsWidth, 0, itsHeight, -1.0, 1.0);

  const std::string extensions =
    glXQueryExtensionsString(itsDisplay,
                             DefaultScreen(itsDisplay));

  std::istringstream s(extensions);

  std::string ext;

  std::cout << " glx extensions: \n";

  while (s >> ext)
    {
      std::cout << " \t" << ext << '\n';
    }

  if (extensions.find("GLX_SGI_video_sync") != std::string::npos)
    {
      itsUsingVsync = true;
      std::cout << " got GLX_SGI_video_sync extension\n";
    }
}

GlxWindow::~GlxWindow()
{
DOTRACE("GlxWindow::~GlxWindow");
  glXDestroyContext(itsDisplay, itsGLXContext);
  XDestroyWindow(itsDisplay, itsWindow);
  XCloseDisplay(itsDisplay);
}

void GlxWindow::swapBuffers()
{
DOTRACE("GlxWindow::swapBuffers");
  glFlush();
  unsigned int counter = 0;
  if (itsUsingVsync)
    glXGetVideoSyncSGI(&counter);

  glXSwapBuffers(itsDisplay, itsWindow);

  if (itsUsingVsync)
    glXWaitVideoSyncSGI(counter + 1, 0, &counter);
  else
    {
      glXWaitX();
      glXWaitGL();
    }
}

char GlxWindow::getKeypress() const
{
DOTRACE("GlxWindow::getKeypress");

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

void GlxWindow::getIntFromKeyboard(int& i) const
{
DOTRACE("GlxWindow::getIntFromKeyboard");

  const std::string buf = this->getWord();

  sscanf(&buf[0], "%d", &i);
}

void GlxWindow::getDoubleFromKeyboard(double& d) const
{
DOTRACE("GlxWindow::getDoubleFromKeyboard");

  const std::string buf = this->getWord();

  sscanf(&buf[0], "%lf", &d);
}

void GlxWindow::eventLoop(void* cdata,
                          ExposeFunc* onExpose,
                          KeyFunc* onKey)
{
DOTRACE("GlxWindow::eventLoop");

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

std::string GlxWindow::getWord() const
{
DOTRACE("GlxWindow::getWord");

  char c;

  std::string result;

  while ((c = this->getKeypress()) != ' ')
    {
      result += c;
    }

  return result;
}

bool GlxWindow::getButtonPress(double& xtime, int& button_number)
{
DOTRACE("GlxWindow::getButtonPress");

  XEvent event;

  while (XCheckMaskEvent(itsDisplay, ButtonPressMask, &event))
    {
      Assert(event.type == ButtonPress);

      if (event.xbutton.window != itsWindow)
        continue;

      button_number = 0;

      switch (event.xbutton.button)
        {
        case Button1: button_number = 1; break;
        case Button2: button_number = 2; break;
        case Button3: button_number = 3; break;
        default:      button_number = 0; break;
        }

      xtime = double(event.xbutton.time);
      return true;
    }

  return false;
}

static const char vcid_glxwindow_cc[] = "$Header$";
#endif // !XSTUFF_CC_DEFINED
