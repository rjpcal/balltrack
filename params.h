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

#include <fstream>
#include <string>

class Graphics;

class ParamFileIn
{
private:
  std::ifstream itsFstream;

public:
  ParamFileIn(const std::string& filebase,
              const char* extension);
  ~ParamFileIn();

  void getInt(int& var);
  void getDouble(double& var);
  std::string getString();

  bool getLine(std::string& str);

  std::ifstream& stream() { return itsFstream; }
};

class ParamFileOut
{
private:
  std::ofstream itsFstream;

public:
  ParamFileOut(const std::string& filebase, char mode,
               const char* extension);
  ~ParamFileOut();

  void putInt(int var, const char* name);
  void putDouble(double var, const char* name);
  void putString(const std::string& str, const char* name);

  void putLine(const char* str);

  std::ofstream& stream() { return itsFstream; }
};

class Params
{
public:
  Params(int argc, char** argv);

  void readFromFile(Graphics& gfx, const char* extension);
  void writeToFile(const char* extension);
  void appendToFile(ParamFileOut& pmfile);
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
  double ballSpeed;
  double ballTwistAngle;        // = PI/16.0;
  double epochSeconds;
  double pauseSeconds;
  double remindSeconds;
  double waitSeconds;
  int    ballMinDistance;
  int    ballNumber;
  int    ballPixmapSize;
  int    ballTrackNumber;
  int    borderX;               // = (width - 120) / 2;
  int    borderY;               // = (height - 120) / 2;
  int    cycleNumber;
  int    displayX;
  int    displayY;
  int    fmriSessionNumber;
  int    framesPerRemind;       // = 11;
  int    remindsPerEpoch;       // = 72;
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
