///////////////////////////////////////////////////////////////////////
//
// balls.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:12:25 2000
// written: Wed Feb 23 15:50:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_C_DEFINED
#define BALLS_C_DEFINED

#include "balls.h"

#include <cmath>
#include <cstdlib>

#include "application.h"
#include "defs.h"
#include "image.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

const int VELOSCALE = 1000;

namespace {
  // XXX These need to be big enough for BALL_ARRAY_SIZE^2
  unsigned char theirBallmap[128*128];
  unsigned char theirHimap[128*128];
}

// unsigned char theirBallmap[];
// unsigned char theirHimap[];

namespace Local {
  int round( int x );
  int fround( float x );
  int abs( int a );
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

void Ball::move(int fildes) {
DOTRACE("Ball::move");

  MoveBlock(fildes, itsXpos, itsYpos,
				BALL_ARRAY_SIZE, BALL_ARRAY_SIZE,
				itsNx, itsNy);
}

void Ball::copy() {
DOTRACE("Ball::copy");

  itsXpos = itsNx;
  itsYpos = itsNy;
}

void Ball::draw(int fildes, unsigned char* bitmap) {
DOTRACE("Ball::draw");

  WriteBitmap(fildes, bitmap, itsXpos, itsYpos, BALL_ARRAY_SIZE);
}

///////////////////////////////////////////////////////////////////////
//
// Balls member definitions
//
///////////////////////////////////////////////////////////////////////

const int Balls::MAX_BALLS;

void Balls::initialize(Application* app) {
DOTRACE("Balls::initialize");

  for( int i=0; i<BALL_NUMBER; i++ ) {

	 // Pick a random initial location that is not too close to the other balls
	 bool too_close;

	 do {
		too_close = false;

		itsBalls[i].randomPosition(app->width(), app->height());

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

void Balls::nextBalls(Application* app) {
DOTRACE("Balls::nextBalls");

  {for( int i=0; i<BALL_NUMBER; i++ ) {
		itsBalls[i].nextPosition(app->width(), app->height());
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

void Balls::moveBalls(Application* app) {
DOTRACE("Balls::moveBalls");
  Graphics::waitVerticalRetrace(app->fildes());

  for(int i = 0; i < BALL_NUMBER; ++i) {
	 itsBalls[i].move(app->fildes());
  }
}

void Balls::copyBalls() {
DOTRACE("Balls::copyBalls");

  for( int i=0; i<BALL_NUMBER; i++ ) {
	 itsBalls[i].copy();
  }
}

void Balls::drawNBalls(int n, int fildes, unsigned char* bitmap) {
DOTRACE("Balls::drawNBalls");

  for (int i = 0; i < n; ++i)
	 itsBalls[i].draw(fildes, bitmap);
}

void Balls::drawNHiBalls(int n, int fildes, unsigned char* bitmap) {
DOTRACE("Balls::drawNHiBalls");
  Graphics::writeUpperPlanes(fildes);

  for( int i=0 ; i<n; i++ )
	 itsBalls[i].draw(fildes, bitmap);

  Graphics::writeLowerPlanes(fildes);
}

void Balls::prepare(Application* app) {
DOTRACE("Balls::prepare");

  initialize(app);

  MakeWhiteMap( theirHimap, BALL_ARRAY_SIZE, BALL_RADIUS, BALL_SIGMA2 );
  MakeBallMap( theirBallmap, BALL_ARRAY_SIZE, BALL_RADIUS, BALL_SIGMA2 );
}

void Balls::runTrial(Application* app, bool cue_track_balls) {
DOTRACE("Balls::runTrial");

  drawNBalls(BALL_NUMBER, app->fildes(), theirBallmap);
 
  if (cue_track_balls) {
	 drawNHiBalls(BALL_TRACK_NUMBER, app->fildes(), theirHimap);
  }

  Timing::mainTimer.set();
  Timing::mainTimer.wait( REMIND_DURATION );

  if (cue_track_balls) {
	 ClearUpperPlanes(app->fildes());
	 DrawCross(app);
  }

  for( int i=0; i<REMINDS_PER_EPOCH; i++ )
    {
		for( int j=0; j<FRAMES_PER_REMIND; j++ )
		  {
			 nextBalls(app);
			 moveBalls(app);
			 copyBalls();
		  }

		Timing::mainTimer.set();

		if (cue_track_balls) {
		  drawNHiBalls(BALL_TRACK_NUMBER, app->fildes(), theirHimap);

		  Timing::addToStimulusStack(app);
		}

		Timing::mainTimer.wait( REMIND_DURATION );

		ClearUpperPlanes(app->fildes());
		DrawCross(app);

		drawNBalls(BALL_TRACK_NUMBER, app->fildes(), theirBallmap);
    }

  Graphics::writeAllPlanes(app->fildes());
  ClearWindow(app->fildes());
  DrawCross(app);
}

static const char vcid_balls_c[] = "$Header$";
#endif // !BALLS_C_DEFINED
