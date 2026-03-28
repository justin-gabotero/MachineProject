#include "user_input.h"
#include "user_system.h"

#include <stdio.h>
#include <time.h>

// Reads a CSI-u key sequence for modern terminals and extracts the key code and
// modifier.
// @param keyCode Pointer to store the extracted key code.
// @param modifier Pointer to store the extracted modifier (e.g., Ctrl).
// @return 1 if a valid CSI-u sequence was read, 0 otherwise.
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
// @param buf Buffer to store the input line.
// @param size Size of the buffer.
// @return 0 on success, -1 on error, -2 if ctrl+c was detected.
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

    // escape ':' as "%3A" so colon-delimited file formats won't break if user
    // input contains colons.
    if (ch == ':') {
      if (idx < size - 4) {
        buf[idx++] = '%';
        buf[idx++] = '3';
        buf[idx++] = 'A';
      }
    } else {
      // append regular printable character
      if (idx < size - 1) {
        buf[idx++] = (char)ch;
      }
    }
  }
}

int getCurrentDate(Date *date) {
  int status = -1;
  time_t now = 0;
  struct tm *localNow = NULL;

  if (date != NULL) {
    now = time(NULL);
    if (now != (time_t)-1) {
      localNow = localtime(&now);
      if (localNow != NULL) {
        date->year = localNow->tm_year + 1900;
        date->month = localNow->tm_mon + 1;
        date->day = localNow->tm_mday;
        status = 0;
      }
    }
  }

  return status;
}

static int readUserMenuChoice(User *currentUser) {
  char buf[16];
  int choice = -1;
  int status;

  printf("\n=== User Menu ===\n");
  printf("1. View Profile\n");

  if (currentUser->role == SUPPLIER) {
    printf("2. View Donations\n");
    printf("3. Create Donation\n");
  } else if (currentUser->role == RECEIVER) {
    printf("2. View Requests\n");
    printf("3. Create Request\n");
  }
  printf("4. View All Donations\n");
  printf("5. Logout\n");
  printf("Choice: ");

  status = readLine(buf, sizeof(buf));
  if (status == -2) {
    return 2;
  }
  if (status != 0) {
    return -1;
  }

  if (sscanf(buf, "%d", &choice) != 1) {
    return -1;
  }

  return choice;
}

int userMenu(User *currentUser) {
  int status = 0, choice = -1;

  // get the role from the user
  if (currentUser == NULL) {
    status = -1;
  }

  if (status == 0) {
    printf("\nWelcome, %s! Your role is: %s\n", currentUser->user,
           currentUser->role == SUPPLIER   ? "Donor"
           : currentUser->role == RECEIVER ? "Recipient"
                                           : "Unknown");
  }

  choice = readUserMenuChoice(currentUser);

  switch (choice) {
  case 1:
    // View Profile
    break;
  case 2:
    // View Donations/Requests
    break;
  case 3:
    // Create Donation/Request
    break;
  case 4:
    // View All Donations
    break;
  case 5:
    // Logout
    break;
  default:
    printf("Invalid choice. Please try again.\n");
    break;
  }

  return status;
}