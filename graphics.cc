///////////////////////////////////////////////////////////////////////
//
// graphics.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:01:32 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_CC_DEFINED
#define GRAPHICS_CC_DEFINED

#include "graphics.h"

#include "glfont.h"
#include "timing.h"
#include "trace.h"
#include "xstuff.h"

#include <GL/glu.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

#define LOCAL_PROF
#include "trace.h"
#include "debug.h"

namespace
{
  void drawGLText(const std::string& word, int stroke_width,
                  double x_pos, double y_pos,
                  double char_width, double char_height)
  {
    DOTRACE("<graphics.cc>::drawGLText");

    const unsigned int listbase = GLFont::getStrokeFontListBase();

    const double x_scale = char_width/5.0;
    const double y_scale = char_height/6.0;

    glColor3d(1.0, 1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
      glPushAttrib(GL_LIST_BIT|GL_LINE_BIT);
      {
        glLineWidth(stroke_width);
        glTranslated(x_pos, y_pos, 0.0);
        glScaled(x_scale, y_scale, 1.0);

        glListBase(listbase);
        glCallLists(word.length(), GL_BYTE, word.c_str());
      }
      glPopAttrib();
    }
    glPopMatrix();
  }
}

Graphics::Graphics(const char* winname,
                   int w, int h, int depth) :
  itsWidth(w),
  itsHeight(h),
  itsXStuff(w, h),
  itsGLXContext(0),
  itsUsingVsync(false),
  isItRecording(false),
  itsFrameCounter(0),
  itsFrameTimer(Timepoint::now())
{
DOTRACE("Graphics::Graphics");

  int attribList[] =
    {
      GLX_DOUBLEBUFFER,
      GLX_RGBA,
      GLX_BUFFER_SIZE,
      depth,
      None
    };

  XVisualInfo* vi = glXChooseVisual(itsXStuff.display(),
                                    DefaultScreen(itsXStuff.display()),
                                    &attribList[0]);

  itsGLXContext = glXCreateContext(itsXStuff.display(), vi, 0, GL_TRUE);

  if (itsGLXContext == 0)
    {
      std::cout << "Couldn't get an OpenGL graphics context.\n";
      exit(1);
    }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  itsXStuff.openWindow(winname, vi, itsWidth, itsHeight);

  XFree(vi);

  glXMakeCurrent(itsXStuff.display(), itsXStuff.window(),
                 itsGLXContext);

  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), 0, height(), -1.0, 1.0);

  const std::string extensions =
    glXQueryExtensionsString(itsXStuff.display(),
                             DefaultScreen(itsXStuff.display()));

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

  this->clearBackBuffer();
  this->swapBuffers();
  this->clearBackBuffer();
  this->swapBuffers();
}

Graphics::~Graphics()
{
DOTRACE("Graphics::~Graphics");
  glXDestroyContext(itsXStuff.display(), itsGLXContext);
}

void Graphics::gfxWait(const Timepoint& t, double delaySeconds)
{
DOTRACE("Graphics::gfxWait");

  if (isItRecording)
    {
      const int nframes = int(delaySeconds/30.0);
      dumpFrames(nframes);
    }
  else
    {
      while (t.elapsedMsec() < 1000*delaySeconds)
        {
          usleep(1000);
        }
    }
}

void Graphics::clearBackBuffer()
{
DOTRACE("Graphics::clearBackBuffer");

  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT);
}

extern "C"
{
  extern int glXGetVideoSyncSGI (unsigned int *);
  extern int glXWaitVideoSyncSGI (int, int, unsigned int *);
}

double Graphics::swapBuffers()
{
DOTRACE("Graphics::swapBuffers");
  glFlush();
  unsigned int counter = 0;
  if (itsUsingVsync)
    glXGetVideoSyncSGI(&counter);

  glXSwapBuffers(itsXStuff.display(), itsXStuff.window());

  if (itsUsingVsync)
    glXWaitVideoSyncSGI(counter + 1, 0, &counter);
  else
    {
      glXWaitX();
      glXWaitGL();
    }

  if (isItRecording)
    {
      dumpFrames(1);
      return 33.0;
    }

  return itsFrameTimer.elapsedMsecAndReset();
}

void Graphics::drawMessage(const std::string& word)
{
DOTRACE("Graphics::drawMessage");

  if (word.length() == 0)
    return;

  const int availWidth = int(0.8 * width());

  const double charWidth = availWidth / word.length();
  const double charHeight = charWidth * 1.5;

  drawGLText(word, 4,
             (width()-availWidth)/2, (height()-charHeight)/2,
             charWidth, charHeight);
}

void Graphics::drawCross()
{
DOTRACE("Graphics::drawCross");
  drawCross(width()/2, height()/2);
}

void Graphics::drawLine(double x1, double y1, double x2, double y2)
{
DOTRACE("Graphics::drawLine");

  glColor4d(1.0, 1.0, 1.0, 1.0);

  glBegin(GL_LINES);
  glVertex2d(x1, y1);
  glVertex2d(x2, y2);
  glEnd();
}

void Graphics::drawCross(int x, int y)
{
DOTRACE("Graphics::drawCross");

  glColor4d(1.0, 1.0, 1.0, 1.0);

  glBegin(GL_LINES);
  glVertex2i(x-50, y);
  glVertex2i(x+50, y);
  glVertex2i(x, y-50);
  glVertex2i(x, y+50);
  glEnd();
}

void Graphics::drawStrings(const std::string* strings, int nstrings,
                           double xpos, double ypos, double char_width,
                           int stroke_width)
{
DOTRACE("Graphics::drawStrings");

  const double char_height = char_width*1.5;

  if (xpos < 0.0)
    xpos = height() + xpos;

  if (ypos < 0.0)
    ypos = height() + ypos;

  for (int i = 0; i < nstrings; ++i)
    drawGLText(strings[i], stroke_width,
               xpos, ypos - (2*i*char_height),
               char_width, char_height);
}

void Graphics::writePixmap(int x, int y, unsigned char* ptr, int size)
{
DOTRACE("Graphics::writePixmap");

  glRasterPos2i(x,y);

  glDrawPixels(size, size, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

}

void Graphics::startRecording()
{
DOTRACE("Graphics::startRecording");

  isItRecording = true;
  std::cout << "starting dumping frames\n";
}

void Graphics::stopRecording()
{
DOTRACE("Graphics::stopRecording");

  isItRecording = false;
  std::cout << "stopping dumping frames\n";
}

void Graphics::dumpFrames(int count)
{
DOTRACE("Graphics::dumpFrame");

  glReadBuffer(GL_FRONT);

  const int bufsiz = this->width() * this->height() * 3;
  unsigned char buf[bufsiz];

  glReadPixels(0, 0,
               this->width(),
               this->height(),
               GL_RGB,
               GL_UNSIGNED_BYTE,
               static_cast<void*>(&buf[0]));

  while (count-- > 0)
    {
      char framename[128];
      snprintf(framename, 128, "frame_%06d.ppm", itsFrameCounter++);

      std::ofstream ofs(framename);

      ofs << "P6\n"
          << this->width() << " " << this->height() << '\n'
          << "255\n";

      ofs.write(reinterpret_cast<const char*>(&buf[0]), bufsiz);

      ofs.close();
    }
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
