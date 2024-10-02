#include <stdio.h>
#include <stdlib.h>

void app(const char *address, const char *name);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage : %s [address] [pseudo]\n", argv[0]);
    return EXIT_FAILURE;
  }

  app(argv[1], argv[2]);

  return EXIT_SUCCESS;
}