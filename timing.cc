///////////////////////////////////////////////////////////////////////
//
// timing.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb  1 16:42:55 2000
// written: Wed Sep  3 14:16:50 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_CC_DEFINED
#define TIMING_CC_DEFINED

#include "timing.h"

#include "application.h"
#include "defs.h"
#include "params.h"

#include <cmath>
#include <cstdlib>

#include "debug.h"
#include "trace.h"

///////////////////////////////////////////////////////////////////////
//
// Timer member definitions
//
///////////////////////////////////////////////////////////////////////

void Timer::set()
{
DOTRACE("Timer::set");
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

void Timer::wait(double requested_delay)
{
DOTRACE("Timer::wait");

  struct timeval  tp;
  struct timezone tzp;

  long aim_for_secs  = (long)             floor( requested_delay );
  long aim_for_usecs = (long)( 1000000. *  fmod( requested_delay, 1.0 ) );

  long sec_stop  = aim_for_secs  + long(itsSec);
  long usec_stop = aim_for_usecs + long(itsUsec);

  while ( usec_stop > 1000000L )
    {
      sec_stop  += 1L;
      usec_stop -= 1000000L;
    }

  while ( usec_stop < 0L )
    {
      sec_stop  -= 1L;
      usec_stop += 1000000L;
    }

  do
    {
      gettimeofday( &tp, &tzp );
    } while (tp.tv_sec < sec_stop);

  do
    {
      gettimeofday( &tp, &tzp );
    } while (tp.tv_usec < usec_stop);

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

void Timer::logToFile(FILE* fl) const
{
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

/*************************************************/

void Timing::getTime( timeval* tp )
{
DOTRACE("Timing::getTime");

  struct timezone tzp;
  gettimeofday( tp, &tzp );
}

double Timing::elapsedMsec( timeval* tp0, timeval* tp1 )
{
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
int RESPONSE_VAL_STACK_SIZE;

struct Response
{
  double time;
  int val;
};

struct Stimulus
{
  double time;
  int correct_val;
};

Stimulus stimulus_stack[ MAXTIMESTACKSIZE ];
Response response_stack[ MAXTIMESTACKSIZE ];

struct Reaction
{
  double time;
  bool correct;
};

Reaction reaction_stack[ MAXTIMESTACKSIZE ];

struct timeval ss_0;
double response_time_stack_0;
struct timeval response_timeval_0;

double percent_correct = 0.0;

namespace
{
  void log_reactions(FILE* f)
  {
    fprintf( f,  " reaction times:\n" );
    for (int i=1; i<STIMULUSSTACKSIZE; ++i)
      fprintf( f, " %d %.0lf\n", i, reaction_stack[ i ].time );
    fprintf( f, " \n\n" );

    fprintf( f,  " reaction correct?:\n" );
    for (int j=1; j<STIMULUSSTACKSIZE; ++j)
      fprintf( f, " %d %d\n", j, int(reaction_stack[ j ].correct) );
    fprintf( f, " \n" );

    fprintf( f, " percent correct: %d\n\n", int(percent_correct) );
  }
}

void Timing::initTimeStack( double xtime, timeval* tp )
{
DOTRACE("Timing::initTimeStack");

  response_time_stack_0 = xtime;

  response_timeval_0 = *tp;

  response_stack[0].time = 0.0;
  stimulus_stack[0].time = 0.0;
  ss_0 = *tp;

  RESPONSESTACKSIZE = 1;
  STIMULUSSTACKSIZE = 1;
  RESPONSE_VAL_STACK_SIZE = 1;
}


void Timing::addToResponseStack(double xtime, int nbutton )
{
DOTRACE("Timing::addToResponseStack");

  double delta = xtime - response_time_stack_0;

  if (delta >= 0.0)
    response_stack[ RESPONSESTACKSIZE ].time = delta;
  else
    response_stack[ RESPONSESTACKSIZE ].time = delta + 4294967295.0;

  response_stack[ RESPONSESTACKSIZE ].val = nbutton;

  RESPONSESTACKSIZE++;

  if (RESPONSESTACKSIZE >= MAXTIMESTACKSIZE)
    {
      printf( " MAXTIMESTACKSIZE too small\n" );
      exit(0);
    }
}

void Timing::addToResponseStack(long sec, long usec, int nbutton)
{
DOTRACE("Timing::addToResponseStack");

  timeval tp;
  tp.tv_sec = sec;
  tp.tv_usec = usec;

  double delta = elapsedMsec( &response_timeval_0, &tp);

  response_stack[ RESPONSESTACKSIZE ].time = delta;

  response_stack[ RESPONSESTACKSIZE ].val = nbutton;

  RESPONSESTACKSIZE++;

  if (RESPONSESTACKSIZE >= MAXTIMESTACKSIZE)
    {
      printf( " MAXTIMESTACKSIZE too small\n" );
      exit(0);
    }
}


void Timing::addToStimulusStack(int correct_nbutton)
{
DOTRACE("Timing::addToStimulusStack");

  struct timeval tp;

  Timing::getTime( &tp );

  // Compute the trial onset time relative to the first time
  stimulus_stack[STIMULUSSTACKSIZE].time =
         elapsedMsec( &ss_0, &tp );

  // Note the correct response value
  stimulus_stack[STIMULUSSTACKSIZE].correct_val = correct_nbutton;

  STIMULUSSTACKSIZE++;

  if (STIMULUSSTACKSIZE >= MAXTIMESTACKSIZE)
    {
      printf( " MAXTIMESTACKSIZE too small\n" );
      exit(0);
    }
}

void Timing::tallyReactionTime(FILE* fl, float remind_duration)
{
DOTRACE("Timing::tallyReactionTime");

  int total_stims = 0;
  int number_correct = 0;

  // Compute the response time for each stimulus (or indicate a
  // non-response with -1.0)
  for (int i=1; i<STIMULUSSTACKSIZE; ++i)
    {
      int j;

      // Find the first response (j'th) that came after the i'th stimulus
      for (j=0; j<RESPONSESTACKSIZE; ++j)
        {
          if (response_stack[j].time > stimulus_stack[i].time)
            break;
        }

         // If we found a corresponding response, compute the response time...
      if (j < RESPONSESTACKSIZE)
        {
          reaction_stack[ i ].time =
            response_stack[j].time - stimulus_stack[i].time;
          reaction_stack[i].correct =
            ( response_stack[j].val == stimulus_stack[i].correct_val );
        }

      // But if there was no corresponding response, indicate a
      // non-response with -1.0
      else
        {
          reaction_stack[ i ].time = -1.0;
          reaction_stack[i].correct = false;
        }

      // If the reaction time was too large, it doesn't count, so
      // indicate a non-response with -1.0
      if (reaction_stack[ i ].time > remind_duration*1000)
        {
          reaction_stack[ i ].time = -1.0;
          reaction_stack[i].correct = false;
        }

      ++total_stims;
      if (reaction_stack[i].correct) ++number_correct;
  }

  percent_correct = (100.0 * number_correct) / total_stims;

  log_reactions(stdout);
  log_reactions(fl);
}

double Timing::recentPercentCorrect()
{
DOTRACE("Timing::recentPercentCorrect");
  return percent_correct;
}

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
