///////////////////////////////////////////////////////////////////////
//
// timing.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb  1 16:42:55 2000
// written: Tue Feb  1 16:43:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_C_DEFINED
#define TIMING_C_DEFINED

#include "timing.h"

#include <cmath>

#include "image.h"
#include "defs.h"
#include "main.h"

double FRAMETIME;

void CheckFrameTime()
{
  struct timeval tp[2];

  ClearWindow();

  FrameCount( 1 );

  GetTime( &tp[0] );

  FrameCount( 99 );

  GetTime( &tp[1] );

  FRAMETIME = DeltaTime( &tp[0], &tp[1] ) / 100.0;

  printf( " Video frame time %7.4lf ms\n", FRAMETIME );  
}

/*************************************************/

double sec_video, usec_video;

void SetVideoCount()
{
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  sec_video  = tp.tv_sec;

  usec_video = tp.tv_usec;
}

void CheckVideoCount( int number )
{
  struct timeval  tp;
  struct timezone tzp;
  double aim_for_time, actual_time;
  long aim_for_secs, aim_for_usecs;
  long sec_stop, usec_stop;

  aim_for_time  = number*FRAMETIME/1000.;
  aim_for_secs  = (long)             floor( aim_for_time );
  aim_for_usecs = (long)( 1000000. *  fmod( aim_for_time, 1.0 ) );

  sec_stop  = aim_for_secs  + sec_video;
  usec_stop = aim_for_usecs + usec_video;

  if( usec_stop > 1000000L )
    {
		sec_stop  += 1L;
		usec_stop -= 1000000L;
    }

  do
    {
		gettimeofday( &tp, &tzp );
    }
  while( tp.tv_sec < sec_stop );

  do
    {
		gettimeofday( &tp, &tzp );
    }
  while( tp.tv_usec < usec_stop );

  sec_video  = tp.tv_sec;

  usec_video = tp.tv_usec;
}

/*************************************************/

double sec_timer1, usec_timer1;

void SetTimer()
{
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  sec_timer1  = tp.tv_sec;

  usec_timer1 = tp.tv_usec;
}

void CheckTimer( double aim_for_time )
{
  struct timeval  tp;
  struct timezone tzp;
  long aim_for_secs, aim_for_usecs;
  long sec_stop, usec_stop;

  aim_for_time  = (double) aim_for_time;
  aim_for_secs  = (long)             floor( aim_for_time );
  aim_for_usecs = (long)( 1000000. *  fmod( aim_for_time, 1.0 ) );

  sec_stop  = aim_for_secs  + sec_timer1;
  usec_stop = aim_for_usecs + usec_timer1;

  if( usec_stop > 1000000L )
    {
		sec_stop  += 1L;
		usec_stop -= 1000000L;
    }

  do
    {
		gettimeofday( &tp, &tzp );
    }
  while( tp.tv_sec < sec_stop );

  do
    {
		gettimeofday( &tp, &tzp );
    }
  while( tp.tv_usec < usec_stop );

  sec_timer1  = tp.tv_sec;

  usec_timer1 = tp.tv_usec;
}

/*************************************************/

double sec_timer2, usec_timer2;

void SetLogTimer()
{
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  sec_timer2  = tp.tv_sec;

  usec_timer2 = tp.tv_usec;
}

void CheckLogTimer( FILE* fl )
{
  struct timeval  tp;
  struct timezone tzp;
  double time_elapsed;
  long sec_stop, usec_stop;

  gettimeofday( &tp, &tzp );

  sec_stop    = tp.tv_sec;

  usec_stop  = tp.tv_usec;

  time_elapsed = sec_stop - sec_timer2 + (usec_stop - usec_timer2)/1000000.0;

  fprintf( fl, " %7.4lf\n", time_elapsed );
  fflush( stdout );

}

/*************************************************/

void GetTime( timeval* tp )
{
  struct timezone tzp;
  gettimeofday( tp, &tzp );
}

double DeltaTime( timeval* tp0, timeval* tp1 )
{
  double sec_lapsed, msec_lapsed, delta;

  sec_lapsed  = (double)( tp1->tv_sec  - tp0->tv_sec );

  msec_lapsed = (double)( tp1->tv_usec - tp0->tv_usec ) / 1000.;

  delta       = sec_lapsed * 1000. + msec_lapsed;

  return( delta );
}


/***********************************************************/


#define MAXTIMESTACKSIZE 1000

int RESPONSESTACKSIZE;
int STIMULUSSTACKSIZE;
double responsestack[ MAXTIMESTACKSIZE ];
double stimulusstack[ MAXTIMESTACKSIZE ];
double reactiontime[ MAXTIMESTACKSIZE ];
struct timeval ss[ MAXTIMESTACKSIZE ];

void InitTimeStack( double xtime, timeval* tp )
{
  responsestack[0] = xtime;
  ss[0] = *tp;

  RESPONSESTACKSIZE = 1;
  STIMULUSSTACKSIZE = 1;
}


void AddToResponseStack( double xtime, int nbutton )
{
  responsestack[ RESPONSESTACKSIZE ] = xtime;
  RESPONSESTACKSIZE++;

  if( RESPONSESTACKSIZE >= MAXTIMESTACKSIZE )
    {
		printf( " MAXTIMESTACKSIZE too small\n" );
		Exit(0);
    }
}

void AddToStimulusStack()
{
  struct timeval tp;

  GetTime( &tp );

  ss[ STIMULUSSTACKSIZE ] = tp;
  STIMULUSSTACKSIZE++;

  if( STIMULUSSTACKSIZE >= MAXTIMESTACKSIZE )
    {
		printf( " MAXTIMESTACKSIZE too small\n" );
		Exit(0);
    }
}

void TallyReactionTime( FILE* fl )
{
  int i, j;
  double delta;

  for( i=0; i<STIMULUSSTACKSIZE; i++ )
    {
		delta = DeltaTime( &ss[0], &ss[i] );

		stimulusstack[i] = delta;

    }

  for( i=1; i<RESPONSESTACKSIZE; i++ )
    {
		delta = responsestack[ i ] - responsestack[ 0 ];

		if( delta >= 0.0 )
		  responsestack[ i ] = delta;
		else
		  responsestack[ i ] = delta + 4294967295.0;
    }
  responsestack[ 0 ] = 0.0;

  for( i=1; i<STIMULUSSTACKSIZE; i++ )
    {
		for( j=0; j<RESPONSESTACKSIZE; j++ )
		  {
			 if( responsestack[j] > stimulusstack[i] )
				{
				  break;
				}
		  }
	
		if( j<RESPONSESTACKSIZE )
		  reactiontime[ i ] = responsestack[j] - stimulusstack[i];
		else
		  reactiontime[ i ] = -1.0;

		if( reactiontime[ i ] > 1000.0 )
		  reactiontime[ i ] = -1.0;

    }
	    
  printf( " reaction times:\n" );
  for( i=1; i<STIMULUSSTACKSIZE; i++ )
	 printf( " %d %.0lf\n", i, reactiontime[ i ] );
  printf( " \n\n" );

  fprintf( fl,  " reaction times:\n" );
  for( i=1; i<STIMULUSSTACKSIZE; i++ )
	 fprintf( fl, " %d %.0lf\n", i, reactiontime[ i ] );
  fprintf( fl, " \n\n" );

}


static const char vcid_timing_c[] = "$Header$";
#endif // !TIMING_C_DEFINED
