///////////////////////////////////////////////////////////////////////
//
// timing.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb  1 16:42:55 2000
// written: Wed Feb 23 15:00:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_C_DEFINED
#define TIMING_C_DEFINED

#include "timing.h"

#include <cmath>

#include "application.h"
#include "defs.h"
#include "image.h"
#include "params.h"

#include "debug.h"
#include "trace.h"

///////////////////////////////////////////////////////////////////////
//
// Timer member definitions
//
///////////////////////////////////////////////////////////////////////

void Timer::set() {
DOTRACE("Timer::set");
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

void Timer::wait(double requested_delay) {
DOTRACE("Timer::wait");

  struct timeval  tp;
  struct timezone tzp;

  long aim_for_secs  = (long)             floor( requested_delay );
  long aim_for_usecs = (long)( 1000000. *  fmod( requested_delay, 1.0 ) );

  long sec_stop  = aim_for_secs  + itsSec;
  long usec_stop = aim_for_usecs + itsUsec;

  while ( usec_stop > 1000000L ) {
	 sec_stop  += 1L;
	 usec_stop -= 1000000L;
  }

  while ( usec_stop < 0L ) {
	 sec_stop  -= 1L;
	 usec_stop += 1000000L;
  }

  do {
	 gettimeofday( &tp, &tzp );
  }
  while( tp.tv_sec < sec_stop );

  do {
	 gettimeofday( &tp, &tzp );
  }
  while( tp.tv_usec < usec_stop );

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

void Timer::logToFile(FILE* fl) const {
DOTRACE("Timer::logToFile");

  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  long sec_stop    = tp.tv_sec;
  long usec_stop  = tp.tv_usec;

  double time_elapsed =
	 sec_stop - itsSec + (usec_stop - itsUsec)/1000000.0;

  fprintf( fl, " %7.4lf\n", time_elapsed );
  fflush( stdout );
}

///////////////////////////////////////////////////////////////////////
//
// Timing member definitions
//
///////////////////////////////////////////////////////////////////////

Timer Timing::mainTimer;
Timer Timing::logTimer;

void Timing::checkFrameTime(Application* app) {
DOTRACE("Timing::checkFrameTime");

  struct timeval tp[2];

  ClearWindow(app->fildes());

  Graphics::waitFrameCount( app->fildes(), 1 );

  Timing::getTime( &tp[0] );

  Graphics::waitFrameCount( app->fildes(), 99 );

  Timing::getTime( &tp[1] );

  FRAMETIME = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  printf( " Video frame time %7.4lf ms\n", FRAMETIME );  
}

/*************************************************/

void Timing::getTime( timeval* tp ) {
DOTRACE("Timing::getTime");

  struct timezone tzp;
  gettimeofday( tp, &tzp );
}

double Timing::elapsedMsec( timeval* tp0, timeval* tp1 ) {
DOTRACE("Timing::elapsedMsec");

  double sec_lapsed  = (double)( tp1->tv_sec  - tp0->tv_sec );

  double msec_lapsed = (double)( tp1->tv_usec - tp0->tv_usec ) / 1000.;

  double delta       = sec_lapsed * 1000. + msec_lapsed;

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

void Timing::initTimeStack( double xtime, timeval* tp ) {
DOTRACE("Timing::initTimeStack");

  responsestack[0] = xtime;
  ss[0] = *tp;

  RESPONSESTACKSIZE = 1;
  STIMULUSSTACKSIZE = 1;
}


void Timing::addToResponseStack(Application* app, double xtime, int nbutton ) {
DOTRACE("Timing::addToResponseStack");

  responsestack[ RESPONSESTACKSIZE ] = xtime;
  RESPONSESTACKSIZE++;

  if( RESPONSESTACKSIZE >= MAXTIMESTACKSIZE )
    {
		printf( " MAXTIMESTACKSIZE too small\n" );
		app->quit(0);
    }
}

void Timing::addToStimulusStack(Application* app) {
DOTRACE("Timing::addToStimulusStack");

  struct timeval tp;

  Timing::getTime( &tp );

  ss[ STIMULUSSTACKSIZE ] = tp;
  STIMULUSSTACKSIZE++;

  if( STIMULUSSTACKSIZE >= MAXTIMESTACKSIZE )
    {
		printf( " MAXTIMESTACKSIZE too small\n" );
		app->quit(0);
    }
}

void Timing::tallyReactionTime( FILE* fl ) {
DOTRACE("Timing::tallyReactionTime");

  int i, j;
  double delta;

  for( i=0; i<STIMULUSSTACKSIZE; i++ )
    {
		delta = Timing::elapsedMsec( &ss[0], &ss[i] );

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
