///////////////////////////////////////////////////////////////////////
//
// starbasegfx.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:55:42 2000
// written: Mon Jun 12 14:22:21 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STARBASEGFX_CC_DEFINED
#define STARBASEGFX_CC_DEFINED

#include "starbasegfx.h"

#include <starbase.c.h>
#include <cstdlib>				  // for exit()

#include "params.h"
#include "timing.h"
#include "xstuff.h"

#include "trace.h"
#include "debug.h"

namespace {
  const int COLOR_NUMBER = 256;
  float ORIGINAL_COLORS[COLOR_NUMBER][3];
}

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

  intvdc_extent( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  intview_port( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  intview_window( fildes(), 0, 0, 100*(width()-1), 100*(height()-1) );

  checkFrameTime();

  clearFrontBuffer();
}

void StarbaseGfx::wrapGraphics() {
DOTRACE("StarbaseGfx::wrapGraphics");
  restoreColormap();

  gclose( itsFildes );
}

bool StarbaseGfx::isRgba() {
DOTRACE("StarbaseGfx::isRgba");
  return false;
}

bool StarbaseGfx::isDoubleBuffered() {
DOTRACE("StarbaseGfx::isDoubleBuffered");
  return false;
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


  if (APPLICATION_MODE == FMRI_SESSION)
	 {
		// Use mirror-reversed text if we are in magnet
		setText(-150, 300);
		dctext( fildes(), width()/2 + 500, height()/2+100, word );
	 }
  else
	 {
		setText(150, 300);
		dctext( fildes(), width()/2 - 500, height()/2+100, word );
	 }

  writeLowerPlanes();
}

void StarbaseGfx::drawCross() {
DOTRACE("StarbaseGfx::drawCross");
  drawCross( width()/2, height()/2 );
}

void StarbaseGfx::drawCross(int x, int y) {
DOTRACE("StarbaseGfx::drawCross(int, int)");

  writeUpperPlanes();

  line_color_index( fildes(), 192 );
  dcmove( fildes(), x-50, y );
  dcdraw( fildes(), x+50, y );
  dcmove( fildes(), x, y-50 );
  dcdraw( fildes(), x, y+50 );

  writeLowerPlanes();
}

void StarbaseGfx::clearUpperPlanes() {
DOTRACE("StarbaseGfx::clearUpperPlanes");

  writeUpperPlanes();

  clear_view_surface( fildes() );

  writeLowerPlanes();
}

void StarbaseGfx::loadColormap(float colors[][3], int ncolors) {
DOTRACE("StarbaseGfx::loadColormap");

  waitVerticalRetrace();
  define_color_table( fildes(), 0, ncolors, colors );
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
  inquire_color_table( fildes(), 0, COLOR_NUMBER, ORIGINAL_COLORS );
}

void StarbaseGfx::restoreColormap() {
DOTRACE("StarbaseGfx::restoreColormap");
  define_color_table( fildes(), 0, COLOR_NUMBER, ORIGINAL_COLORS );
}

static const char vcid_starbasegfx_cc[] = "$Header$";
#endif // !STARBASEGFX_CC_DEFINED
