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

#include "defs.h"

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
  Params(int argc, char** argv);

  void readFromFile(Graphics& gfx, char extension[]);
  void writeToFile(char extension[]);
  void appendToFile(ParamFile& pmfile);
  void showSettings(Graphics& gfx);

  void setGroup1(Graphics& gfx);
  void setGroup2(Graphics& gfx);
  void setGroup3(Graphics& gfx);

  enum AppMode { TRAINING, EYE_TRACKING, FMRI_SESSION };
  AppMode appMode;
  bool  doMovie;
  char  filestem[STRINGSIZE];
  char  observer[STRINGSIZE];
  float BALL_RADIUS;
  float BALL_SIGMA2;
  float BALL_TWIST_ANGLE;  // = PI/16.0;
  float EPOCH_DURATION;
  float PAUSE_DURATION;
  float REMIND_DURATION;
  float WAIT_DURATION;
  int   BALL_ARRAY_SIZE;     // = 16;
  int   BALL_MIN_DISTANCE; // = 24;
  int   BALL_NUMBER;                 // = 8;
  int   BALL_TRACK_NUMBER; // = 2;
  int   BALL_VELOCITY;       // = 3;
  int   BORDER_X;                    // = ( width - 120 ) / 2;
  int   BORDER_Y;                    // = ( height - 120 ) / 2;
  int   CYCLE_NUMBER;
  int   DISPLAY_X;
  int   DISPLAY_Y;
  int   FRAMES_PER_REMIND; // = 11;
  int   FUDGEFRAME;
  int   REMINDS_PER_EPOCH; // = 72;
  int   FMRI_SESSION_NUMBER;

private:
  void recompute(Graphics& gfx);
};

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
