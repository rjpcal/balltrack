///////////////////////////////////////////////////////////////////////
//
// balls.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:58:20 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_H_DEFINED
#define BALLS_H_DEFINED

#include "timing.h"

#include <vector>

class Graphics;
class Params;

struct timeval;

class Ball
{
public:
  void randomPosition(int width, int height,
                      int xdisplay, int ydisplay,
                      int arraysize);
  bool isTooClose(const Ball& other, int min_dist) const;
  void randomSpeed(double vel);
  void nextPosition(int width, int height,
                    int xdisplay, int ydisplay,
                    int arraysize, double lapsed_seconds);
  void collideIfNeeded(Ball& other, int min_dist,
                       double lapsed_seconds);
  void twist(double angle);
  void copy();
  void draw(Graphics& gfx, unsigned char* bitmap, int size,
            bool debug);

private:
  double xpos;
  double ypos;
  double xnext;
  double ynext;
  double velx; // pixels per second
  double vely; // pixels per second
};

struct Stimulus
{
  Stimulus(const Timepoint& now, int v) :
    time(now),
    correct_val(v)
  {}
  Timepoint time;
  int correct_val;

  double msecFrom(const Timepoint& time0) const
  { return 1000.0*this->time.elapsedSecSince(time0); }
};

enum Buttons
  {
    BUTTON1,
    BUTTON2,
    BUTTON3
  };

class Balls
{
private:
  void pickInitialPositions(Graphics& gfx);

  void pickNextPositions(Graphics& gfx, double lapsed_seconds);

  void drawNBalls(Graphics& gfx, int first, int last,
                  unsigned char* bitmap, bool debug);

public:
  enum TrialType { PASSIVE, CHECK_ALL, CHECK_ONE };

  Balls(const Params& params);

  void runTrial(Graphics& gfx, std::vector<Stimulus>& stimuli,
                TrialType ttype);

private:
  const Params& itsParams;
  std::vector<Ball> itsBalls;
};

static const char vcid_balls_h[] = "$Header$";
#endif // !BALLS_H_DEFINED
