#include "user_input.h"

#include <stdio.h>

// Reads a CSI-u key sequence and extracts the key code and modifier.
static int readCsiUKey(int *keyCode, int *modifier) {
  int ch = getchar();
  char seq[32];
  int i = 0;

  // CSI-u sequences start with ESC [
  if (ch != '[') {
    return 0;
  }

  // read until the final control character (e.g., 'u')
  while (i < (int)sizeof(seq) - 1) {
    ch = getchar();
    if (ch == EOF) {
      return 0;
    }

    seq[i++] = (char)ch;
    if (ch >= '@' && ch <= '~') {
      break;
    }
  }
  seq[i] = '\0';

  // only handle the CSI-u keyboard protocol payload
  if (i == 0 || seq[i - 1] != 'u') {
    return 0;
  }

  // supported formats: <key>; <modifier>u OR <key>u
  *modifier = 1;
  if (sscanf(seq, "%d;%du", keyCode, modifier) == 2) {
    return 1;
  }

  if (sscanf(seq, "%du", keyCode) == 1) {
    return 1;
  }

  return 0;
}

// Reads a line of input from the user into the provided buffer, with support
// for backspace and ctrl+c.
int readLine(char *buf, int size) {
  int ch;
  int idx = 0;

  // invalid buffer input
  if (buf == NULL || size <= 1) {
    return -1;
  }

  // read input one character at a time for custom key handling
  while (1) {
    ch = getchar();

    // input stream ended
    if (ch == EOF) {
      buf[0] = '\0';
      return -1;
    }

    // end of line reached
    if (ch == '\n') {
      buf[idx] = '\0';
      return 0;
    }

    // raw ctrl+c support
    if (ch == 3) {
      buf[0] = '\0';
      return -2;
    }

    // esc
    if (ch == 27) {
      int keyCode = 0;
      int modifier = 1;

      if (readCsiUKey(&keyCode, &modifier)) {
        // ctrl+c
        if (keyCode == 99 && modifier == 5) {
          buf[0] = '\0';
          return -2;
        }

        // backspace
        if (keyCode == 127 || keyCode == 8) {
          if (idx > 0) {
            idx--;
            printf("\b \b");
            fflush(stdout);
          }
        }
      }

      continue;
    }

    // raw backspace support
    if (ch == 8 || ch == 127) {
      if (idx > 0) {
        idx--;
        printf("\b \b");
        fflush(stdout);
      }
      continue;
    }

    // append regular printable character
    if (idx < size - 1) {
      buf[idx++] = (char)ch;
    }
  }
}