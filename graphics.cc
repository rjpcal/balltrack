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
#include "params.h"
#include "simplemovie.h"
#include "timing.h"
#include "trace.h"
#include "xstuff.h"

#include <GL/glu.h>

#include <cstdlib>
#include <cstring>

#define LOCAL_PROF
#include "trace.h"
#include "debug.h"

namespace
{
  void drawGLText(const char* word, int stroke_width,
                  int x_pos, int y_pos,
                  int char_width, int char_height)
  {
  DOTRACE("<graphics.cc>::drawGLText");

    unsigned int listbase = GLFont::getStrokeFontListBase();

    double x_scale = char_width/5.0;
    double y_scale = char_height/6.0;

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
        glCallLists(strlen(word), GL_BYTE, word);
      }
      glPopAttrib();
    }
    glPopMatrix();
  }
}

Graphics::Graphics(const char* winname,
                   int width, int height, int depth) :
  itsWidth(width),
  itsHeight(height),
  itsFrameTime(-1.0),
  itsXStuff(width, height),
  itsGLXContext(0),
  itsClearIndex(0),
  itsMovie(0),
  isItRecording(false)
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
      fprintf(stdout,"Couldn't get an OpenGL graphics context.\n");
      exit(-1);
    }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  itsXStuff.openWindow(winname, vi, width, height, depth);

  XFree(vi);

  initWindow();
}

Graphics::~Graphics()
{
DOTRACE("Graphics::~Graphics");
  glXDestroyContext(itsXStuff.display(), itsGLXContext);
  delete itsMovie;
}

void Graphics::initWindow()
{
DOTRACE("Graphics::initWindow");

  glXMakeCurrent(itsXStuff.display(), itsXStuff.window(), itsGLXContext);

  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), 0, height(), -1.0, 1.0);

  glClearIndex(0);
  glClear(GL_COLOR_BUFFER_BIT);

  // forces the frame time to be computed and then cached in the base class
  frameTime();

  clearBackBuffer();
  swapBuffers();
  clearBackBuffer();
  swapBuffers();
}

void Graphics::gfxWait(double delaySeconds)
{
DOTRACE("Graphics::gfxWait");
  if (itsMovie && isItRecording)
    {
      int nframes = int(delaySeconds * itsMovie->frameRate());

      // Repeat frames of whatever was most recently appended
      for (int i = 0; i < nframes; ++i)
        itsMovie->appendTempBuffer();
    }
  else
    {
      Timing::mainTimer.wait(delaySeconds);
    }
}

double Graphics::frameTime()
{
#if 0
  if (itsFrameTime < 0)
    itsFrameTime = computeFrameTime();
#else
  itsFrameTime = 13;
#endif
  return itsFrameTime;
}

void Graphics::clearFrontBuffer()
{
DOTRACE("Graphics::clearFrontBuffer");

  glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::clearBackBuffer()
{
DOTRACE("Graphics::clearBackBuffer");

  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::swapBuffers()
{
DOTRACE("Graphics::swapBuffers");
  glXSwapBuffers(itsXStuff.display(), itsXStuff.window());
  glXWaitGL();
  glXWaitX();

  if (itsMovie && isItRecording)
    {
      static int f = 0;
      ++f;

      glReadBuffer(GL_FRONT);
      glReadPixels((width()-itsMovie->width()) / 2,
                   (height()-itsMovie->height()) / 2,
                   itsMovie->width(),
                   itsMovie->height(),
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   itsMovie->tempFrameBuffer());

      unsigned int* buf = (unsigned int*) itsMovie->tempFrameBuffer();

      for (unsigned int i = 0; i < itsMovie->frameSize()/4; ++i)
        {
          unsigned int reformat = 0;

          reformat |= (buf[i] & 0xff000000) >> 24;
          reformat |= (buf[i] & 0x00ff0000) >> 8;
          reformat |= (buf[i] & 0x0000ff00) << 8;

          buf[i] = reformat;
        }

      itsMovie->appendTempBuffer();

//     if (f > 90)
//       exit(1);
    }
}

void Graphics::drawMessage(char word[])
{
DOTRACE("Graphics::drawMessage");

  int nchars = 0;

  char* p = &word[0];
  while (*p) { ++p; ++nchars; }

  int availWidth = int(0.8 * width());

  if (itsMovie && isItRecording)
    {
      availWidth = itsMovie->width();
    }

  int charWidth = availWidth / nchars;
  int charHeight = int (charWidth * 1.4);

  drawGLText(word, 4,
             (width()-availWidth)/2, (height()-charHeight)/2,
             charWidth, charHeight);
}

void Graphics::drawCross()
{
DOTRACE("Graphics::drawCross");
  drawCross(width()/2, height()/2);
}

void Graphics::drawCross(int x, int y)
{
DOTRACE("Graphics::drawCross");

  glColor3d(1.0, 1.0, 1.0);

  glBegin(GL_LINES);
  glVertex2i(x-50, y);
  glVertex2i(x+50, y);
  glVertex2i(x, y-50);
  glVertex2i(x, y+50);
  glEnd();
}

void Graphics::showMenu(char menu[][STRINGSIZE], int nmenu)
{
DOTRACE("Graphics::showMenu");
  clearFrontBuffer();

  glIndexi(1);

  glFlush();
  glXSwapBuffers(itsXStuff.display(), itsXStuff.window());

  int char_width = 16;
  int char_height = 25;

  for (int i=0; i<nmenu; ++i)
    drawGLText(menu[i], 2,
               100, height() - 200 - i * (char_height*2),
               char_width, char_height);
}

void Graphics::showParams(char params[][STRINGSIZE], int nparams)
{
DOTRACE("Graphics::showParams");
  int col1 = (nparams < 23) ? nparams : 23;
  int col2 = (nparams < 23) ? 0       : nparams;

  for (int i=0; i<col1; ++i)
    drawGLText(params[i], 2,
               width()/2 - 500, height()/2 -  450 + i * 40,
               10, 15);

  if (col1+1 < col2)
    {
      for (int i=col1+1; i<col2; ++i)
        drawGLText(params[i], 2,
                   width()/2 + 100, height()/2 - 1370 + i * 40,
                   10, 15);
    }
}

void Graphics::writePixmap(int x, int y, unsigned char* ptr, int size)
{
DOTRACE("Graphics::writePixmap");

  glRasterPos2i(x,y);

  glDrawPixels(size, size, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

}

void Graphics::startRecording(int width, int height)
{
DOTRACE("Graphics::startRecording");

  if (itsMovie == 0)
    itsMovie = new SimpleMovie("ballmovie.mov", MV_FORMAT_QT,
                               width, height);

  isItRecording = true;
}

void Graphics::stopRecording()
{
DOTRACE("Graphics::stopRecording");

  if (itsMovie != 0)
    itsMovie->flush();

  isItRecording = false;
}

void Graphics::waitFrameCount(int number)
{
DOTRACE("Graphics::waitFrameCount");
  while (number--)
    swapBuffers();
}

double Graphics::computeFrameTime()
{
DOTRACE("Graphics::computeFrameTime");

  struct timeval tp[2];

  clearFrontBuffer();

  waitFrameCount(1);

  Timing::getTime(&tp[0]);

  waitFrameCount(99);

  Timing::getTime(&tp[1]);

  double frametime = Timing::elapsedMsec(&tp[0], &tp[1]) / 100.0;

  return frametime;
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
