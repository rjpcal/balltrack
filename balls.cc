///////////////////////////////////////////////////////////////////////
//
// balls.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:12:25 2000
// written: Tue Feb 22 15:16:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_C_DEFINED
#define BALLS_C_DEFINED

#include "balls.h"

#include <cmath>
#include <cstdlib>
#include <starbase.c.h>
#include <X11/Xlib.h>

#include "defs.h"
#include "image.h"
#include "main.h"
#include "params.h"
#include "timing.h"

extern Application theApp;

int xpos[25], ypos[25], nx[25], ny[25],
  xvel[25], yvel[25];

struct timeval tp[2];

unsigned char ballmap[128*128], himap[128*128];

// Prototypes
void PrepareTrial();
void RunTrial();
void RunDummy();
void InitBalls();
void NextBalls();
int Round( int x );
int Fround( float x );
void Collide( int xij, int yij, int* vxi, int* vyi, int* vxj, int* vyj );
void Twist( int* vx, int* vy, float angle );
void CopyBalls( int x[], int y[], int nx[], int ny[] );
void WriteBalls( int xpos[], int ypos[], int number, unsigned char* bitmap );
void WriteHiBalls( int xpos[], int ypos[], int number, unsigned char* bitmap );
int Abs( int a );
void SmallLoop();
void TimeButtonEvent( XEvent* event );

void RunApplication()
{
  FILE *fl;
  int cycle, i;
  struct timeval tp[10];

  Openfile( &fl, APPEND, "tme" );

  LogParams( fl );

  WriteAll();
  ClearWindow();
  DrawCross();
  SetTransparent();

  SetTimer();

  GetTime( &tp[0] );

  SetMessage();

  CheckTimer( WAIT_DURATION );

  for( cycle=0; cycle<CYCLE_NUMBER; cycle++ )
    {
		SetTimer();

		GetTime( &tp[2*cycle+1] );

		EraseWords();
		DrawMessage( "KCART" );

		PrepareTrial();

		CheckTimer( PAUSE_DURATION );

		EraseWords();
		DrawCross();

		RunTrial();

		SetTimer();

		GetTime( &tp[2*cycle+2] );

		EraseWords();
		DrawMessage( " POTS " );

		PrepareTrial();

		CheckTimer( PAUSE_DURATION );

		EraseWords();
		DrawCross();

		RunDummy();
    }

  SetTimer();

  GetTime( &tp[2*cycle+1] );

  CheckTimer( WAIT_DURATION );

  GetTime( &tp[2*cycle+2] );

  for( i=0; i<2*cycle+2; i++ )
    {
		printf( " %d %lf\n", i, DeltaTime( &tp[0], &tp[i] ) );
		fprintf( fl, " %d %lf\n", i, DeltaTime( &tp[0], &tp[i] ) );
    }

  WriteAll();

  SmallLoop();

  TallyReactionTime( fl );

  Closefile( fl );
}

void PrepareTrial()
{
  InitBalls();

  MakeWhiteMap( himap, BALL_ARRAY_SIZE, BALL_RADIUS, BALL_SIGMA2 );

  MakeBallMap( ballmap, BALL_ARRAY_SIZE, BALL_RADIUS, BALL_SIGMA2 );
}

void RunTrial()
{
  int i, j;

  WriteBalls( xpos, ypos, BALL_NUMBER, ballmap );

  WriteHiBalls( xpos, ypos, BALL_TRACK_NUMBER, himap );

  SetTimer();

  CheckTimer( REMIND_DURATION );

  EraseWords();
  DrawCross();

  WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );

  for( i=0; i<REMINDS_PER_EPOCH; i++ )
    {
		for( j=0; j<FRAMES_PER_REMIND; j++ )
		  {
			 NextBalls();

			 MoveBalls( xpos, ypos, nx, ny );

			 CopyBalls( xpos, ypos, nx, ny );
		  }

		WriteHiBalls( xpos, ypos, BALL_TRACK_NUMBER, himap );

		AddToStimulusStack();

		SetTimer();

		CheckTimer( REMIND_DURATION );

		EraseWords();
		DrawCross();

		WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );
    }


  WriteAll();
  ClearWindow();
  DrawCross();
}

void RunDummy()
{
  int i, j;

  WriteBalls( xpos, ypos, BALL_NUMBER, ballmap );

  WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );

  SetTimer();

  CheckTimer( REMIND_DURATION );

  WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );

  for( i=0; i<REMINDS_PER_EPOCH; i++ )
    {
		for( j=0; j<FRAMES_PER_REMIND; j++ )
		  {
			 NextBalls();

			 MoveBalls( xpos, ypos, nx, ny );

			 CopyBalls( xpos, ypos, nx, ny );
		  }

		SetTimer();

		WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );

		CheckTimer( REMIND_DURATION );

		WriteBalls( xpos, ypos, BALL_TRACK_NUMBER, ballmap );
    }

  WriteAll();
  ClearWindow();
  DrawCross();
}

void InitBalls()
{
  int i, j, too_close;
  float angle;

  for( i=0; i<BALL_NUMBER; i++ )
    {
		do
		  {
			 too_close = 0;

			 xpos[i] = BORDER_X + (int)( ( theApp.width  - BALL_ARRAY_SIZE - 2 * BORDER_X ) * drand48() );
			 ypos[i] = BORDER_Y + (int)( ( theApp.height - BALL_ARRAY_SIZE - 2 * BORDER_Y ) * drand48() );
			 for( j=0; j<i; j++ )
				{
				  if( Abs( xpos[i]-xpos[j] ) < BALL_MIN_DISTANCE &&
						Abs( ypos[i]-ypos[j] ) < BALL_MIN_DISTANCE )
					 {
						too_close = 1;
					 }
				}
		  }
		while( too_close );
    }

  for( i=0; i<BALL_NUMBER; i++ )
    {
		angle = TWOPI * drand48();

		xvel[i] = (int)( VELOSCALE * BALL_VELOCITY * cos(angle) );
		yvel[i] = (int)( VELOSCALE * BALL_VELOCITY * sin(angle) );
    }
}

void NextBalls()
{
  int i, j, tmp, dx, dy;

  for( i=0; i<BALL_NUMBER; i++ )
    {
		nx[i] = xpos[i] + Round( xvel[i] );
		ny[i] = ypos[i] + Round( yvel[i] );

		if( nx[i] < BORDER_X || nx[i] > theApp.width - BORDER_X - BALL_ARRAY_SIZE )
		  {
			 xvel[i] = -xvel[i];
			 nx[i]   = xpos[i] + Round( xvel[i] );
		  }

		if( ny[i] < BORDER_Y || ny[i] > theApp.height - BORDER_Y - BALL_ARRAY_SIZE )
		  {
			 yvel[i] = -yvel[i];
			 ny[i]   = ypos[i] + Round( yvel[i] );
		  }
    }

  for( i=0; i<BALL_NUMBER-1; i++ )
    for( j=i+1; j<BALL_NUMBER; j++ )
		{
        dx = nx[i] - nx[j];
		  dy = ny[i] - ny[j];

        if( Abs( dx ) < BALL_MIN_DISTANCE && Abs( dy ) < BALL_MIN_DISTANCE )
			 {

				Collide( dx, dy, &xvel[i], &yvel[i], &xvel[j], &yvel[j] );

				nx[i] = xpos[i] + Round( xvel[i] );
				ny[i] = ypos[i] + Round( yvel[i] );

				nx[j] = xpos[j] + Round( xvel[j] );
				ny[j] = ypos[j] + Round( yvel[j] );
			 }
		}

  for( i=0; i<BALL_NUMBER; i++ )
    {
		Twist( &xvel[i], &yvel[i], BALL_TWIST_ANGLE );
    }
}

int Round( int x )
{
  int ix;

  if( x < 0 )
	 ix = ( x - VELOSCALE/2 ) / VELOSCALE;
  else
	 ix = ( x + VELOSCALE/2 ) / VELOSCALE;

  return( ix );
}

int Fround( float x )
{
  int ix;

  if( x < 0 )
	 ix = (int)( x - 0.5 );
  else
	 ix = (int)( x + 0.5 );

  return( ix );
}

void Collide( int xij, int yij, int* vxi, int* vyi, int* vxj, int* vyj )
{
  float xa, ya, xo, yo, d, fi, fj, nvi2, nvj2, vij2, vai, voi, vaj, voj;

  d    =  sqrt( (double) xij*xij + yij*yij );
  xa   =  xij/d;
  ya   =  yij/d;
  xo   = -ya;
  yo   =  xa;

  vai  = (*vxi)*(xa) + (*vyi)*(ya);
  vaj  = (*vxj)*(xa) + (*vyj)*(ya);

  if( vai - vaj < 0. )
    {
		voi  = (*vxi)*(xo) + (*vyi)*(yo);
		voj  = (*vxj)*(xo) + (*vyj)*(yo);
		/*
        ovi2 = vai*vai + voi*voi;
        ovj2 = vaj*vaj + voj*voj;
		*/
		nvi2 = vaj*vaj + voi*voi;
		nvj2 = vai*vai + voj*voj;

		vij2 = vai*vai - vaj*vaj;

		fi   = sqrt( 1. + vij2 / nvi2 );
		fj   = sqrt( 1. - vij2 / nvj2 );

		*vxi = Fround( fi * ( voi * xo + vaj * xa ) );
		*vyi = Fround( fi * ( voi * yo + vaj * ya ) );

		*vxj = Fround( fj * ( voj * xo + vai * xa ) );
		*vyj = Fround( fj * ( voj * yo + vai * ya ) );
    }
}

void Twist( int* vx, int* vy, float angle )
{
  static int flag = 0;
  static float a11, a12, a22, a21;
  int x, y;

  if( !flag )
    {
		flag = 1;
		a11  =  cos( angle );
		a12  =  sin( angle );
		a21  = -sin( angle );
		a22  =  cos( angle );
    }

  x = *vx;
  y = *vy;

  if( drand48() < 0.5 )
    {
		*vx = Fround( a11*x + a12*y );
		*vy = Fround( a21*x + a22*y );
    }
  else
    {
		*vx = Fround( a11*x - a12*y );
		*vy = Fround(-a21*x + a22*y );
    }
}

void CopyBalls( int x[], int y[], int nx[], int ny[] )
{
  int i;

  for( i=0; i<BALL_NUMBER; i++ )
    {
		x[i] = nx[i];
		y[i] = ny[i];
    }
}

float Zerototwopi( float angle )
{
  while( angle > TWOPI )
	 angle -= TWOPI;
    
  while( angle < 0 )
	 angle += TWOPI;

  return( angle );
}

void WriteBalls( int xpos[], int ypos[], int number, unsigned char* bitmap )
{
  int i;

  for( i=0 ; i<number; i++ )
	 WriteBitmap( bitmap, xpos[i], ypos[i], BALL_ARRAY_SIZE );
}

void WriteHiBalls( int xpos[], int ypos[], int number, unsigned char* bitmap )
{
  int i;

  write_enable( theApp.fildes, 0xc0 );
  for( i=0 ; i<number; i++ )
	 WriteBitmap( bitmap, xpos[i], ypos[i], BALL_ARRAY_SIZE );
  write_enable( theApp.fildes, 0x3f );
}

int Abs( int a )
{
  return( ( a > 0 ) ? a : -a );
}

static const char vcid_balls_c[] = "$Header$";
#endif // !BALLS_C_DEFINED
