///////////////////////////////////////////////////////////////////////
//
// timing.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:52:28 2000
// written: Tue Feb  1 16:25:09 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_H_DEFINED
#define TIMING_H_DEFINED

#include <cstdio>
#include <sys/time.h>

void CheckFrameTime();
void SetVideoCount();
void CheckVideoCount( int number );
void SetTimer();
void CheckTimer( double aim_for_time );
void SetLogTimer();
void CheckLogTimer( FILE* fl );
void GetTime( timeval* tp );
double DeltaTime( timeval* tp0, timeval* tp1 );
void InitTimeStack( double xtime, timeval* tp );
void AddToResponseStack( double xtime, int nbutton );
void AddToStimulusStack();
void TallyReactionTime( FILE* fl );

static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
