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

#include "timepoint.h"

#include <cmath>
#include <vector>

class Graphics;
class Params;

struct vec
{
  vec(double xx=0.0, double yy=0.0) : x(xx), y(yy) {}

  double x;
  double y;

  vec operator-(const vec& other) const
  { return vec(this->x - other.x,
               this->y - other.y); }

  vec operator+(const vec& other) const
  { return vec(this->x + other.x,
               this->y + other.y); }

  double length() const
  { return sqrt(x*x + y*y); }

  double lengthsq() const
  { return x*x + y*y; }
};

inline double dot(const vec& v1, const vec& v2)
{ return
    (v1.x * v2.x) +
    (v1.y * v2.y); }

inline vec operator*(const vec& w, double v)
{ return vec(w.x * v,
             w.y * v); }

inline vec operator*(double v, const vec& w)
{ return vec(w.x * v,
             w.y * v); }

inline vec operator/(const vec& w, double v)
{ return vec(w.x / v,
             w.y / v); }

inline vec operator/(double v, const vec& w)
{ return vec(w.x / v,
             w.y / v); }

inline vec unit(const vec& w)
{
  return w / w.length();
}

inline vec rot90(const vec& w)
{ return vec( -w.y,
               w.x ); }

inline vec rotate(const vec& w, double angle /* radians */)
{
  const double a11  =  cos(angle);
  const double a12  =  sin(angle);
  const double a21  = -sin(angle);
  const double a22  =  cos(angle);

  return vec(a11*w.x + a12*w.y,
             a21*w.x + a22*w.y);
}

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

  bool checkInsideBorder(int width, int height,
                         int xdisplay, int ydisplay,
                         int arraysize) const;

  void bringInsideBorder(int width, int height,
                         int xdisplay, int ydisplay,
                         int arraysize);

  void collideIfNeeded(Ball& other, int min_dist,
                       double lapsed_seconds);
  void twist(double angle);
  void copy();
  void draw(Graphics& gfx, unsigned char* bitmap, int size,
            bool debug);

private:
  vec pos;
  vec next;
  vec vel; // pixels per second
};

struct Stimulus
{
  Stimulus(const Timepoint& now, int v) :
    time(now),
    correct_val(v)
  {}
  Timepoint time;
  int correct_val;

  double elapsedSec(const Timepoint& time0) const
  { return this->time.elapsedSecSince(time0); }
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

static const char vcid_balls_h[] = "$Id$ $HeadURL$";
#endif // !BALLS_H_DEFINED
