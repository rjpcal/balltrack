///////////////////////////////////////////////////////////////////////
//
// openglgfx.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 15:05:30 2000
// written: Tue Feb 29 18:06:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OPENGLGFX_CC_DEFINED
#define OPENGLGFX_CC_DEFINED

#include "openglgfx.h"

#include <GL/glu.h>

#include <cstdlib>
#include <vector>

#include "glfont.h"
#include "params.h"
#include "timing.h"
#include "xhints.h"
#include "xstuff.h"

#include "trace.h"
#include "debug.h"

namespace {
  void drawGLText(const char* word, int stroke_width,
						int x_pos, int y_pos,
						int char_width, int char_height,
						bool rgba) {
  DOTRACE("<openglgfx.cc>::drawGLText");

	 unsigned int listbase = GLFont::getStrokeFontListBase();

	 double x_scale = char_width/4.0;
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

///////////////////////////////////////////////////////////////////////
//
// OpenglGfx member definitions
//
///////////////////////////////////////////////////////////////////////

OpenglGfx::OpenglGfx(XStuff* xinfo, const XHints& hints,
							int aWidth, int aHeight) :
  Graphics(aWidth, aHeight),
  itsXStuff(xinfo),
  itsGLXContext(0),
  itsClearIndex(0)
{
DOTRACE("OpenglGfx::OpenglGfx");
  vector<int> attribList;

  if (hints.doubleBuffer()) 
	 attribList.push_back(GLX_DOUBLEBUFFER);

  if (hints.rgba())
	 attribList.push_back(GLX_RGBA);
  
  attribList.push_back(GLX_BUFFER_SIZE);
  attribList.push_back(hints.depth());
  attribList.push_back(None);

  XVisualInfo* vi = glXChooseVisual(itsXStuff->display(),
												DefaultScreen(itsXStuff->display()),
												&attribList[0]);

  itsXStuff->setPrefVisInfo(vi);

  itsGLXContext = glXCreateContext(itsXStuff->display(), vi, 0, GL_TRUE);

  if ( itsGLXContext == 0 )
    {
		fprintf( stdout,"Couldn't get an OpenGL graphics context.\n" );
		exit( -1 );
    }
}

void OpenglGfx::initWindow() {
DOTRACE("OpenglGfx::initWindow");

  glXMakeCurrent(itsXStuff->display(), itsXStuff->window(), itsGLXContext);

  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), 0, height(), -1.0, 1.0);

  glClearIndex(0);
  glClear(GL_COLOR_BUFFER_BIT);

  checkFrameTime();

  if (isDoubleBuffered()) {
    clearBackBufferRegion(false);
    swapBuffers();
    clearBackBufferRegion(false);
    swapBuffers();
  }
  else {
    clearFrontBuffer();
  }
}

void OpenglGfx::wrapGraphics() {
DOTRACE("OpenglGfx::wrapGraphics");
  glXDestroyContext(itsXStuff->display(), itsGLXContext);
}

bool OpenglGfx::isRgba() {
DOTRACE("OpenglGfx::isRgba");
  GLboolean is_rgba;
  glGetBooleanv(GL_RGBA_MODE, &is_rgba);
  return bool(is_rgba);
}

bool OpenglGfx::isDoubleBuffered() {
DOTRACE("OpenglGfx::isDoubleBuffered");
  GLboolean is_db;
  glGetBooleanv(GL_DOUBLEBUFFER, &is_db);
  return bool(is_db);
}

void OpenglGfx::writeUpperPlanes() {
DOTRACE("OpenglGfx::writeUpperPlanes");
  glIndexMask(0xc0);
}

void OpenglGfx::writeLowerPlanes() {
DOTRACE("OpenglGfx::writeLowerPlanes");
  glIndexMask(0x3f);
}

void OpenglGfx::writeAllPlanes() {
DOTRACE("OpenglGfx::writeAllPlanes");
  glIndexMask(0xff);
}

void OpenglGfx::waitVerticalRetrace() {
DOTRACE("OpenglGfx::waitVerticalRetrace");
  /* do nothing */;
}

void OpenglGfx::swapBuffers() {
DOTRACE("OpenglGfx::swapBuffers");
  glXSwapBuffers(itsXStuff->display(), itsXStuff->window());
  glXWaitGL();
  glXWaitX();
}

void OpenglGfx::waitFrameCount(int number) {
DOTRACE("OpenglGfx::waitFrameCount");
  while( number-- )
 	 swapBuffers();
}

void OpenglGfx::drawMessage(char word[]) {
DOTRACE("OpenglGfx::drawMessage");

  writeUpperPlanes();

  drawGLText(word, 4,
				 width()/2 - 500, height()/2-100,
				 200, 250,
				 isRgba());

  writeLowerPlanes();
}

void OpenglGfx::drawCross() {
DOTRACE("OpenglGfx::drawCross");

  writeUpperPlanes();

  if (isRgba())
	 glColor3d(1.0, 1.0, 1.0);
  else
	 glIndexi(192);

  glBegin(GL_LINES);
  glVertex2i(width()/2-50, height()/2);
  glVertex2i(width()/2+50, height()/2);
  glVertex2i(width()/2, height()/2-50);
  glVertex2i(width()/2, height()/2+50);
  glEnd();

  writeLowerPlanes();
}

void OpenglGfx::clearUpperPlanes() {
DOTRACE("OpenglGfx::clearUpperPlanes");

  writeUpperPlanes();

  clearFrontBuffer();

  writeLowerPlanes();
}

void OpenglGfx::loadColormap(float colors[][3], int ncolors) {
DOTRACE("OpenglGfx::loadColormap");

  waitVerticalRetrace();
  for (int i = 0; i < ncolors; ++i) {
	 itsXStuff->storeColor(i, colors[i][0], colors[i][1], colors[i][2]);
  }
}

void OpenglGfx::clearFrontBuffer() {
DOTRACE("OpenglGfx::clearFrontBuffer");

  glClear(GL_COLOR_BUFFER_BIT); 
}

void OpenglGfx::clearBackBufferRegion(bool use_scissor) {
DOTRACE("OpenglGfx::clearBackBufferRegion");

  if (false & use_scissor) { 
	 glEnable(GL_SCISSOR_TEST);
	 glScissor( (width() - DISPLAY_X)/2, (height() - DISPLAY_Y)/2,
					DISPLAY_X, DISPLAY_Y );
  }

  glDrawBuffer(GL_BACK); 
  glClear(GL_COLOR_BUFFER_BIT);

  if (use_scissor) {
	 glDisable(GL_SCISSOR_TEST);
  }
}

void OpenglGfx::clearBackBuffer() {
DOTRACE("OpenglGfx::clearBackBuffer");

  clearBackBufferRegion(true);
}

void OpenglGfx::showMenu(char menu[][STRINGSIZE], int nmenu) {
DOTRACE("OpenglGfx::showMenu");
  clearFrontBuffer();
	 
  glIndexi(1);
	 
  glFlush();
  glXSwapBuffers(itsXStuff->display(), itsXStuff->window());

  int char_width = 13;
  int char_height = 25;

  for( int i=0; i<nmenu; i++ )
	 drawGLText(menu[i], 2,
					100, height() - 200 - i * (char_height*2),
					char_width, char_height,
					isRgba());
}

void OpenglGfx::showParams(char params[][STRINGSIZE], int nparams) {
DOTRACE("OpenglGfx::showParams");
  int col1 = (nparams < 23) ? nparams : 23;
  int col2 = (nparams < 23) ? 0       : nparams;

  for( int i=0; i<col1; i++ )
	 drawGLText(params[i], 2,
					width()/2 - 500, height()/2 -  450 + i * 40,
					10, 15,
					isRgba());

  if( col1+1 < col2 ) {
	 for( int i=col1+1; i<col2; i++ ) 
		drawGLText(params[i], 2,
					  width()/2 + 100, height()/2 - 1370 + i * 40,
					  10, 15,
					  isRgba());
  }
}

void OpenglGfx::moveBlock(int x, int y, int xsz, int ysz, int nx, int ny) {
DOTRACE("OpenglGfx::moveBlock");
  glRasterPos2i(nx, ny);
  glReadBuffer(GL_FRONT);
  glDrawBuffer(GL_BACK);
  glCopyPixels(x, y, xsz, ysz, GL_COLOR);
}

void OpenglGfx::writeBitmap(unsigned char* ptr, int x, int y, int size) {
DOTRACE("OpenglGfx::writeBitmap");
  glRasterPos2i(x,y);
 
  glDrawPixels(size, size, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, ptr);
}

void OpenglGfx::writeTrueColorMap(unsigned char* ptr, int x, int y, int size) {
DOTRACE("OpenglGfx::writeTrueColorMap");
  glRasterPos2i(x,y);
 
  glDrawPixels(size, size, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
}


///////////////////////////////////////////////////////////////////////
//
// OpenglGfx private member definitions
//
///////////////////////////////////////////////////////////////////////

void OpenglGfx::checkFrameTime() {
DOTRACE("OpenglGfx::checkFrameTime");

  struct timeval tp[2];

  clearFrontBuffer();

  waitFrameCount( 1 );

  Timing::getTime( &tp[0] );

  waitFrameCount( 99 );

  Timing::getTime( &tp[1] );

  FRAMETIME = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  printf( " Video frame time %7.4lf ms\n", FRAMETIME );  
}

static const char vcid_openglgfx_cc[] = "$Header$";
#endif // !OPENGLGFX_CC_DEFINED
