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
#include "xhints.h"
#include "xstuff.h"

#include <GL/glu.h>

#include <cstdlib>
#include <vector>

#define LOCAL_PROF
#include "trace.h"
#include "debug.h"

namespace
{
  void drawGLText(const char* word, int stroke_width,
                  int x_pos, int y_pos,
                  int char_width, int char_height,
                  bool rgba)
  {
  DOTRACE("<graphics.cc>::drawGLText");

    unsigned int listbase = GLFont::getStrokeFontListBase();

    double x_scale = char_width/5.0;
    double y_scale = char_height/6.0;

    if (rgba)
      glColor3d(1.0, 1.0, 1.0);
    else
      glIndexi(1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
      glPushAttrib(GL_LIST_BIT|GL_LINE_BIT);
      {
        glLineWidth(stroke_width);
        glTranslated(x_pos, y_pos, 0.0);
        glScaled(x_scale, y_scale, 1.0);

        glListBase( listbase );
        glCallLists(strlen(word), GL_BYTE, word);
      }
      glPopAttrib();
    }
    glPopMatrix();
  }
}

Graphics::Graphics(const XHints& hints) :
  itsWidth(hints.width()),
  itsHeight(hints.height()),
  itsFrameTime(-1.0),
  itsXStuff(hints),
  itsGLXContext(0),
  itsClearIndex(0),
  itsMovie(0),
  isItRecording(false),
  isItRgba(0),
  isItDoubleBuffered(0)
{
DOTRACE("Graphics::Graphics");
  std::vector<int> attribList;

  attribList.push_back(GLX_DOUBLEBUFFER);

  attribList.push_back(GLX_RGBA);

  attribList.push_back(GLX_BUFFER_SIZE);
  attribList.push_back(hints.depth());

  attribList.push_back(None);

  XVisualInfo* vi = glXChooseVisual(itsXStuff.display(),
                                    DefaultScreen(itsXStuff.display()),
                                    &attribList[0]);

  itsXStuff.setPrefVisInfo(vi);

  itsGLXContext = glXCreateContext(itsXStuff.display(), vi, 0, GL_TRUE);

  if ( itsGLXContext == 0 )
    {
      fprintf( stdout,"Couldn't get an OpenGL graphics context.\n" );
      exit( -1 );
    }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

Graphics::~Graphics()
{
DOTRACE("Graphics::~Graphics");
  delete isItDoubleBuffered;
  delete isItRgba;
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

  if (isDoubleBuffered())
    {
      clearBackBufferRegion(false);
      swapBuffers();
      clearBackBufferRegion(false);
      swapBuffers();
    }
  else
    {
      clearFrontBuffer();
    }
}

void Graphics::wrapGraphics()
{
DOTRACE("Graphics::wrapGraphics");
  glXDestroyContext(itsXStuff.display(), itsGLXContext);
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
#ifndef I686
  if (itsFrameTime < 0)
    itsFrameTime = computeFrameTime();
#else
  itsFrameTime = 13;
#endif
  return itsFrameTime;
}


bool Graphics::isRgba()
{
DOTRACE("Graphics::isRgba");

  if (isItRgba == 0)

{
      GLboolean is_rgba;
      glGetBooleanv(GL_RGBA_MODE, &is_rgba);
      isItRgba = new bool(is_rgba == GL_TRUE ? true : false);
    }
  return *isItRgba;
}

bool Graphics::isDoubleBuffered()
{
DOTRACE("Graphics::isDoubleBuffered");

  if (isItDoubleBuffered == 0)

{
      GLboolean is_db;
      glGetBooleanv(GL_DOUBLEBUFFER, &is_db);
      isItDoubleBuffered = new bool(is_db == GL_TRUE ? true : false);
    }
  return *isItDoubleBuffered;
}

void Graphics::clearFrontBuffer()
{
DOTRACE("Graphics::clearFrontBuffer");

  glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::clearBackBuffer()
{
DOTRACE("Graphics::clearBackBuffer");

  clearBackBufferRegion(true);
}

void Graphics::clearUpperPlanes()
{
DOTRACE("Graphics::clearUpperPlanes");

  writeUpperPlanes();

  clearFrontBuffer();

  writeLowerPlanes();
}

void Graphics::waitVerticalRetrace()
{
DOTRACE("Graphics::waitVerticalRetrace");
  /* do nothing */;
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

      glReadBuffer( GL_FRONT );
      glReadPixels( (width()-itsMovie->width()) / 2,
                    (height()-itsMovie->height()) / 2,
                    itsMovie->width(),
                    itsMovie->height(),
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    itsMovie->tempFrameBuffer() );

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

void Graphics::writeUpperPlanes()
{
DOTRACE("Graphics::writeUpperPlanes");
  glIndexMask(0xc0);
}

void Graphics::writeLowerPlanes()
{
DOTRACE("Graphics::writeLowerPlanes");
  glIndexMask(0x3f);
}

void Graphics::writeAllPlanes()
{
DOTRACE("Graphics::writeAllPlanes");
  glIndexMask(0xff);
}

void Graphics::drawMessage(char word[])
{
DOTRACE("Graphics::drawMessage");

  writeUpperPlanes();

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
             charWidth, charHeight,
             isRgba());

  writeLowerPlanes();
}

void Graphics::drawCross()
{
DOTRACE("Graphics::drawCross");
  drawCross( width()/2, height()/2 );
}

void Graphics::drawCross(int x, int y)
{
DOTRACE("Graphics::drawCross");

  writeUpperPlanes();

  if (isRgba())
    glColor3d(1.0, 1.0, 1.0);
  else
    glIndexi(192);

  glBegin(GL_LINES);
  glVertex2i(x-50, y);
  glVertex2i(x+50, y);
  glVertex2i(x, y-50);
  glVertex2i(x, y+50);
  glEnd();

  writeLowerPlanes();
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

  for( int i=0; i<nmenu; i++ )
    drawGLText(menu[i], 2,
               100, height() - 200 - i * (char_height*2),
               char_width, char_height,
               isRgba());
}

void Graphics::showParams(char params[][STRINGSIZE], int nparams)
{
DOTRACE("Graphics::showParams");
  int col1 = (nparams < 23) ? nparams : 23;
  int col2 = (nparams < 23) ? 0       : nparams;

  for( int i=0; i<col1; i++ )
    drawGLText(params[i], 2,
               width()/2 - 500, height()/2 -  450 + i * 40,
               10, 15,
               isRgba());

  if( col1+1 < col2 )
    {
      for( int i=col1+1; i<col2; i++ )
        drawGLText(params[i], 2,
                   width()/2 + 100, height()/2 - 1370 + i * 40,
                   10, 15,
                   isRgba());
    }
}

void Graphics::writeBitmap(unsigned char* ptr, int x, int y, int size)
{
DOTRACE("Graphics::writeBitmap");
  glRasterPos2i(x,y);

  glDrawPixels(size, size, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, ptr);
}

void Graphics::writeTrueColorMap(unsigned char* ptr, int x, int y, int size)
{
DOTRACE("Graphics::writeTrueColorMap");

  glRasterPos2i(x,y);

  glDrawPixels(size, size, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

}

void Graphics::moveBlock(int x, int y, int xsz, int ysz, int nx, int ny)
{
DOTRACE("Graphics::moveBlock");
  glRasterPos2i(nx, ny);
  glReadBuffer(GL_FRONT);
  glDrawBuffer(GL_BACK);
  glCopyPixels(x, y, xsz, ysz, GL_COLOR);
}

void Graphics::startRecording()
{
DOTRACE("Graphics::startRecording");

  if (MAKING_MOVIE && (itsMovie == 0))
    itsMovie = new SimpleMovie("ballmovie.mov", MV_FORMAT_QT,
                               DISPLAY_X, DISPLAY_Y);

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
  while( number-- )
    swapBuffers();
}

double Graphics::computeFrameTime()
{
DOTRACE("Graphics::computeFrameTime");

  struct timeval tp[2];

  clearFrontBuffer();

  waitFrameCount( 1 );

  Timing::getTime( &tp[0] );

  waitFrameCount( 99 );

  Timing::getTime( &tp[1] );

  double frametime = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  return frametime;
}

void Graphics::clearBackBufferRegion(bool use_scissor)
{
DOTRACE("Graphics::clearBackBufferRegion");

  if (false & use_scissor)
    {
      glEnable(GL_SCISSOR_TEST);
      glScissor( (width() - DISPLAY_X)/2, (height() - DISPLAY_Y)/2,
                 DISPLAY_X, DISPLAY_Y );
    }

  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT);

  if (use_scissor)
    {
      glDisable(GL_SCISSOR_TEST);
    }
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
