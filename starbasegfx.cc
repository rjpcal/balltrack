///////////////////////////////////////////////////////////////////////
//
// starbasegfx.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:55:42 2000
// written: Tue Feb 29 14:53:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STARBASEGFX_CC_DEFINED
#define STARBASEGFX_CC_DEFINED

#include "starbasegfx.h"

#include <starbase.c.h>
#include <cmath>
#include <cstdlib>				  // for exit()
#include <X11/keysym.h>

#include "applic.h"
#include "params.h"
#include "timing.h"
#include "xstuff.h"

#include "trace.h"
#include "debug.h"

#define CMAP_NUMBER  3
#define COLOR_NUMBER 256
float Color[CMAP_NUMBER][COLOR_NUMBER][3];

///////////////////////////////////////////////////////////////////////
//
// StarbaseGfx member definitions
//
///////////////////////////////////////////////////////////////////////

StarbaseGfx::StarbaseGfx(XStuff* xinfo, int aWidth, int aHeight) :
  Graphics(aWidth, aHeight),
  itsXStuff(xinfo),
  itsFildes(-1)
{
DOTRACE("StarbaseGfx::StarbaseGfx");
}

void StarbaseGfx::initWindow() {
DOTRACE("StarbaseGfx::initWindow");

  char* device = ( char * ) make_X11_gopen_string( itsXStuff->display(),
																	itsXStuff->window() );
  itsFildes = gopen( device, OUTDEV, NULL, INT_XFORM|CMAP_FULL );

  gescape_arg arg1, arg2;

  arg1.i[0] = TRUE;
  gescape( itsFildes, READ_COLOR_MAP, &arg1, &arg2 );

  if ( itsFildes < 0 )
    {
		fprintf( stdout,"Could not gopen window.\n" );
		exit( -1 );
    }

  sizeColormap();

  saveColormap();

  newColormap();

  intvdc_extent( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  intview_port( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  intview_window( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  setTransparent();

  checkFrameTime();

  clearFrontBuffer();
}

void StarbaseGfx::wrapGraphics() {
DOTRACE("StarbaseGfx::wrapGraphics");
  restoreColormap();

  gclose( itsFildes );
}

void StarbaseGfx::writeUpperPlanes() {
DOTRACE("StarbaseGfx::writeUpperPlanes");
  write_enable(fildes(), 0xc0);	// 11000000
}

void StarbaseGfx::writeLowerPlanes() {
DOTRACE("StarbaseGfx::writeLowerPlanes");
  write_enable(fildes(), 0x3f);	// 00111111
}

void StarbaseGfx::writeAllPlanes() {
DOTRACE("StarbaseGfx::writeAllPlanes");
  write_enable(fildes(), 0xff); // 11111111
}

void StarbaseGfx::waitVerticalRetrace() {
DOTRACE("StarbaseGfx::waitVerticalRetrace");
  await_retrace( fildes() );
}

void StarbaseGfx::swapBuffers() {
DOTRACE("StarbaseGfx::swapBuffers");
  /* do nothing */;
}

void StarbaseGfx::waitFrameCount(int number) {
DOTRACE("StarbaseGfx::waitFrameCount");
  while( number-- )
	 waitVerticalRetrace();
}

void StarbaseGfx::drawMessage(char word[]) {
DOTRACE("StarbaseGfx::drawMessage");

  writeUpperPlanes();

  setText(200, 250);

  dctext( fildes(), width()/2 - 500, height()/2+100, word );

  writeLowerPlanes();
}

void StarbaseGfx::drawCross() {
DOTRACE("StarbaseGfx::drawCross");

  writeUpperPlanes();

  line_color_index( fildes(), 192 );
  dcmove( fildes(), width()/2-50, height()/2 );
  dcdraw( fildes(), width()/2+50, height()/2 );
  dcmove( fildes(), width()/2, height()/2-50 );
  dcdraw( fildes(), width()/2, height()/2+50 );

  writeLowerPlanes();
}

void StarbaseGfx::clearUpperPlanes() {
DOTRACE("StarbaseGfx::clearUpperPlanes");

  writeUpperPlanes();

  clear_view_surface( fildes() );

  writeLowerPlanes();
}

void StarbaseGfx::setOpaque() {
DOTRACE("StarbaseGfx::setOpaque");

  waitVerticalRetrace();
  define_color_table( fildes(), 0, COLOR_NUMBER, Color[1] );
}

void StarbaseGfx::setTransparent() {
DOTRACE("StarbaseGfx::setTransparent");

  waitVerticalRetrace();
  define_color_table( fildes(), 0, COLOR_NUMBER, Color[0] );
}

void StarbaseGfx::clearFrontBuffer() {
DOTRACE("StarbaseGfx::clearFrontBuffer");

  background_color_index( fildes(), 0 );
  clear_view_surface( fildes() );
}

void StarbaseGfx::clearBackBuffer() {
DOTRACE("StarbaseGfx::clearBackBuffer");
  /* do nothing */;
}

void StarbaseGfx::showMenu(char menu[][STRINGSIZE], int nmenu) {
DOTRACE("StarbaseGfx::showMenu");
  int i;

  setText(10, 15);

  for( i=0; i<nmenu; i++ )
    {
		dctext( fildes(),
				  width()/2 - 400,
				  height()/2 - 200 + i * 40,
				  menu[i] );
    }
}

void StarbaseGfx::showParams(char params[][STRINGSIZE], int nparams) {
DOTRACE("StarbaseGfx::showParams");

  int col1 = (nparams < 23) ? nparams : 23;
  int col2 = (nparams < 23) ? 0       : nparams;

  setText(10, 15);

  for( int i=0; i<col1; i++ )
    {
		dctext( fildes(),
				  width()/2 - 500,
				  height()/2 -  450 + i * 40,
				  params[i] );
    }

  if( col1+1 < col2 ) {
    for( int i=col1+1; i<col2; i++ )
		{
        dctext( fildes(),
					 width()/2 + 100,
					 height()/2 - 1370 + i * 40,
					 params[i] );
		}
  }
}

void StarbaseGfx::setText(int wd, int ht) {
DOTRACE("StarbaseGfx::setText");

  define_color_table( fildes(), 0, 256, Color[0] );
  background_color_index( fildes(), 0 );
  text_color_index( fildes(), 192 );
  dccharacter_height( fildes(), ht );
  dccharacter_width( fildes(), wd );
}

void StarbaseGfx::moveBlock(int x, int y, int xsz, int ysz, int nx, int ny) {
DOTRACE("StarbaseGfx::moveBlock");
  dcblock_move(fildes(), x, y, xsz, ysz, nx, ny);
}

void StarbaseGfx::writeBitmap(unsigned char* ptr, int x, int y, int size) {
DOTRACE("StarbaseGfx::writeBitmap");
  dcblock_write(fildes(), x, y, size, size, ptr, FALSE);
}

void StarbaseGfx::writeTrueColorMap(unsigned char* ptr, int x, int y, int size) {
DOTRACE("StarbaseGfx::writeTrueColorMap");
}


///////////////////////////////////////////////////////////////////////
//
// StarbaseGfx private member definitions
//
///////////////////////////////////////////////////////////////////////

void StarbaseGfx::checkFrameTime() {
DOTRACE("StarbaseGfx::checkFrameTime");

  struct timeval tp[2];

  clearFrontBuffer();

  waitFrameCount( 1 );

  Timing::getTime( &tp[0] );

  waitFrameCount( 99 );

  Timing::getTime( &tp[1] );

  FRAMETIME = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  printf( " Video frame time %7.4lf ms\n", FRAMETIME );  
}

void StarbaseGfx::sizeColormap() {
DOTRACE("StarbaseGfx::sizeColormap");

  float dummy[2][3];
  int color_number;

  inquire_sizes( fildes(), dummy, dummy[0],
					  dummy[0], dummy[0], &color_number);

  if( color_number != COLOR_NUMBER )
    {
		printf( " Colormap is of size %d instead of %d\n",
				  color_number, COLOR_NUMBER );

		exit(0);
    }
}

void StarbaseGfx::saveColormap() {
DOTRACE("StarbaseGfx::saveColormap");
  inquire_color_table( fildes(), 0, COLOR_NUMBER, Color[2] );
}

void StarbaseGfx::newColormap() {
DOTRACE("StarbaseGfx::newColormap");

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

void StarbaseGfx::restoreColormap() {
DOTRACE("StarbaseGfx::restoreColormap");
  define_color_table( fildes(), 0, COLOR_NUMBER, Color[2] );
}

static const char vcid_starbasegfx_cc[] = "$Header$";
#endif // !STARBASEGFX_CC_DEFINED
