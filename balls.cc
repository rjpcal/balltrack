///////////////////////////////////////////////////////////////////////
//
// balls.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:12:25 2000
// written: Tue Feb 29 10:25:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_C_DEFINED
#define BALLS_C_DEFINED

#include "balls.h"

#include <cmath>
#include <cstdlib>

#include "defs.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

const int VELOSCALE = 1000;

#if defined(COLOR_INDEX)
const int BYTES_PER_PIXEL = 1;
#elif defined(RGBA)
const int BYTES_PER_PIXEL = 4;
#endif

namespace {
  // XXX These need to be big enough for BALL_ARRAY_SIZE^2
  unsigned char theirBallmap[128*128*BYTES_PER_PIXEL];
  unsigned char theirHimap[128*128*BYTES_PER_PIXEL];
}

namespace Local {
  int round( int x );
  int fround( float x );
  int abs( int a );

  void makeBallMap( unsigned char* ptr, int size,
						  float radius, float sigma,
						  unsigned char background);

  const int COLOR_NUMBER = 256;

  float Colors[COLOR_NUMBER][3];

  void generateColors();
}

struct timeval tp[2];

///////////////////////////////////////////////////////////////////////
//
// Local function definitions
//
///////////////////////////////////////////////////////////////////////

int Local::round( int x ) {
DOTRACE("Local::round");

  int ix = (x < 0) ? ( x - VELOSCALE/2 ) / VELOSCALE
	                : ( x + VELOSCALE/2 ) / VELOSCALE;

  return( ix );
}

int Local::fround( float x ) {
DOTRACE("Local::fround");

  int ix = (x < 0) ? (int)( x - 0.5 )
	                : (int)( x + 0.5 );

  return( ix );
}

int Local::abs( int a ) {
DOTRACE("Local::abs");

  return( ( a > 0 ) ? a : -a );
}

void Local::makeBallMap( unsigned char* ptr, int size,
								 float radius, float sigma,
								 unsigned char background ) {
DOTRACE("Local::makeBallMap");

  generateColors(); 

  for( int i=0; i<size; i++ ) {
    for( int j=0; j<size; j++ ) {
		float x   = (float)( i - size/2 + 0.5 );
		float y   = (float)( j - size/2 + 0.5 );

		float rsq = x*x + y*y;

		unsigned char index;

		if( x*x+y*y < radius*radius ) {
		  index = 
			 (unsigned char)
			 ( BALL_COLOR_MIN
				+ ( BALL_COLOR_MAX - BALL_COLOR_MIN ) * exp(-rsq/sigma ));
		}
		else {
		  index = (unsigned char)( background );
		}

#if defined(COLOR_INDEX)
		*ptr++ = index;
#elif defined(RGBA)
		ptr[0] = (unsigned char)(255 * Colors[index][0]); // Red
		ptr[1] = (unsigned char)(255 * Colors[index][1]); // Green
		ptr[2] = (unsigned char)(255 * Colors[index][2]); // Blue
		ptr[3] = (unsigned char)(255); // Alpha
		ptr += 4;
#endif

	 }
  }
}

void Local::generateColors() {
DOTRACE("Local::generateColors");
  float lmin =   0.;
  float lmax = 196.;

  int n;
  float ratio;

  for( n=0; n<BALL_COLOR_MIN; n++ )
	 Colors[n][0] = Colors[n][1] = Colors[n][2] = 0.;

  for( n=BALL_COLOR_MIN; n<=BALL_COLOR_MAX; n++ )
    {
		ratio = lmin/lmax +
		  ( (lmax-lmin)*(n-BALL_COLOR_MIN)/
			 (lmax*(BALL_COLOR_MAX-BALL_COLOR_MIN)) );

		Colors[n][0] = 1.0*ratio;
		Colors[n][1] = 0.5*ratio;
		Colors[n][2] = 0.0*ratio;
    }

  for( n=BALL_COLOR_MAX+1; n<COLOR_NUMBER; n++ )
	 Colors[n][0] = Colors[n][1] = Colors[n][2] = 1.;
}

///////////////////////////////////////////////////////////////////////
//
// Ball member definitions
//
///////////////////////////////////////////////////////////////////////

void Ball::randomPosition(int width, int height) {
DOTRACE("Ball::randomPosition");

  itsXpos = 
	 BORDER_X + int( ( width - BALL_ARRAY_SIZE - 2*BORDER_X ) * drand48() );
  itsYpos = 
	 BORDER_Y +int( ( height - BALL_ARRAY_SIZE - 2*BORDER_Y ) * drand48() );
}

bool Ball::isTooClose(const Ball& other) const {
DOTRACE("Ball::isTooClose");

  return ( Local::abs(itsXpos - other.itsXpos) < BALL_MIN_DISTANCE &&
			  Local::abs(itsYpos - other.itsYpos) < BALL_MIN_DISTANCE );
}

void Ball::randomVelocity() {
DOTRACE("Ball::randomVelocity");

  // Pick a random direction for the velocity
  float angle = TWOPI * drand48();

  itsXvel = (int)( VELOSCALE * BALL_VELOCITY * cos(angle) );
  itsYvel = (int)( VELOSCALE * BALL_VELOCITY * sin(angle) );
}

void Ball::nextPosition(int width, int height) {
DOTRACE("Ball::nextPosition");

  itsNx = itsXpos + Local::round( itsXvel );
  itsNy = itsYpos + Local::round( itsYvel );

  if( itsNx < BORDER_X || itsNx > width - BORDER_X - BALL_ARRAY_SIZE )
	 {
		itsXvel = -itsXvel;
		itsNx   = itsXpos + Local::round( itsXvel );
	 }

  if( itsNy < BORDER_Y || itsNy > height - BORDER_Y - BALL_ARRAY_SIZE )
	 {
		itsYvel = -itsYvel;
		itsNy   = itsYpos + Local::round( itsYvel );
	 }
}

void Ball::collideIfNeeded(Ball& other) {
DOTRACE("Ball::collideIfNeeded");

  int dx = itsNx - other.itsNx;
  int dy = itsNy - other.itsNy;

  if( Local::abs( dx ) < BALL_MIN_DISTANCE &&
		Local::abs( dy ) < BALL_MIN_DISTANCE ) {

	 collide(other, dx, dy);
  }
}

void Ball::collide(Ball& other, int xij, int yij) {
DOTRACE("Ball::collide");

  float d    =  sqrt( (double) xij*xij + yij*yij );
  float xa   =  xij/d;
  float ya   =  yij/d;
  float xo   = -ya;
  float yo   =  xa;

  float vai  = (itsXvel)*(xa) + (itsYvel)*(ya);
  float vaj  = (other.itsXvel)*(xa) + (other.itsYvel)*(ya);

  if( vai - vaj < 0. )
	 {
		float voi  = (itsXvel)*(xo) + (itsYvel)*(yo);
		float voj  = (other.itsXvel)*(xo) + (other.itsYvel)*(yo);
		/*
		  ovi2 = vai*vai + voi*voi;
		  ovj2 = vaj*vaj + voj*voj;
		*/
		float nvi2 = vaj*vaj + voi*voi;
		float nvj2 = vai*vai + voj*voj;

		float vij2 = vai*vai - vaj*vaj;

		float fi   = sqrt( 1. + vij2 / nvi2 );
		float fj   = sqrt( 1. - vij2 / nvj2 );

		itsXvel = Local::fround( fi * ( voi * xo + vaj * xa ) );
		itsYvel = Local::fround( fi * ( voi * yo + vaj * ya ) );

		other.itsXvel = Local::fround( fj * ( voj * xo + vai * xa ) );
		other.itsYvel = Local::fround( fj * ( voj * yo + vai * ya ) );
	 }

  itsNx = itsXpos + Local::round( itsXvel );
  itsNy = itsYpos + Local::round( itsYvel );

  other.itsNx = other.itsXpos + Local::round( other.itsXvel );
  other.itsNy = other.itsYpos + Local::round( other.itsYvel );
		
}

void Ball::twist() {
DOTRACE("Ball::twist");

  static float a11  =  cos( BALL_TWIST_ANGLE );
  static float a12  =  sin( BALL_TWIST_ANGLE );
  static float a21  = -sin( BALL_TWIST_ANGLE );
  static float a22  =  cos( BALL_TWIST_ANGLE );

  int x = itsXvel;
  int y = itsYvel;

  if( drand48() < 0.5 )
	 {
		itsXvel = Local::fround( a11*x + a12*y );
		itsYvel = Local::fround( a21*x + a22*y );
	 }
  else
	 {
		itsXvel = Local::fround( a11*x - a12*y );
		itsYvel = Local::fround(-a21*x + a22*y );
	 }
}

void Ball::move(Graphics* gfx) {
DOTRACE("Ball::move");

  gfx->moveBlock(itsXpos, itsYpos,
					  BALL_ARRAY_SIZE, BALL_ARRAY_SIZE,
					  itsNx, itsNy);
}

void Ball::copy() {
DOTRACE("Ball::copy");

  itsXpos = itsNx;
  itsYpos = itsNy;
}

void Ball::draw(Graphics* gfx, unsigned char* bitmap) {
DOTRACE("Ball::draw");

#if defined(COLOR_INDEX)
  gfx->writeBitmap(bitmap, itsXpos, itsYpos, BALL_ARRAY_SIZE);
#elif defined(RGBA)
  gfx->writeTrueColorMap(bitmap, itsXpos, itsYpos, BALL_ARRAY_SIZE);
#endif
}

///////////////////////////////////////////////////////////////////////
//
// Balls member definitions
//
///////////////////////////////////////////////////////////////////////

const int Balls::MAX_BALLS;

void Balls::initialize(Graphics* gfx) {
DOTRACE("Balls::initialize");

  for( int i=0; i<BALL_NUMBER; i++ ) {

	 // Pick a random initial location that is not too close to the other balls
	 bool too_close;

	 do {
		too_close = false;

		itsBalls[i].randomPosition(gfx->width(), gfx->height());

		for( int j=0; j<i; j++ ) {
		  if( itsBalls[i].isTooClose(itsBalls[j]) )
			 {
				too_close = true;
				break;
			 }
		}
	 } while( too_close );

	 itsBalls[i].randomVelocity();
  }

}

void Balls::nextBalls(Graphics* gfx) {
DOTRACE("Balls::nextBalls");

  {for( int i=0; i<BALL_NUMBER; i++ ) {
		itsBalls[i].nextPosition(gfx->width(), gfx->height());
  }}

  {for( int i=0; i<BALL_NUMBER-1; i++ ) {
		for( int j=i+1; j<BALL_NUMBER; j++ ) {
		  itsBalls[i].collideIfNeeded(itsBalls[j]);
		}
  }}

  {for( int i=0; i<BALL_NUMBER; i++ ) {
		itsBalls[i].twist();
  }}
}

void Balls::moveBalls(Graphics* gfx) {
DOTRACE("Balls::moveBalls");
  gfx->waitVerticalRetrace();

  gfx->clearBackBuffer();

#if defined(IRIX6)
  for(int i = 0; i < BALL_NUMBER; ++i) {
	 itsBalls[i].draw(gfx, theirBallmap);
  }
#elif defined(HP9000S700)
  for(int i = 0; i < BALL_NUMBER; ++i) {
	 itsBalls[i].move(gfx);
  }
#else
#  error No architecture macro.
#endif

  gfx->drawCross();

  gfx->swapBuffers();
}

void Balls::copyBalls() {
DOTRACE("Balls::copyBalls");

  for( int i=0; i<BALL_NUMBER; i++ ) {
	 itsBalls[i].copy();
  }
}

void Balls::drawNBalls(Graphics* gfx, int first, int last,
							  unsigned char* bitmap) {
DOTRACE("Balls::drawNBalls");

  while (first != last) {
	 itsBalls[first].draw(gfx, bitmap);
	 ++first;
  }
}

void Balls::drawNHiBalls(Graphics* gfx, int first, int last,
								 unsigned char* bitmap) {
DOTRACE("Balls::drawNHiBalls");
  gfx->writeUpperPlanes();

  drawNBalls(gfx, first, last, bitmap);

  gfx->writeLowerPlanes();
}

void Balls::prepare(Graphics* gfx) {
DOTRACE("Balls::prepare");

  initialize(gfx);

  Local::makeBallMap( theirHimap, BALL_ARRAY_SIZE,
							 BALL_RADIUS, BALL_SIGMA2, 255 );
  Local::makeBallMap( theirBallmap, BALL_ARRAY_SIZE,
							 BALL_RADIUS, BALL_SIGMA2, 0 );
}

void Balls::runTrial(Graphics* gfx, timeval* starttime, TrialType ttype) {
DOTRACE("Balls::runTrial");

  Timing::mainTimer.set();

  Timing::getTime( starttime );

  gfx->clearUpperPlanes();

  gfx->clearBackBuffer();

  if (ttype == Balls::PASSIVE) {
	 gfx->drawMessage( " STOP" );
  }
  else if (ttype == Balls::CHECK_ALL) {
	 gfx->drawMessage( " ALL" );	 
  }
  else if (ttype == Balls::CHECK_ONE) {
	 gfx->drawMessage( " ONE" );
  }

  gfx->swapBuffers();

  prepare(gfx);

  Timing::mainTimer.wait( PAUSE_DURATION );

  // Show the initial position of the balls
  Timing::mainTimer.set();

  gfx->clearUpperPlanes();
  gfx->drawCross();

  drawNBalls(gfx, 0, BALL_NUMBER, theirBallmap);
 
  if (ttype == Balls::CHECK_ALL ||
		ttype == Balls::CHECK_ONE) {
	 drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, theirHimap);
	 gfx->drawCross();
  }

  gfx->swapBuffers();

  Timing::mainTimer.wait( REMIND_DURATION );

  if (ttype == Balls::CHECK_ALL ||
		ttype == Balls::CHECK_ONE) {
 	 gfx->clearUpperPlanes();
 	 gfx->drawCross();
	 drawNBalls(gfx, 0, BALL_NUMBER, theirBallmap);
	 gfx->swapBuffers();
  }

  for( int i=0; i<REMINDS_PER_EPOCH; i++ )
    {
		for( int j=0; j<FRAMES_PER_REMIND; j++ )
		  {
			 nextBalls(gfx);
			 moveBalls(gfx);
			 copyBalls();
		  }

		Timing::mainTimer.set();

		if (ttype == Balls::CHECK_ALL) {
		  gfx->waitVerticalRetrace();
		  gfx->clearBackBuffer();
		  drawNBalls(gfx, 0, BALL_NUMBER, theirBallmap);
		  drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, theirHimap);

		  gfx->drawCross();
		  gfx->swapBuffers();

		  Timing::addToStimulusStack(LEFTBUTTON);
		}
		else if (ttype == Balls::CHECK_ONE) {
 		  bool pick_target = ( drand48() > 0.5 );

		  int random_ball;
		  if (pick_target)
			 random_ball = int( BALL_TRACK_NUMBER * drand48() );
		  else
			 random_ball = int( (BALL_NUMBER - BALL_TRACK_NUMBER) * drand48() )
				+ BALL_TRACK_NUMBER;

		  drawNBalls(gfx, 0, BALL_NUMBER, theirBallmap);
		  drawNHiBalls(gfx, random_ball, random_ball+1, theirHimap);

		  gfx->swapBuffers();

		  if (random_ball < BALL_TRACK_NUMBER)
			 Timing::addToStimulusStack(LEFTBUTTON);
		  else
			 Timing::addToStimulusStack(MIDDLEBUTTON);

		  Timing::mainTimer.wait( REMIND_DURATION );

		  Timing::mainTimer.set();
		  drawNHiBalls(gfx, random_ball, random_ball+1, theirBallmap);
		  drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, theirHimap);
		  gfx->drawCross();
		  gfx->swapBuffers();
		}

		Timing::mainTimer.wait( REMIND_DURATION );

		gfx->clearUpperPlanes();
		gfx->drawCross();

		drawNBalls(gfx, 0, BALL_NUMBER, theirBallmap);
    }

  gfx->writeAllPlanes();
  gfx->clearWindow();
  gfx->drawCross();
}

static const char vcid_balls_c[] = "$Header$";
#endif // !BALLS_C_DEFINED
