///////////////////////////////////////////////////////////////////////
//
// params.cc
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:30:51 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_CC_DEFINED
#define PARAMS_CC_DEFINED

#include "params.h"

#include "graphics.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "trace.h"
#include "debug.h"

//----------------------------------------------------------
//
// ParamFileIn class
//
//----------------------------------------------------------

ParamFileIn::ParamFileIn(const std::string& filebase,
                         const char* extension) :
  itsFstream()
{
  DOTRACE("<params.cc>::openfile");

  std::string fname = filebase + "." + extension;

  itsFstream.open(fname.c_str());

  if (!itsFstream.is_open() || itsFstream.fail())
    {
      std::cerr << "couldn't open " << fname << " for reading\n";
      exit(1);
    }
}

ParamFileIn::~ParamFileIn()
{}

void ParamFileIn::getInt(int& var)
{
  std::string dummy;
  itsFstream >> dummy >> var;
}

void ParamFileIn::getDouble(double& var)
{
  std::string dummy;
  itsFstream >> dummy >> var;
}

std::string ParamFileIn::getString()
{
  std::string dummy, result;
  itsFstream >> dummy >> result;
  return result;
}

bool ParamFileIn::getLine(std::string& str)
{
  std::getline(itsFstream, str);
  return itsFstream;
}

//----------------------------------------------------------
//
// ParamFileOut class
//
//----------------------------------------------------------

ParamFileOut::ParamFileOut(const std::string& filebase, char mode,
                           const char* extension) :
  itsFstream()
{
  DOTRACE("<params.cc>::openfile");

  std::string fname = filebase + "." + extension;

  switch (mode)
    {
    case 'w': itsFstream.open(fname.c_str(), std::ios::out); break;
    case 'a': itsFstream.open(fname.c_str(), std::ios::out|std::ios::app); break;
    default:
      std::cerr << "unknown file mode '" << mode << "'\n";
      exit(1);
    }

  if (!itsFstream.is_open() || itsFstream.fail())
    {
      std::cerr << "couldn't open " << fname << " in mode '"
                << mode << "'\n";
      exit(1);
    }
}

ParamFileOut::~ParamFileOut()
{}

void ParamFileOut::putInt(int var, const char* name)
{
  itsFstream << std::left << std::setw(19) << name << " "
             << var << '\n';
}

void ParamFileOut::putDouble(double var, const char* name)
{
  itsFstream << std::left << std::setw(19) << name << " "
             << std::showpoint << std::fixed << std::setprecision(2)
             << var << '\n';
}

void ParamFileOut::putString(const std::string& str, const char* name)
{
  itsFstream << std::left << std::setw(19) << name << " "
             << str << '\n';
}

void ParamFileOut::putLine(const char* str)
{
  itsFstream << str << '\n';
}

//----------------------------------------------------------
//
// Params class
//
//----------------------------------------------------------

Params::Params(int argc, char** argv) :
  appMode(EYE_TRACKING),
  doMovie(false),
  filestem(""),
  ballRadius(),
  ballSigma2(),
  ballSpeed(1.0),
  ballTwistAngle(),
  epochSeconds(),
  pauseSeconds(),
  remindSeconds(),
  waitSeconds(),
  ballMinDistance(),
  ballNumber(),
  ballPixmapSize(),
  ballTrackNumber(),
  borderX(),
  borderY(),
  cycleNumber(),
  displayX(),
  displayY(),
  fmriSessionNumber(1),
  remindsPerEpoch(),
  windowDepth(24),
  windowHeight(1024),
  windowWidth(1280)
{
  this->appMode = Params::TRAINING;

  bool got_filename = false;

  for (int i = 1; i < argc; ++i)
    {
      if (strcmp(argv[i], "--session") == 0)
        {
          this->fmriSessionNumber = atoi(argv[++i]);
        }
      else if (strcmp(argv[i], "--fmri") == 0)
        {
          this->appMode = Params::FMRI_SESSION;
        }
      else if (strcmp(argv[i], "--train") == 0)
        {
          this->appMode = Params::TRAINING;
        }
      else if (strcmp(argv[i], "--itrk") == 0)
        {
          this->appMode = Params::EYE_TRACKING;
        }
      else if (strcmp(argv[i], "--makemovie") == 0)
        {
          this->doMovie = true;
        }
      else if (strcmp(argv[i], "--depth") == 0)
        {
          this->windowDepth = atoi(argv[++i]);
          std::cout << " windowDepth " << this->windowDepth << "\n";
        }
      else if (strcmp(argv[i], "--width") == 0)
        {
          this->windowWidth = atoi(argv[++i]);
          std::cout << " windowWidth " << this->windowWidth << "\n";
        }
      else if (strcmp(argv[i], "--height") == 0)
        {
          this->windowHeight = atoi(argv[++i]);
          std::cout << " windowHeight " << this->windowHeight << "\n";
        }
      else if (!got_filename)
        {
          this->filestem = argv[i];
          std::cout << " filename '" << argv[i] << "'\n";
          got_filename = true;
        }
      else
        {
          std::cerr << "unknown command-line argument '"
                    << argv[i] << "'\n";
          exit(1);
        }
    }

  if (!got_filename)
    {
      std::cerr << "need to specify a file basename\n";
      exit(1);
    }

  if (Params::FMRI_SESSION == this->appMode)
    {
      if (this->fmriSessionNumber < 0 ||
          this->fmriSessionNumber > 4)
        {
          std::cerr << "session number must be 1, 2, 3, or 4\n";
          exit(1);
        }
    }
}

void Params::readFromFile(Graphics& gfx, const char* extension)
{
DOTRACE("Params::readFromFile");

  ParamFileIn pmfile(this->filestem, extension);

  int dummy;

  pmfile.getInt   (this->displayX);
  pmfile.getInt   (this->displayY);
  pmfile.getInt   (this->cycleNumber);
  pmfile.getDouble(this->waitSeconds);
  pmfile.getDouble(this->epochSeconds);
  pmfile.getDouble(this->pauseSeconds);
  pmfile.getDouble(this->remindSeconds);
  pmfile.getInt   (this->remindsPerEpoch);
  pmfile.getInt   (dummy); // FIXME used to be framesPerRemind
  pmfile.getInt   (this->ballNumber);
  pmfile.getInt   (this->ballTrackNumber);
  pmfile.getDouble(this->ballSpeed);
  pmfile.getInt   (this->ballPixmapSize);
  pmfile.getInt   (this->ballMinDistance);
  pmfile.getDouble(this->ballRadius);
  pmfile.getDouble(this->ballSigma2);
  pmfile.getDouble(this->ballTwistAngle);
  this->filestem = pmfile.getString();

  // this is a placeholder for the application mode, but we ignore the
  // value in the file since the actual application mode is set at
  // startup time
  std::string appmode = pmfile.getString();

  pmfile.getInt(this->fmriSessionNumber);

  this->recompute(gfx);
}

void Params::writeToFile(const char* extension)
{
DOTRACE("Params::writeToFile");

  ParamFileOut pmfile(this->filestem, 'w', extension);

  appendToFile(pmfile);
}

void Params::appendToFile(ParamFileOut& pmfile)
{
DOTRACE("Params::appendToFile");

  pmfile.putInt   (this->displayX,          "DISPLAY_X");
  pmfile.putInt   (this->displayY,          "DISPLAY_Y");
  pmfile.putInt   (this->cycleNumber,       "CYCLE_NUMBER");
  pmfile.putDouble(this->waitSeconds,       "WAIT_DURATION");
  pmfile.putDouble(this->epochSeconds,      "EPOCH_DURATION");
  pmfile.putDouble(this->pauseSeconds,      "PAUSE_DURATION");
  pmfile.putDouble(this->remindSeconds  ,   "REMIND_DURATION");
  pmfile.putInt   (this->remindsPerEpoch,   "REMINDS_PER_EPOCH");
  pmfile.putInt   (0,                       "<defunct>");// FIXME
  pmfile.putInt   (this->ballNumber,        "BALL_NUMBER");
  pmfile.putInt   (this->ballTrackNumber,   "BALL_TRACK_NUMBER");
  pmfile.putDouble(this->ballSpeed,         "BALL_SPEED");
  pmfile.putInt   (this->ballPixmapSize,    "BALL_ARRAY_SIZE");
  pmfile.putInt   (this->ballMinDistance,   "BALL_MIN_DISTANCE");
  pmfile.putDouble(this->ballRadius,        "BALL_RADIUS");
  pmfile.putDouble(this->ballSigma2,        "BALL_SIGMA2");
  pmfile.putDouble(this->ballTwistAngle,    "BALL_TWIST_ANGLE");
  pmfile.putString(this->filestem,          "FILENAME");

  const char* app_mode = "unknown";
  switch (appMode)
    {
    case TRAINING:      app_mode = "TRAINING";     break;
    case EYE_TRACKING:  app_mode = "EYE_TRACKING"; break;
    case FMRI_SESSION:  app_mode = "FMRI_SESSION"; break;
    }

  pmfile.putString(app_mode,                "APPLICATION_MODE");
  pmfile.putInt   (this->fmriSessionNumber, "FMRI_SESSION_NUMBER");
}

void Params::showSettings(Graphics& gfx)
{
DOTRACE("Params::showSettings");

  writeToFile("sta");

  std::vector<std::string> params;

  ParamFileIn pmfile(this->filestem, "sta");

  std::string buf;
  while (pmfile.getLine(buf))
    {
      params.push_back(buf);
    }

  gfx.clearBackBuffer();
  gfx.drawStrings(&params[0], params.size(),
                  50, gfx.height() - 50, 10 /* cwidth */);
  gfx.swapBuffers();
}

void Params::setGroup1(Graphics& gfx)
{
DOTRACE("Params::setGroup1");

  const int bufsize = 256;
  char buf[bufsize];

  std::string menu[4];

  gfx.clearBackBuffer();

  menu[0] = " BALL  NUMBER TRACK  SPEED  SIZE   MINDIS RADIUS SIGMA2 TWIST";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-6d %-6d %-6.1f %-6d %-6d %-6.1f %-6.1f %-6.3f",
           this->ballNumber, this->ballTrackNumber, this->ballSpeed,
           this->ballPixmapSize, this->ballMinDistance, this->ballRadius,
           this->ballSigma2, this->ballTwistAngle);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->ballNumber);
  snprintf(buf, bufsize, "       %-6d", this->ballNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->ballTrackNumber);
  snprintf(buf, bufsize, " %-6d", this->ballTrackNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->ballSpeed);
  snprintf(buf, bufsize, " %-6.3f", this->ballSpeed);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->ballPixmapSize);
  snprintf(buf, bufsize, " %-6d", this->ballPixmapSize);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->ballMinDistance);
  snprintf(buf, bufsize, " %-6d", this->ballMinDistance);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->ballRadius);
  snprintf(buf, bufsize, " %-6.1f", this->ballRadius);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->ballSigma2);
  snprintf(buf, bufsize, " %-6.1f", this->ballSigma2);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->ballTwistAngle);
  snprintf(buf, bufsize, " %-6.3f", this->ballTwistAngle);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  this->recompute(gfx);
}

void Params::setGroup2(Graphics& gfx)
{
DOTRACE("Params::setGroup2");

  const int bufsize = 256;
  char buf[bufsize];

  std::string menu[4];

  gfx.clearBackBuffer();

  menu[0] = "       CYCL_NUM WAIT_DUR EPCH_DUR PAUS_DUR RMND_NUM RMND_DUR";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-8d %-8.2f %-8.2f %-8.2f %-8d %-8.2f",
          this->cycleNumber, this->waitSeconds, this->epochSeconds,
          this->pauseSeconds, this->remindsPerEpoch, this->remindSeconds);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->cycleNumber);
  snprintf(buf, bufsize, "       %-8d", this->cycleNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->waitSeconds);
  snprintf(buf, bufsize, " %-8.2f", this->waitSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->epochSeconds);
  snprintf(buf, bufsize, " %-8.2f", this->epochSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->pauseSeconds);
  snprintf(buf, bufsize, " %-8.2f", this->pauseSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(this->remindsPerEpoch);
  snprintf(buf, bufsize, " %-8d", this->remindsPerEpoch);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(this->remindSeconds);
  snprintf(buf, bufsize, " %-8.2f", this->remindSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  this->recompute(gfx);
}

void Params::setGroup3(Graphics& gfx)
{
DOTRACE("Params::setGroup2");

  const int bufsize = 256;
  char buf[bufsize];

  std::string menu[4];

  gfx.clearBackBuffer();

  menu[0] = "       SESSION_NUMBER";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-8d", this->fmriSessionNumber);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(fmriSessionNumber);
  snprintf(buf, bufsize, "       %-8d", this->fmriSessionNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  this->recompute(gfx);
}

void Params::recompute(Graphics& gfx)
{
DOTRACE("Params::recompute");

  this->borderX = (gfx.width() - displayX) / 2;
  this->borderY = (gfx.height() - displayY) / 2;
}

static const char vcid_params_cc[] = "$Header$";
#endif // !PARAMS_CC_DEFINED
