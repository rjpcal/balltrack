#include <dmedia/moviefile.h>

#include "simplemovie.h"

int main() {
  SimpleMovie mymov("testmovie.mov", MV_FORMAT_QT, 480, 480);
  mymov.addSillyFrames();
  return 0;
}
