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
#include <string>

class Graphics;

#define      LEFTBUTTON            'l'
#define      RIGHTBUTTON           'r'
#define      MIDDLEBUTTON          'm'

class ParamFile
{
private:
  FILE* itsFile;
  char itsLine[120];
  char itsText[120];

  void fetchLine();

public:
  ParamFile(const char* filebase, char mode, char extension[]);
  ~ParamFile();

  FILE* fp() { return itsFile; }

  void getInt(int& var);
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
  void readParams(Graphics& gfx, char extension[]);
  void writeParams(char extension[]);
  void appendParams(ParamFile& pmfile);
  void logParams(ParamFile& logfile);
  void displayParams(Graphics& gfx);

  void setGroup1(Graphics& gfx);
  void setGroup2(Graphics& gfx);
  void setGroup3(Graphics& gfx);

  enum AppMode { TRAINING, EYE_TRACKING, FMRI_SESSION };
  static AppMode APPLICATION_MODE;
  static bool MAKING_MOVIE;
  static char  FILENAME[];
  static char  OBSERVER[];
  static char  PROGRAM[];
  static float BALL_RADIUS;
  static float BALL_SIGMA2;
  static float BALL_TWIST_ANGLE;  // = PI/16.0;
  static float EPOCH_DURATION;
  static float PAUSE_DURATION;
  static float REMIND_DURATION;
  static float WAIT_DURATION;
  static int   BALL_ARRAY_SIZE;     // = 16;
  static int   BALL_MIN_DISTANCE; // = 24;
  static int   BALL_NUMBER;                 // = 8;
  static int   BALL_TRACK_NUMBER; // = 2;
  static int   BALL_VELOCITY;       // = 3;
  static int   BORDER_X;                    // = ( width - 120 ) / 2;
  static int   BORDER_Y;                    // = ( height - 120 ) / 2;
  static int   CYCLE_NUMBER;
  static int   DISPLAY_X;
  static int   DISPLAY_Y;
  static int   FRAMES_PER_REMIND; // = 11;
  static int   FUDGEFRAME;
  static int   REMINDS_PER_EPOCH; // = 72;
  static int   FMRI_SESSION_NUMBER;

private:
  void recompute(Graphics& gfx);
};

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
