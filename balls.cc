///////////////////////////////////////////////////////////////////////
//
// balls.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:12:25 2000
// written: Mon Jun 12 14:13:53 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_C_DEFINED
#define BALLS_C_DEFINED

#include "balls.h"

#include <cmath>
#include <cstdlib>
#include <vector>

#include "defs.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

namespace {
  const int VELOSCALE = 1000;

  vector<unsigned char> theirBallmap;
  vector<unsigned char> theirHimap;
}

namespace Local {
  int roundVelocity( int x );
  int fround( float x );
  int abs( int a );

  void makeBallMap( vector<unsigned char>& vec, int size,
						  float radius, float sigma,
						  unsigned char background,
						  bool rgba );

  bool colorsAlreadyGenerated = false;
}

///////////////////////////////////////////////////////////////////////
//
// Local function definitions
//
///////////////////////////////////////////////////////////////////////

int Local::roundVelocity( int x ) {
DOTRACE("Local::roundVelocity");

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

void Local::makeBallMap( vector<unsigned char>& vec, int size,
								 float radius, float sigma,
								 unsigned char background,
								 bool rgba ) {
DOTRACE("Local::makeBallMap");

  Balls::generateColors(); 

  int bytes_per_pixel = rgba ? 4 : 1;

  int num_bytes = size*size*bytes_per_pixel;

  vec.resize(num_bytes);

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

		size_t base_loc = bytes_per_pixel*(i*size + j);

		if (rgba) {
		  vec[base_loc + 0] = (unsigned char)(0xff * Balls::theColors[index][0]);
		  vec[base_loc + 1] = (unsigned char)(0xff * Balls::theColors[index][1]);
		  vec[base_loc + 2] = (unsigned char)(0xff * Balls::theColors[index][2]);
		  vec[base_loc + 3] = (unsigned char)(0xff);
		}
		else {
		  vec[base_loc] = index;
		}
	 }
  }
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

  itsNx = itsXpos + Local::roundVelocity( itsXvel );
  itsNy = itsYpos + Local::roundVelocity( itsYvel );

  if( itsNx < BORDER_X || itsNx > width - BORDER_X - BALL_ARRAY_SIZE )
	 {
		itsXvel = -itsXvel;
		itsNx   = itsXpos + Local::roundVelocity( itsXvel );
	 }

  if( itsNy < BORDER_Y || itsNy > height - BORDER_Y - BALL_ARRAY_SIZE )
	 {
		itsYvel = -itsYvel;
		itsNy   = itsYpos + Local::roundVelocity( itsYvel );
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

  itsNx = itsXpos + Local::roundVelocity( itsXvel );
  itsNy = itsYpos + Local::roundVelocity( itsYvel );

  other.itsNx = other.itsXpos + Local::roundVelocity( other.itsXvel );
  other.itsNy = other.itsYpos + Local::roundVelocity( other.itsYvel );
		
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

  if (gfx->isRgba()) 
	 gfx->writeTrueColorMap(bitmap, itsXpos, itsYpos, BALL_ARRAY_SIZE);
  else
	 gfx->writeBitmap(bitmap, itsXpos, itsYpos, BALL_ARRAY_SIZE);
}

///////////////////////////////////////////////////////////////////////
//
// Balls member definitions
//
///////////////////////////////////////////////////////////////////////

const int Balls::MAX_BALLS;
const int Balls::COLOR_NUMBER;
float Balls::theColors[Balls::COLOR_NUMBER][3];

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

  if (gfx->isDoubleBuffered()) {

	 gfx->clearBackBuffer();

	 for(int i = 0; i < BALL_NUMBER; ++i) {
		itsBalls[i].draw(gfx, &theirBallmap[0]);
	 }
  }
  else {
	 for(int i = 0; i < BALL_NUMBER; ++i) {
		itsBalls[i].move(gfx);
	 }
  }

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
							 BALL_RADIUS, BALL_SIGMA2, 255, gfx->isRgba() );
  Local::makeBallMap( theirBallmap, BALL_ARRAY_SIZE,
							 BALL_RADIUS, BALL_SIGMA2, 0, gfx->isRgba() );
}

void Balls::generateColors() {
DOTRACE("Balls::generateColors");

  if (Local::colorsAlreadyGenerated) return;

  float lmin =   0.;
  float lmax = 196.;

  int n;
  float ratio;

  for( n=0; n<BALL_COLOR_MIN; n++ )
	 theColors[n][0] = theColors[n][1] = theColors[n][2] = 0.;

  for( n=BALL_COLOR_MIN; n<=BALL_COLOR_MAX; n++ )
    {
		ratio = lmin/lmax +
		  ( (lmax-lmin)*(n-BALL_COLOR_MIN)/
			 (lmax*(BALL_COLOR_MAX-BALL_COLOR_MIN)) );

		theColors[n][0] = 1.0*ratio; DebugEval(theColors[n][0]);
		theColors[n][1] = 0.5*ratio; DebugEval(theColors[n][1]);
		theColors[n][2] = 0.0*ratio; DebugEvalNL(theColors[n][2]);
    }

  for( n=BALL_COLOR_MAX+1; n<COLOR_NUMBER; n++ )
	 theColors[n][0] = theColors[n][1] = theColors[n][2] = 1.;

  Local::colorsAlreadyGenerated = true;
}

void Balls::runTrial(Graphics* gfx, timeval* starttime, TrialType ttype) {
DOTRACE("Balls::runTrial");

  Timing::mainTimer.set();

  Timing::getTime( starttime );

  gfx->clearUpperPlanes();

  gfx->clearBackBuffer();

  if (ttype == Balls::PASSIVE) {
	 gfx->drawMessage( "PASSIVE" );
  }
  else if (ttype == Balls::CHECK_ALL) {
	 gfx->drawMessage( " ALL" );	 
  }
  else if (ttype == Balls::CHECK_ONE) {
	 gfx->drawMessage( "TRACK" );
  }

  gfx->swapBuffers();

  prepare(gfx);

  Timing::mainTimer.wait( PAUSE_DURATION );

  // Show the initial position of the balls
  Timing::mainTimer.set();

  gfx->clearUpperPlanes();
  gfx->drawCross();

  drawNBalls(gfx, 0, BALL_NUMBER, &theirBallmap[0]);
 
  if (ttype == Balls::CHECK_ALL ||
		ttype == Balls::CHECK_ONE) {
	 drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, &theirHimap[0]);
	 gfx->drawCross();
  }

  gfx->swapBuffers();

  Timing::mainTimer.wait( REMIND_DURATION );

  if (ttype == Balls::CHECK_ALL ||
		ttype == Balls::CHECK_ONE) {
 	 gfx->clearUpperPlanes();
 	 gfx->drawCross();
	 drawNBalls(gfx, 0, BALL_NUMBER, &theirBallmap[0]);
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

		  gfx->drawCross();
		  drawNBalls(gfx, 0, BALL_NUMBER, &theirBallmap[0]);
		  drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, &theirHimap[0]);

		  gfx->swapBuffers();

		  Timing::addToStimulusStack(LEFTBUTTON);

		  Timing::mainTimer.wait( REMIND_DURATION );
		}
		else if (ttype == Balls::CHECK_ONE) {
		  // Randomly choose whether the highlighted ball will be a
		  // target or a non-target
 		  bool pick_target = ( drand48() > 0.5 );

		  // Pick a random ball
		  int random_ball;
		  if (pick_target)
			 random_ball = int( BALL_TRACK_NUMBER * drand48() );
		  else
			 random_ball = int( (BALL_NUMBER - BALL_TRACK_NUMBER) * drand48() )
				+ BALL_TRACK_NUMBER;

		  // Redraw the balls with the random ball highlighted
		  gfx->waitVerticalRetrace();
		  gfx->clearBackBuffer();

		  gfx->drawCross();
		  drawNBalls(gfx, 0, BALL_NUMBER, &theirBallmap[0]);
		  drawNHiBalls(gfx, random_ball, random_ball+1, &theirHimap[0]);

		  gfx->swapBuffers();

		  // Note what the correct response should be for the random ball
		  if (random_ball < BALL_TRACK_NUMBER)
			 Timing::addToStimulusStack(LEFTBUTTON);
		  else
			 Timing::addToStimulusStack(MIDDLEBUTTON);

		  Timing::mainTimer.wait( REMIND_DURATION / 2.0 );

		  Timing::mainTimer.set();

		  // Redraw the balls, but now with the correct balls
		  // highlighted in order to cue the next trial
		  gfx->waitVerticalRetrace();
		  gfx->clearBackBuffer();

		  gfx->drawCross();
		  drawNHiBalls(gfx, random_ball, random_ball+1, &theirBallmap[0]);
		  drawNBalls(gfx, BALL_TRACK_NUMBER, BALL_NUMBER, &theirBallmap[0]);
		  drawNHiBalls(gfx, 0, BALL_TRACK_NUMBER, &theirHimap[0]);

		  gfx->swapBuffers();

		  Timing::mainTimer.wait( REMIND_DURATION / 2.0 );
		}
		else {
		  Timing::mainTimer.wait( REMIND_DURATION );
		}

		gfx->clearUpperPlanes();
		gfx->drawCross();

		drawNBalls(gfx, 0, BALL_NUMBER, &theirBallmap[0]);
    }

  gfx->writeAllPlanes();
  gfx->clearFrontBuffer();
  gfx->clearBackBuffer();
  gfx->drawCross();
  gfx->swapBuffers();
}

static const char vcid_balls_c[] = "$Header$";
#endif // !BALLS_C_DEFINED
