///////////////////////////////////////////////////////////////////////
//
// openglgfx.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 15:05:30 2000
// written: Tue Feb 29 12:24:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OPENGLGFX_CC_DEFINED
#define OPENGLGFX_CC_DEFINED

#include "openglgfx.h"

#include <GL/glu.h>

#include <cmath>
#include <cstdlib>
#include <X11/keysym.h>
#include <vector>

#include "application.h"
#include "applic.h"
#include "balls.h"
#include "glfont.h"
#include "params.h"
#include "timing.h"
#include "xhints.h"
#include "xstuff.h"

#include "trace.h"
#include "debug.h"

#define CMAP_NUMBER  3
#define COLOR_NUMBER 256

namespace {
  float Color[CMAP_NUMBER][COLOR_NUMBER][3];


  void drawGLText(const char* word, int stroke_width,
						int x_pos, int y_pos,
						int char_width, int char_height) {
  DOTRACE("<openglgfx.cc>::drawGLText");

	 unsigned int listbase = GLFont::getStrokeFontListBase();

	 double x_scale = char_width/4.0;
	 double y_scale = char_height/6.0;

#if defined(COLOR_INDEX)
	 glIndexi(1);
#elif defined(RGBA)
	 glColor3d(1.0, 1.0, 1.0);
#else
#  error No color format macro.
#endif

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

void OpenglGfx::makeCurrent() {
DOTRACE("OpenglGfx::makeCurrent");
  glXMakeCurrent(itsXStuff->display(), itsXStuff->window(), itsGLXContext);

  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), 0, height(), -1.0, 1.0);

  glClearIndex(0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglGfx::wrapGraphics() {
DOTRACE("OpenglGfx::wrapGraphics");
  restoreColormap();

  glXDestroyContext(itsXStuff->display(), itsGLXContext);
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
				 200, 250);

  writeLowerPlanes();
}

void OpenglGfx::drawCross() {
DOTRACE("OpenglGfx::drawCross");

  writeUpperPlanes();

#if defined(COLOR_INDEX)
  glIndexi(192);
#elif defined(RGBA)
  glColor3d(1.0, 1.0, 1.0);
#else
#  error No color format macro.
#endif

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

  clearWindow();

  writeLowerPlanes();
}

void OpenglGfx::setTransparent() {
DOTRACE("OpenglGfx::setTransparent");

#ifdef COLOR_INDEX
  waitVerticalRetrace();
  for (int i = 0; i < COLOR_NUMBER; ++i) {
	 itsXStuff->storeColor(i,
								  Color[0][i][0],
								  Color[0][i][1],
								  Color[0][i][2]);
  }
#endif
}

void OpenglGfx::initWindow() {
DOTRACE("OpenglGfx::initWindow");

//   sizeColormap();

//   saveColormap();

  newColormap();

  setTransparent();

  checkFrameTime();

  clearWindow();
}

void OpenglGfx::clearWindow() {
DOTRACE("OpenglGfx::clearWindow");

  GLint mask;
  glGetIntegerv(GL_INDEX_WRITEMASK, &mask);

  glClearIndex(itsClearIndex);
  glClear(GL_COLOR_BUFFER_BIT); 
}

void OpenglGfx::clearBackBuffer() {
DOTRACE("OpenglGfx::clearBackBuffer");
  glDrawBuffer(GL_BACK); 
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglGfx::showMenu(char menu[][STRINGSIZE], int nmenu) {
DOTRACE("OpenglGfx::showMenu");
  clearWindow();
	 
  glIndexi(1);
	 
  glFlush();
  glXSwapBuffers(itsXStuff->display(), itsXStuff->window());

  int char_width = 13;
  int char_height = 25;

  for( int i=0; i<nmenu; i++ )
	 drawGLText(menu[i], 2,
					100, height() - 200 - i * (char_height*2),
					char_width, char_height);
}

void OpenglGfx::showParams(char params[][STRINGSIZE], int nparams) {
DOTRACE("OpenglGfx::showParams");
  int col1 = (nparams < 23) ? nparams : 23;
  int col2 = (nparams < 23) ? 0       : nparams;

  for( int i=0; i<col1; i++ )
	 drawGLText(params[i], 2,
					width()/2 - 500, height()/2 -  450 + i * 40,
					10, 15);

  if( col1+1 < col2 ) {
	 for( int i=col1+1; i<col2; i++ ) 
		drawGLText(params[i], 2,
						 width()/2 + 100, height()/2 - 1370 + i * 40,
						 10, 15);
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

  clearWindow();

  waitFrameCount( 1 );

  Timing::getTime( &tp[0] );

  waitFrameCount( 99 );

  Timing::getTime( &tp[1] );

  FRAMETIME = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  printf( " Video frame time %7.4lf ms\n", FRAMETIME );  
}

void OpenglGfx::sizeColormap() {
DOTRACE("OpenglGfx::sizeColormap");

//   float dummy[2][3];
//   int color_number;

//   inquire_sizes( fildes(), dummy, dummy[0],
// 					  dummy[0], dummy[0], &color_number);

//   if( color_number != COLOR_NUMBER )
//     {
// 		printf( " Colormap is of size %d instead of %d\n",
// 				  color_number, COLOR_NUMBER );

// 		exit(0);
//     }
}

void OpenglGfx::saveColormap() {
DOTRACE("OpenglGfx::saveColormap");
//   inquire_color_table( fildes(), 0, COLOR_NUMBER, Color[2] );
}

void OpenglGfx::newColormap() {
DOTRACE("OpenglGfx::newColormap");

  float lmin =   0.;
  float lmax = 196.;

  int n;
  float ratio;

  for( n=0; n<BALL_COLOR_MIN; n++ )
	 Color[0][n][0] = Color[0][n][1] = Color[0][n][2] = 0.;

  for( n=BALL_COLOR_MIN; n<=BALL_COLOR_MAX; n++ )
    {
		ratio = lmin/lmax +
		  ( (lmax-lmin)*(n-BALL_COLOR_MIN)/
			 (lmax*(BALL_COLOR_MAX-BALL_COLOR_MIN)) );

		Color[0][n][0] = 1.0*ratio;
		Color[0][n][1] = 0.5*ratio;
		Color[0][n][2] = 0.0*ratio;
    }

  for( n=BALL_COLOR_MAX+1; n<COLOR_NUMBER; n++ )
	 Color[0][n][0] = Color[0][n][1] = Color[0][n][2] = 1.;
}

void OpenglGfx::restoreColormap() {
DOTRACE("OpenglGfx::restoreColormap");
//   define_color_table( fildes(), 0, COLOR_NUMBER, Color[2] );
}

static const char vcid_openglgfx_cc[] = "$Header$";
#endif // !OPENGLGFX_CC_DEFINED
