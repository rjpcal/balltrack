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
// ParamFile class
//
//----------------------------------------------------------

ParamFile::ParamFile(const std::string& filebase, char mode,
                     const char* extension) :
  itsFstream(0)
{
  DOTRACE("<params.cc>::openfile");

  std::string fname = filebase + "." + extension;

  switch (mode)
    {
    case 'r': itsFstream = new std::fstream(fname.c_str(), std::ios::in); break;
    case 'w': itsFstream = new std::fstream(fname.c_str(), std::ios::out); break;
    case 'a': itsFstream = new std::fstream(fname.c_str(), std::ios::out|std::ios::app); break;
    default:
      std::cerr << "unknown file mode '" << mode << "'\n";
      exit(1);
    }

  if (!itsFstream->is_open() || itsFstream->fail())
    {
      std::cerr << "couldn't open " << fname << " in mode '"
                << mode << "'\n";
      exit(1);
    }
}

ParamFile::~ParamFile()
{
  delete itsFstream;
}

void ParamFile::getInt(int& var)
{
  std::string dummy;
  *itsFstream >> dummy >> var;
}

void ParamFile::getDouble(double& var)
{
  std::string dummy;
  *itsFstream >> dummy >> var;
}

std::string ParamFile::getString()
{
  std::string dummy, result;
  *itsFstream >> dummy >> result;
  return result;
}

bool ParamFile::getLine(std::string& str)
{
  std::getline(*itsFstream, str);
  return *itsFstream;
}

void ParamFile::putInt(int var, const char* name)
{
  *itsFstream << std::left << std::setw(19) << name << " "
              << var << '\n';
}

void ParamFile::putDouble(double var, const char* name)
{
  *itsFstream << std::left << std::setw(19) << name << " "
              << std::showpoint << std::fixed << std::setprecision(2)
              << var << '\n';
}

void ParamFile::putString(const std::string& str, const char* name)
{
  *itsFstream << std::left << std::setw(19) << name << " "
              << str << '\n';
}

void ParamFile::putLine(const char* str)
{
  *itsFstream << str << '\n';
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
  ballTwistAngle(),
  epochSeconds(),
  pauseSeconds(),
  remindSeconds(),
  waitSeconds(),
  ballMinDistance(),
  ballNumber(),
  ballPixmapSize(),
  ballTrackNumber(),
  ballVelocity(),
  borderX(),
  borderY(),
  cycleNumber(),
  displayX(),
  displayY(),
  fmriSessionNumber(1),
  framesPerRemind(),
  remindsPerEpoch(),
  windowDepth(24),
  windowHeight(1024),
  windowWidth(1280),
  fudgeframes(10)
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

void Params::readFromFile(Graphics& gfx, char extension[])
{
DOTRACE("Params::readFromFile");

  ParamFile pmfile(filestem, 'r', extension);

  pmfile.getInt   (displayX);
  pmfile.getInt   (displayY);
  pmfile.getInt   (cycleNumber);
  pmfile.getDouble(waitSeconds);
  pmfile.getDouble(epochSeconds);
  pmfile.getDouble(pauseSeconds);
  pmfile.getDouble(remindSeconds);
  pmfile.getInt   (remindsPerEpoch);
  pmfile.getInt   (framesPerRemind);
  pmfile.getInt   (ballNumber);
  pmfile.getInt   (ballTrackNumber);
  pmfile.getInt   (ballVelocity);
  pmfile.getInt   (ballPixmapSize);
  pmfile.getInt   (ballMinDistance);
  pmfile.getDouble(ballRadius);
  pmfile.getDouble(ballSigma2);
  pmfile.getDouble(ballTwistAngle);
  filestem = pmfile.getString();

  // this is a placeholder for the application mode, but we ignore the
  // value in the file since the actual application mode is set at
  // startup time
  std::string appmode = pmfile.getString();

  pmfile.getInt(fmriSessionNumber);

  this->recompute(gfx);
}

void Params::writeToFile(char extension[])
{
DOTRACE("Params::writeToFile");

  ParamFile pmfile(filestem, 'w', extension);

  appendToFile(pmfile);
}

void Params::appendToFile(ParamFile& pmfile)
{
DOTRACE("Params::appendToFile");

  pmfile.putInt   (displayX,          "DISPLAY_X");
  pmfile.putInt   (displayY,          "DISPLAY_Y");
  pmfile.putInt   (cycleNumber,       "CYCLE_NUMBER");
  pmfile.putDouble(waitSeconds,       "WAIT_DURATION");
  pmfile.putDouble(epochSeconds,      "EPOCH_DURATION");
  pmfile.putDouble(pauseSeconds,      "PAUSE_DURATION");
  pmfile.putDouble(remindSeconds  ,   "REMIND_DURATION");
  pmfile.putInt   (remindsPerEpoch,   "REMINDS_PER_EPOCH");
  pmfile.putInt   (framesPerRemind,   "FRAMES_PER_REMIND");
  pmfile.putInt   (ballNumber,        "BALL_NUMBER");
  pmfile.putInt   (ballTrackNumber,   "BALL_TRACK_NUMBER");
  pmfile.putInt   (ballVelocity,      "BALL_VELOCITY");
  pmfile.putInt   (ballPixmapSize,    "BALL_ARRAY_SIZE");
  pmfile.putInt   (ballMinDistance,   "BALL_MIN_DISTANCE");
  pmfile.putDouble(ballRadius,        "BALL_RADIUS");
  pmfile.putDouble(ballSigma2,        "BALL_SIGMA2");
  pmfile.putDouble(ballTwistAngle,    "BALL_TWIST_ANGLE");
  pmfile.putString(filestem,          "FILENAME");

  const char* app_mode = "unknown";
  switch (appMode)
    {
    case TRAINING:      app_mode = "TRAINING";     break;
    case EYE_TRACKING:  app_mode = "EYE_TRACKING"; break;
    case FMRI_SESSION:  app_mode = "FMRI_SESSION"; break;
    }

  pmfile.putString(app_mode,          "APPLICATION_MODE");
  pmfile.putInt   (fmriSessionNumber, "FMRI_SESSION_NUMBER");
}

void Params::showSettings(Graphics& gfx)
{
DOTRACE("Params::showSettings");

  writeToFile("sta");

  std::vector<std::string> params;

  ParamFile pmfile(filestem, 'r', "sta");

  std::string buf;
  while (pmfile.getLine(buf))
    {
      params.push_back(buf);
    }

  gfx.clearFrontBuffer();
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

  gfx.clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
      gfx.clearBackBuffer();
      gfx.swapBuffers();
    }

  menu[0] = " BALL  NUMBER TRACK  VELOC  SIZE   MINDIS RADIUS SIGMA2 TWIST";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-6d %-6d %-6d %-6d %-6d %-6.1f %-6.1f %-6.3f",
           ballNumber, ballTrackNumber, ballVelocity,
           ballPixmapSize, ballMinDistance, ballRadius,
           ballSigma2, ballTwistAngle);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(ballNumber);
  snprintf(buf, bufsize, "       %-6d", ballNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(ballTrackNumber);
  snprintf(buf, bufsize, " %-6d", ballTrackNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(ballVelocity);
  snprintf(buf, bufsize, " %-6d", ballVelocity);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(ballPixmapSize);
  snprintf(buf, bufsize, " %-6d", ballPixmapSize);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(ballMinDistance);
  snprintf(buf, bufsize, " %-6d", ballMinDistance);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(ballRadius);
  snprintf(buf, bufsize, " %-6.1f", ballRadius);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(ballSigma2);
  snprintf(buf, bufsize, " %-6.1f", ballSigma2);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(ballTwistAngle);
  snprintf(buf, bufsize, " %-6.3f", ballTwistAngle);
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

  gfx.clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
      gfx.clearBackBuffer();
      gfx.swapBuffers();
    }

  menu[0] = "       CYCL_NUM WAIT_DUR EPCH_DUR PAUS_DUR RMND_NUM RMND_DUR";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-8d %-8.2f %-8.2f %-8.2f %-8d %-8.2f",
          cycleNumber, waitSeconds, epochSeconds,
          pauseSeconds, remindsPerEpoch, remindSeconds);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(cycleNumber);
  snprintf(buf, bufsize, "       %-8d", cycleNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(waitSeconds);
  snprintf(buf, bufsize, " %-8.2f", waitSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(epochSeconds);
  snprintf(buf, bufsize, " %-8.2f", epochSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(pauseSeconds);
  snprintf(buf, bufsize, " %-8.2f", pauseSeconds);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(remindsPerEpoch);
  snprintf(buf, bufsize, " %-8d", remindsPerEpoch);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getDouble(remindSeconds);
  snprintf(buf, bufsize, " %-8.2f", remindSeconds);
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

  gfx.clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
      gfx.clearBackBuffer();
      gfx.swapBuffers();
    }

  menu[0] = "       SESSION_NUMBER";
  menu[1] = "";
  menu[2] = "";
  snprintf(buf, bufsize, "       %-8d", fmriSessionNumber);
  menu[3] = buf;

  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  gfx.getInt(fmriSessionNumber);
  snprintf(buf, bufsize, "       %-8d", fmriSessionNumber);
  menu[1] += buf;
  gfx.drawStrings(menu, 4, 100, -200, 16);
  gfx.swapBuffers();

  this->recompute(gfx);
}

void Params::recompute(Graphics& gfx)
{
DOTRACE("Params::recompute");

  borderX = (gfx.width() - displayX) / 2;
  borderY = (gfx.height() - displayY) / 2;

  const double time_between_reminds =
    (epochSeconds - pauseSeconds - remindSeconds) / remindsPerEpoch;

  const double frametime = gfx.frameTime();

  std::cout << " Video frame time " << frametime << "ms\n";

  framesPerRemind = int(1000.0*(time_between_reminds-remindSeconds)
                        / frametime) - fudgeframes;

  DebugEval(time_between_reminds);
  DebugEval(frametime);
  DebugEvalNL(fudgeframes);

  DebugEvalNL(framesPerRemind);
}

static const char vcid_params_cc[] = "$Header$";
#endif // !PARAMS_CC_DEFINED
