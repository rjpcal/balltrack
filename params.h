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

  void readFromFile(const char* extension);
  void writeToFile(const char* extension);
  void appendToFile(ParamFileOut& pmfile);

  void showSettings(Graphics& gfx);

  void setParams(Graphics& gfx);

  enum AppMode { TRAINING, EYE_TRACKING, FMRI_SESSION };

  // command-line args
  AppMode appMode;
  bool doMovie;
  bool showPhysics;
  std::string filestem;
  int    windowDepth;
  int    windowHeight;
  int    windowWidth;

  int    displayX;
  int    displayY;

  // group 1
  int    ballNumber;
  int    ballTrackNumber;
  double ballSpeed;
  int    ballPixmapSize;
  int    ballMinDistance;
  double ballRadius;
  double ballSigma2;
  double ballTwistAngle;

  // group 2
  int    cycleNumber;
  double waitSeconds;
  double epochSeconds;
  double pauseSeconds;
  int    remindsPerEpoch;
  double remindSeconds;

  // group 3
  int    fmriSessionNumber;

  double ballMotionSeconds() const
  {
    return
      ((epochSeconds - pauseSeconds - remindSeconds)
       / remindsPerEpoch) - remindSeconds;
  }
};

static const char vcid_params_h[] = "$Id$ $HeadURL$";
#endif // !PARAMS_H_DEFINED
