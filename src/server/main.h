#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#include "mediator.h"

void app(struct Mediator *);

int main(int argc, char **argv) {

  struct Mediator mediator;

  init_mediator(&mediator);

  app(&mediator);

  // test();

  return EXIT_SUCCESS;
}
#endif