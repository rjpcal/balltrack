///////////////////////////////////////////////////////////////////////
//
// params.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:05:04 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_H_DEFINED
#define PARAMS_H_DEFINED

#include <cstdio>

class Application;

class Graphics;

enum AppMode { TRAINING, EYE_TRACKING, FMRI_SESSION };
extern bool MAKING_MOVIE;
extern AppMode APPLICATION_MODE;
extern int FMRI_SESSION_NUMBER;

extern int   DISPLAY_X;
extern int   DISPLAY_Y;
extern int   BORDER_X;                    // = ( width - 120 ) / 2;
extern int   BORDER_Y;                    // = ( height - 120 ) / 2;
extern int   CYCLE_NUMBER;
extern float EPOCH_DURATION;
extern float PAUSE_DURATION;
extern float WAIT_DURATION;
extern float REMIND_DURATION;
extern int   FRAMES_PER_REMIND; // = 11;
extern int   REMINDS_PER_EPOCH; // = 72;
extern int   BALL_NUMBER;                 // = 8;
extern int   BALL_TRACK_NUMBER; // = 2;
extern int   BALL_VELOCITY;       // = 3;
extern int   BALL_ARRAY_SIZE;     // = 16;
extern float BALL_RADIUS;
extern float BALL_SIGMA2;
extern int   BALL_MIN_DISTANCE; // = 24;
extern float BALL_TWIST_ANGLE;  // = PI/16.0;
extern char  PROGRAM[];
extern char  FILENAME[];
extern char  OBSERVER[];

#define      NOBUTTON            'n'
#define      LEFTBUTTON            'l'
#define      RIGHTBUTTON           'r'
#define      MIDDLEBUTTON          'm'

class ParamFile
{
private:
  FILE* itsFile;
  char itsLine[120];
  char itsText[120];

  static FILE* openfile(Application* app, char mode, char extension[]);

  void fetchLine();

public:
  ParamFile(Application* app, char mode, char extension[]);
  ~ParamFile();

  FILE* fp() { return itsFile; }

  void getInt(int& var);
  void getChar(char& var);
  void getFloat(float& var);
  void getText(char* var);

  void ignoreText();

  void putInt(int var, const char* name);
  void putChar(char var, const char* name);
  void putFloat(float var, const char* name);
  void putText(const char* var, const char* name);
};

class Params
{
public:
  static void readParams(Application* app, char extension[]);
  static void writeParams(Application* app, char extension[]);
  static void appendParams(ParamFile& pmfile);
  static void logParams(Application* app, ParamFile& logfile);
  static void displayParams(Application* app);
};

void RecomputeParams(Graphics& gfx);

void SetParameters1(Application* app);
void SetParameters2(Application* app);
void SetParameters3(Application* app);

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
