#ifndef B_H
#define B_H

#include <stdio.h>
#include <stdlib.h>

unsigned int bin_size_to_hex_size(unsigned int data_size) {
  return data_size * 2;
}

int bin_to_hex(const void *data, unsigned int data_size, char *hex_out) {

  const unsigned char *bytes = data;

  for (unsigned int i = 0; i < data_size; ++i) {
    sprintf(hex_out + (i * 2), "%02X", bytes[i]);
  }

  return 1;
}

int hex_to_bin(const char *hex, unsigned int hex_size, void *data_out) {

  unsigned char *bytes = data_out;

  for (unsigned int i = 0; i < hex_size; i += 2) {
    sscanf(hex + i, "%2hhX", &bytes[i / 2]);
  }

  return 1;
}

#endif // B_H
