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

class Graphics;
class Params;
class ResponseData;

struct timeval;

class Ball
{
public:
  void randomPosition(int width, int height,
                      int xborder, int yborder,
                      int arraysize);
  bool isTooClose(const Ball& other, int min_dist) const;
  void randomVelocity(int vel);
  void nextPosition(int width, int height,
                    int xborder, int yborder,
                    int arraysize);
  void collideIfNeeded(Ball& other, int min_dist);
  void twist(double angle);
  void copy();
  void draw(Graphics& gfx, unsigned char* bitmap, int size);

private:
  void collide(Ball& other, double xij, double yij);

  double xpos;
  double ypos;
  double xnext;
  double ynext;
  double xvel;
  double yvel;
};

class Balls
{
private:
  void initialize(Graphics& gfx);

  void nextBalls(Graphics& gfx);
  void copyBalls();

  void drawNBalls(Graphics& gfx, int first, int last, unsigned char* bitmap);
  void drawNHiBalls(Graphics& gfx, int first, int last, unsigned char* bitmap);

public:
  static const int MAX_BALLS = 25;

  enum TrialType { PASSIVE, CHECK_ALL, CHECK_ONE };

  Balls(const Params& params);

  void runTrial(Graphics& gfx, ResponseData& rdata, TrialType ttype);

private:
  Ball itsBalls[MAX_BALLS];
  const Params& itsParams;
};

static const char vcid_balls_h[] = "$Header$";
#endif // !BALLS_H_DEFINED
