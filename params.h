///////////////////////////////////////////////////////////////////////
//
// params.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:05:04 2000
// written: Tue Feb 22 15:12:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_H_DEFINED
#define PARAMS_H_DEFINED


extern int   DISPLAY_X;
extern int   DISPLAY_Y;
extern int   BORDER_X;
extern int   BORDER_Y;
extern int   CYCLE_NUMBER;
extern float EPOCH_DURATION;
extern float PAUSE_DURATION;
extern float WAIT_DURATION;
extern float REMIND_DURATION;
extern int   FRAMES_PER_REMIND;
extern int   REMINDS_PER_EPOCH;
extern int   BALL_NUMBER;
extern int   BALL_TRACK_NUMBER;
extern int   BALL_VELOCITY;
extern int   BALL_ARRAY_SIZE;
extern float BALL_RADIUS;
extern float BALL_SIGMA2;
extern int   BALL_MIN_DISTANCE;
extern float BALL_TWIST_ANGLE;
extern char  PROGRAM[];
extern char  FILENAME[];
extern char  OBSERVER[];
extern double FRAMETIME;

/**************

    REMINDS_PER_EPOCH     = 72;
    FRAMES_PER_REMIND   = 11;  

    BORDER_X    = ( width - 120 ) / 2; 
    BORDER_Y    = ( height - 120 ) / 2;
    BALL_MIN_DISTANCE     = 24;        
    BALL_NUMBER    = 8;                
    BALL_TRACK_NUMBER   = 2;           

    BALL_ARRAY_SIZE     = 16;          
    BALL_VELOCITY = 3;
    BALL_TWIST_ANGLE    = PI/16.;

****************/

#define      YES             	   'y'
#define      NO              	   'n'
			     	      
#define      READY           	   ' '
#define      QUIT            	   'q'
			     	      
#define      LEFTBUTTON      	   'l'
#define      RIGHTBUTTON     	   'r'
#define      MIDDLEBUTTON    	   'm'
			     	      
#define      READ            	   'r'
#define      WRITE           	   'w'
#define      APPEND          	   'a'
#define      OPTIONAL        	   'o'
			     	      
extern char line[], text[];

#include <cstdio>

void ReadParams( char extension[] );
void CheckParams();
void WriteParams( char extension[] );
void LogParams( FILE* fl );
void ListParams();
void Openfile( FILE** fp, char mode, char extension[] );
void Closefile( FILE* fp );
void PrintParams();
void SetParameters1();
void SetParameters2();

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
