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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "trace.h"
#include "debug.h"

//----------------------------------------------------------
//
// ParamFileIn class
//
//----------------------------------------------------------

class ParamFileIn
{
private:
  std::ifstream itsFstream;

public:
  ParamFileIn(const std::string& filebase,
              const char* extension);
  ~ParamFileIn();

  void getInt(int& var, const char* name);
  void getDouble(double& var, const char* name);
  std::string getString(const char* name);

  bool getLine(std::string& str);

  std::ifstream& stream() { return itsFstream; }
};

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

void ParamFileIn::getInt(int& var, const char* name)
{
  std::string actualname;
  itsFstream >> actualname >> var;
  if (actualname != name)
    {
      std::cerr << "param file lines out of order: "
                << "expected '" << name << "', "
                << "got '" << actualname << "'\n";
      exit(1);
    }
}

void ParamFileIn::getDouble(double& var, const char* name)
{
  std::string actualname;
  itsFstream >> actualname >> var;
  if (actualname != name)
    {
      std::cerr << "param file lines out of order: "
                << "expected '" << name << "', "
                << "got '" << actualname << "'\n";
      exit(1);
    }
}

std::string ParamFileIn::getString(const char* name)
{
  std::string actualname, result;
  itsFstream >> actualname >> result;
  if (actualname != name)
    {
      std::cerr << "param file lines out of order: "
                << "expected '" << name << "', "
                << "got '" << actualname << "'\n";
      exit(1);
    }
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
  showPhysics(false),
  filestem(""),
  windowDepth(24),
  windowHeight(1024),
  windowWidth(1280),

  displayX(),
  displayY(),

  ballNumber(),
  ballTrackNumber(),
  ballSpeed(1.0),
  ballPixmapSize(),
  ballMinDistance(),
  ballRadius(),
  ballSigma2(),
  ballTwistAngle(M_PI/16.0),

  cycleNumber(),
  waitSeconds(),
  epochSeconds(),
  pauseSeconds(),
  remindsPerEpoch(),
  remindSeconds(),

  fmriSessionNumber(1)
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
      else if (strcmp(argv[i], "--physics") == 0)
        {
          this->showPhysics = true;
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

void Params::readFromFile(const char* extension)
{
DOTRACE("Params::readFromFile");

  ParamFileIn pmfile(this->filestem, extension);

  pmfile.getInt   (this->displayX,        "DISPLAY_X");
  pmfile.getInt   (this->displayY,        "DISPLAY_Y");
  pmfile.getInt   (this->cycleNumber,     "CYCLE_NUMBER");
  pmfile.getDouble(this->waitSeconds,     "WAIT_DURATION");
  pmfile.getDouble(this->epochSeconds,    "EPOCH_DURATION");
  pmfile.getDouble(this->pauseSeconds,    "PAUSE_DURATION");
  pmfile.getDouble(this->remindSeconds,   "REMIND_DURATION");
  pmfile.getInt   (this->remindsPerEpoch, "REMINDS_PER_EPOCH");
  pmfile.getInt   (this->ballNumber,      "BALL_NUMBER");
  pmfile.getInt   (this->ballTrackNumber, "BALL_TRACK_NUMBER");
  pmfile.getDouble(this->ballSpeed,       "BALL_SPEED");
  pmfile.getInt   (this->ballPixmapSize,  "BALL_ARRAY_SIZE");
  pmfile.getInt   (this->ballMinDistance, "BALL_MIN_DISTANCE");
  pmfile.getDouble(this->ballRadius,      "BALL_RADIUS");
  pmfile.getDouble(this->ballSigma2,      "BALL_SIGMA2");
  pmfile.getDouble(this->ballTwistAngle,  "BALL_TWIST_ANGLE");
  this->filestem = pmfile.getString("FILENAME");

  // this is a placeholder for the application mode, but we ignore the
  // value in the file since the actual application mode is set at
  // startup time
  std::string appmode = pmfile.getString("APPLICATION_MODE");

  pmfile.getInt(this->fmriSessionNumber, "FMRI_SESSION_NUMBER");
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
                  50, gfx.height() - 50, 12 /* cwidth */, 1);
  gfx.swapBuffers();
}

namespace
{
  struct ParamInfo
  {
    ParamInfo(const std::string& d, const std::string& n, int& v) :
      descr(d), name(n), dvar(0), ivar(&v), dorig(0.0), iorig(v)
    {}

    ParamInfo(const std::string& d, const std::string& n, double& v) :
      descr(d), name(n), dvar(&v), ivar(0), dorig(v), iorig(0)
    {}

    bool changed() const
    {
      if (dvar != 0) return (*dvar != dorig);
      return (*ivar != iorig);
    }

    void putvar(std::ostream& s) const
    {
      if      (dvar != 0)  s << *dvar;
      else if (ivar != 0)  s << *ivar;
    }

    void putorig(std::ostream& s) const
    {
      if      (dvar != 0)  s << dorig;
      else if (ivar != 0)  s << iorig;
    }

    void getvalue(Graphics & gfx)
    {
      if      (dvar != 0) gfx.getValueFromKeyboard(*dvar);
      else if (ivar != 0) gfx.getValueFromKeyboard(*ivar);
    }

    std::string descr;
    std::string name;

  private:
    double* dvar;
    int* ivar;

    double dorig;
    int iorig;
  };

  class ParamMenu
  {
  public:
    ParamMenu() {}

    template <class T>
    void addItem(const std::string& d, const std::string& n, T& v)
    {
      items.push_back(ParamInfo(d, n, v));
    }

    void showVmenu(Graphics& gfx, unsigned int nitems)
    {
      std::vector<TextLine> vmenu;

      unsigned int descrwid = 0;
      unsigned int namewid = 0;

      for (unsigned int i = 0; i < items.size(); ++i)
        {
          if (items[i].descr.length() > descrwid)
            descrwid = items[i].descr.length();

          if (items[i].name.length() > namewid)
            namewid = items[i].name.length();
        }

      descrwid += 1;
      namewid += 1;

      std::ostringstream oss;

      oss.setf(std::ios::fixed);
      oss.precision(2);

      const char* sep = "  ";

      for (unsigned int i = 0; i < items.size(); ++i)
        {
          oss.str("");
          oss.setf(std::ios::right);
          oss << std::setw(descrwid) << items[i].descr;

          oss << sep << std::setw(namewid+2)
              << ('(' + items[i].name + ')');

          oss << sep << std::setw(6);
          items[i].putorig(oss);

          const char* marker = items[i].changed() ? "* " : "  ";

          if (i < nitems)
            {
              oss << sep << marker << std::setw(6);
              items[i].putvar(oss);

              if (items[i].changed())
                vmenu.push_back(TextLine(oss.str(), 0.7, 0.7, 0.5, 2));
              else
                vmenu.push_back(TextLine(oss.str(), 0.5, 0.5, 0.3, 1));
            }
          else if (i == nitems)
            {
              oss << sep << marker << std::setw(6) << "???";

              vmenu.push_back(TextLine(oss.str(), 0.2, 1.0, 0.2, 2));
            }
          else
            {
              vmenu.push_back(TextLine(oss.str(), 0.5, 0.6, 0.5, 1));
            }
        }

      gfx.clearBackBuffer();
      gfx.drawText(&vmenu[0], vmenu.size(), 50, -50, 14);
      gfx.swapBuffers();
    }

    void goVert(Graphics& gfx)
    {
      showVmenu(gfx, 0);

      for (unsigned int i = 0; i < items.size(); ++i)
        {
          items[i].getvalue(gfx);

          showVmenu(gfx, i+1);
        }
    }

  private:
    std::vector<ParamInfo> items;
  };
}

void Params::setParams(Graphics& gfx)
{
DOTRACE("Params::setParams");

  ParamMenu pm;

  pm.addItem("arena width",                      "DISPLAY_X", this->displayX);
  pm.addItem("arena height",                     "DISPLAY_Y", this->displayY);

  pm.addItem("# of cycles",                      "CYCLE_NUMBER", this->cycleNumber);
  pm.addItem("wait duration (seconds)",          "WAIT_DURATION", this->waitSeconds);
  pm.addItem("epoch duration (seconds)",         "EPOCH_DURATION", this->epochSeconds);
  pm.addItem("pause duration (seconds)",         "PAUSE_DURATION", this->pauseSeconds);
  pm.addItem("remind duration (seconds)",        "REMIND_DURATION", this->remindSeconds);
  pm.addItem("# of reminds per epoch",           "REMINDS_PER_EPOCH", this->remindsPerEpoch);

  pm.addItem("number of balls  (total)",         "BALL_NUMBER", this->ballNumber);
  pm.addItem("number of balls to track",         "BALL_TRACK_NUMBER", this->ballTrackNumber);
  pm.addItem("ball speed (ball widths/second)",  "BALL_SPEED", this->ballSpeed);
  pm.addItem("ball pixmap size (#pixels)",       "BALL_ARRAY_SIZE", this->ballPixmapSize);
  pm.addItem("collision radius (#pixels)",       "BALL_MIN_DISTANCE", this->ballMinDistance);
  pm.addItem("ball radius (#pixels)",            "BALL_RADIUS", this->ballRadius);
  pm.addItem("ball sigma^2 (#pixels)",           "BALL_SIGMA2", this->ballSigma2);
  pm.addItem("ball twist angle (radians)",       "BALL_TWIST_ANGLE", this->ballTwistAngle);

  pm.addItem("fMRI session #",                   "FMRI_SESSION_NUMBER", this->fmriSessionNumber);

  pm.goVert(gfx);
}

static const char vcid_params_cc[] = "$Header$";
#endif // !PARAMS_CC_DEFINED
