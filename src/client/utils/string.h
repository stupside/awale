#ifndef UTILS_STRING_H
#define UTILS_STRING_H

// Copyright (C) 2021

#include <ctype.h>
#include <string.h>

int is_number(const char *str) {
  if (str == NULL || *str == '\0')
    return 0; // Empty string is not a number

  // Check each character to ensure it's a digit
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i]))
      return 0; // Non-digit character found
  }

  return 1; // All characters are digits
}

#endif