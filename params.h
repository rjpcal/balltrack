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

#include <iosfwd>
#include <string>

class Graphics;

class ParamFile
{
private:
  std::fstream* itsFstream;

public:
  ParamFile(const std::string& filebase, char mode,
            const char* extension);
  ~ParamFile();

  void getInt(int& var);
  void getDouble(double& var);
  std::string getString();

  bool getLine(std::string& str);

  void putInt(int var, const char* name);
  void putDouble(double var, const char* name);
  void putString(const std::string& str, const char* name);

  void putLine(const char* str);
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
  std::string filestem;
  double ballRadius;
  double ballSigma2;
  double ballTwistAngle;  // = PI/16.0;
  double epochSeconds;
  double pauseSeconds;
  double remindSeconds;
  double waitSeconds;
  int    ballMinDistance; // = 24;
  int    ballNumber;                 // = 8;
  int    ballPixmapSize;     // = 16;
  int    ballTrackNumber; // = 2;
  int    ballVelocity;       // = 3;
  int    borderX;                    // = (width - 120) / 2;
  int    borderY;                    // = (height - 120) / 2;
  int    cycleNumber;
  int    displayX;
  int    displayY;
  int    fmriSessionNumber;
  int    framesPerRemind; // = 11;
  int    remindsPerEpoch; // = 72;
  int    windowDepth;
  int    windowHeight;
  int    windowWidth;
private:
  int    fudgeframes;

private:
  void recompute(Graphics& gfx);
};

static const char vcid_params_h[] = "$Header$";
#endif // !PARAMS_H_DEFINED
