///////////////////////////////////////////////////////////////////////
//
// params.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:05:04 2000
// written: Mon Jun 12 10:46:21 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_H_DEFINED
#define PARAMS_H_DEFINED

#include <cstdio>

class Application;

class Graphics;

enum AppMode { TRAINING, EYE_TRACKING, FMRI_SESSION };
extern AppMode APPLICATION_MODE;
extern int FMRI_SESSION_NUMBER;

extern int   DISPLAY_X;
extern int   DISPLAY_Y;
extern int   BORDER_X;			  // = ( width - 120 ) / 2;
extern int   BORDER_Y;			  // = ( height - 120 ) / 2;
extern int   CYCLE_NUMBER;
extern float EPOCH_DURATION;
extern float PAUSE_DURATION;
extern float WAIT_DURATION;
extern float REMIND_DURATION;
extern int   FRAMES_PER_REMIND; // = 11;
extern int   REMINDS_PER_EPOCH; // = 72;
extern int   BALL_NUMBER;		  // = 8;
extern int   BALL_TRACK_NUMBER; // = 2;
extern int   BALL_VELOCITY;	  // = 3;
extern int   BALL_ARRAY_SIZE;	  // = 16;
extern float BALL_RADIUS;
extern float BALL_SIGMA2;
extern int   BALL_MIN_DISTANCE; // = 24;
extern float BALL_TWIST_ANGLE;  // = PI/16.0;
extern char  PROGRAM[];
extern char  FILENAME[];
extern char  OBSERVER[];
extern double FRAMETIME;

#define      NOBUTTON            'n'
#define      LEFTBUTTON      	   'l'
#define      RIGHTBUTTON     	   'r'
#define      MIDDLEBUTTON    	   'm'

#define      READ            	   'r'
#define      WRITE           	   'w'
#define      APPEND          	   'a'
#define      OPTIONAL        	   'o'

void ReadParams(Application* app, char extension[]);
void CheckParams(Graphics* gfx);
void WriteParams(Application* app, char extension[]);

// Called from BallsExpt::runExperiment
void LogParams(Application* app, FILE* fl);
void ListParams(Application* app);

// Called from BallsExpt::runExperiment, ReadParams, WriteParams,
// LogParams, ListParams
void Openfile(Application* app, FILE** fp, char mode, char extension[] );
void Closefile( FILE* fp );
void PrintParams();

void SetParameters1(Application* app);
void SetParameters2(Application* app);

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
