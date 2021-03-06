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
      std::cerr << "\nERROR: couldn't open file '"
                << fname << "' for reading\n";
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
      std::cerr << "\nERROR: param file lines out of order: "
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
      std::cerr << "\nERROR: param file lines out of order: "
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
      std::cerr << "\nERROR: param file lines out of order: "
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
      std::cerr << "\nERROR: unknown file mode '" << mode << "'\n";
      exit(1);
    }

  if (!itsFstream.is_open() || itsFstream.fail())
    {
      std::cerr << "\nERROR: couldn't open file '"
                << fname << "' in mode '"
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

namespace
{
  void showUsage(const char* progname)
  {
    using std::cerr;

    cerr << "usage: " << progname << " [options] <sta-file-name>\n"
         << "\n"
      "    <sta-file-name> should be the base name of a file with the extension\n"
      "        '.sta', that contains initial parameter settings. For example, \n"
      "        if you have a file named 'sample.sta', then you should pass\n"
      "        'sample' as the last argument to the progam.\n"
      "\n"
      "    [options] can be any of the following, although it is not necessary\n"
      "        to pass any options at all. If you have problems getting the\n"
      "        program to start, try including '--depth 16' or '--depth 8'.\n"
      "        You can check which video modes are available on your system\n"
      "        by running 'glxinfo'.\n"
      "\n"
      "        --mode-fmri              run in fMRI mode\n"
      "        --mode-train             run in training mode\n"
      "        --mode-eyetrack          run in eyetracking mode\n"
      "        --fmri-session-number    select from [1-4] with --mode-fmri\n"
      "\n"
      "        --makemovie              save video frames as image files\n"
      "        --physics                show ball physics for debugging\n"
      "\n"
      "        --depth [bits]           set the window depth in bits [8,16,24]\n"
      "        --width [pixels]         set the window width in pixels\n"
      "        --height [pixels]        set the window height in pixels\n"
      "\n"
      "        --help                   show this help message\n";
  }
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
      if (strcmp(argv[i], "--fmri-session-number") == 0)
        {
          this->fmriSessionNumber = atoi(argv[++i]);
        }
      else if (strcmp(argv[i], "--mode-fmri") == 0)
        {
          this->appMode = Params::FMRI_SESSION;
        }
      else if (strcmp(argv[i], "--mode-train") == 0)
        {
          this->appMode = Params::TRAINING;
        }
      else if (strcmp(argv[i], "--mode-eyetrack") == 0)
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
          if (i+1 >= argc)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer argument "
                "to option '--depth'\n";
              exit(1);
            }
          if (sscanf(argv[++i], "%d", &this->windowDepth) != 1)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer, but got '"
                        << argv[i] << "'\n";
              exit(1);
            }
          std::cout << " window depth " << this->windowDepth << "\n";
        }
      else if (strcmp(argv[i], "--width") == 0)
        {
          if (i+1 >= argc)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer argument "
                "to option '--width'\n";
              exit(1);
            }
          if (sscanf(argv[++i], "%d", &this->windowWidth) != 1)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer, but got '"
                        << argv[i] << "'\n";
              exit(1);
            }
          std::cout << " window width " << this->windowWidth << "\n";
        }
      else if (strcmp(argv[i], "--height") == 0)
        {
          if (i+1 >= argc)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer argument "
                "to option '--height'\n";
              exit(1);
            }
          if (sscanf(argv[++i], "%d", &this->windowHeight) != 1)
            {
              showUsage(argv[0]);
              std::cerr << "\nERROR: expected an integer, but got '"
                        << argv[i] << "'\n";
              exit(1);
            }
          std::cout << " window height " << this->windowHeight << "\n";
        }
      else if (!got_filename && argv[i][0] != '-')
        {
          this->filestem = argv[i];
          std::cout << " filename '" << argv[i] << "'\n";
          got_filename = true;
        }
      else if (strcmp(argv[i], "--help") == 0)
        {
          showUsage(argv[0]);
          exit(0);
        }
      else
        {
          showUsage(argv[0]);
          std::cerr << "\nERROR: unknown option '" << argv[i] << "'\n";
          exit(1);
        }
    }

  if (!got_filename)
    {
      showUsage(argv[0]);
      std::cerr << "\nERROR: need to specify a <sta-file-name>\n";
      exit(1);
    }

  if (Params::FMRI_SESSION == this->appMode)
    {
      if (this->fmriSessionNumber < 0 ||
          this->fmriSessionNumber > 4)
        {
          std::cerr << "\nERROR: fmri session number must be 1, 2, 3, or 4\n";
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
    ParamInfo(const std::string& d, const std::string& n, double& v, bool imm = false) :
      descr(d), name(n), immutable(imm), empty(false),
      dvar(&v), dorig(v),
      ivar(0), iorig(),
      bvar(0), borig()
    {}

    ParamInfo(const std::string& d, const std::string& n, int& v, bool imm = false) :
      descr(d), name(n), immutable(imm), empty(false),
      dvar(0), dorig(),
      ivar(&v), iorig(v),
      bvar(0), borig()
    {}

    ParamInfo(const std::string& d, const std::string& n, bool& v, bool imm = false) :
      descr(d), name(n), immutable(imm), empty(false),
      dvar(0), dorig(),
      ivar(0), iorig(),
      bvar(&v), borig(v)
    {}

    ParamInfo() :
      descr(), name(), immutable(false), empty(true),
      dvar(0), dorig(),
      ivar(0), iorig(),
      bvar(0), borig()
    {}

    bool changed() const
    {
      if (immutable)      return false;
      else if (dvar != 0) return (*dvar != dorig);
      else if (ivar != 0) return (*ivar != iorig);
      else if (bvar != 0) return (*bvar != borig);
      return false;
    }

    void putvar(std::ostream& s) const
    {
      if (immutable)       s << "<immutable>";

      else if (dvar != 0)  s << *dvar;
      else if (ivar != 0)  s << *ivar;
      else if (bvar != 0)  s << *bvar;
      else if (empty)      s << "";
    }

    void putorig(std::ostream& s) const
    {
      if      (dvar != 0)  s << dorig;
      else if (ivar != 0)  s << iorig;
      else if (bvar != 0)  s << borig;
      else if (empty)      s << "";
    }

    void getvalue(Graphics & gfx)
    {
      if (immutable) { /* do nothing */ }
      else if (dvar != 0) gfx.getValueFromKeyboard(*dvar);
      else if (ivar != 0) gfx.getValueFromKeyboard(*ivar);
      else if (bvar != 0)
        { int b=*bvar; gfx.getValueFromKeyboard(b); *bvar=bool(b); }
    }

    std::string descr;
    std::string name;

    bool immutable;
    bool empty;

  private:
    double* dvar;
    double dorig;

    int* ivar;
    int iorig;

    bool* bvar;
    bool borig;
  };

  void showVmenu(Graphics& gfx, std::vector<ParamInfo>& items,
                 unsigned int nitems)
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

        if (items[i].name.length() > 0)
          oss << sep << std::setw(namewid+2)
              << ('(' + items[i].name + ')');
        else
          oss << sep << std::setw(namewid+2) << "";

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
    gfx.drawText(&vmenu[0], vmenu.size(), 50, -50, 12);
    gfx.swapBuffers();
  }

  void doVmenu(Graphics& gfx, std::vector<ParamInfo>& items)
  {
    showVmenu(gfx, items, 0);

    for (unsigned int i = 0; i < items.size(); ++i)
      {
        items[i].getvalue(gfx);
        showVmenu(gfx, items, i+1);
      }
  }
}

void Params::setParams(Graphics& gfx)
{
DOTRACE("Params::setParams");

  std::vector<ParamInfo> items;

  items.push_back(ParamInfo("window depth",                     "", this->windowDepth, true));
  items.push_back(ParamInfo("window width",                     "", this->windowWidth, true));
  items.push_back(ParamInfo("window height",                    "", this->windowHeight, true));
  items.push_back(ParamInfo("arena width",                      "DISPLAY_X", this->displayX));
  items.push_back(ParamInfo("arena height",                     "DISPLAY_Y", this->displayY));
  items.push_back(ParamInfo());

  items.push_back(ParamInfo("# of cycles",                      "CYCLE_NUMBER", this->cycleNumber));
  items.push_back(ParamInfo("wait duration (seconds)",          "WAIT_DURATION", this->waitSeconds));
  items.push_back(ParamInfo("epoch duration (seconds)",         "EPOCH_DURATION", this->epochSeconds));
  items.push_back(ParamInfo("pause duration (seconds)",         "PAUSE_DURATION", this->pauseSeconds));
  items.push_back(ParamInfo("remind duration (seconds)",        "REMIND_DURATION", this->remindSeconds));
  items.push_back(ParamInfo("# of reminds per epoch",           "REMINDS_PER_EPOCH", this->remindsPerEpoch));
  items.push_back(ParamInfo("fMRI session #",                   "FMRI_SESSION_NUMBER", this->fmriSessionNumber));
  items.push_back(ParamInfo());

  items.push_back(ParamInfo("number of balls  (total)",         "BALL_NUMBER", this->ballNumber));
  items.push_back(ParamInfo("number of balls to track",         "BALL_TRACK_NUMBER", this->ballTrackNumber));
  items.push_back(ParamInfo("ball speed (ball widths/second)",  "BALL_SPEED", this->ballSpeed));
  items.push_back(ParamInfo("ball pixmap size (#pixels)",       "BALL_ARRAY_SIZE", this->ballPixmapSize));
  items.push_back(ParamInfo("collision radius (#pixels)",       "BALL_MIN_DISTANCE", this->ballMinDistance));
  items.push_back(ParamInfo("ball radius (#pixels)",            "BALL_RADIUS", this->ballRadius));
  items.push_back(ParamInfo("ball sigma^2 (#pixels)",           "BALL_SIGMA2", this->ballSigma2));
  items.push_back(ParamInfo("ball twist angle (radians)",       "BALL_TWIST_ANGLE", this->ballTwistAngle));
  items.push_back(ParamInfo());

  items.push_back(ParamInfo("DEBUG: show ball physics",         "", this->showPhysics));

  doVmenu(gfx, items);
}

static const char vcid_params_cc[] = "$Id$ $HeadURL$";
#endif // !PARAMS_CC_DEFINED
