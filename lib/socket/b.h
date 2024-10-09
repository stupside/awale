#ifndef B_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void byte_to_struct(const char *bytes, void *struct_ptr, size_t size) {
  memcpy(struct_ptr, bytes, size);
}

char *struct_to_byte(const void *struct_ptr, size_t size) {
  char *bytes = malloc(size);
  memcpy(bytes, struct_ptr, size);
  return bytes;
}

#endif