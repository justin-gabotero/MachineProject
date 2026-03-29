#include "user_input.h"
#include "user_system.h"

#include <stdio.h>
#include <time.h>

static int isValidRole(Role role) {
  return role == SUPPLIER || role == RECEIVER;
}

static void printDate(const Date *date) {
  if (date != NULL) {
    printf("%04d-%02d-%02d", date->year, date->month, date->day);
  }
}

static void printProfile(const User *currentUser) {
  if (currentUser != NULL) {
    printf("\n=== Profile ===\n");
    printf("Username: %s\n", currentUser->user);
    printf("Role: %s\n", currentUser->role == SUPPLIER ? "Donor" : "Recipient");
    printf("Created On: ");
    printDate(&currentUser->creationDate);
    printf("\n");
  }
}

/**
 * @brief Reads a CSI-u key sequence for modern terminals and extracts key
 *        metadata.
 * @param keyCode Pointer to store the extracted key code.
 * @param modifier Pointer to store the extracted modifier (e.g., Ctrl).
 * @return 1 if a valid CSI-u sequence was read, 0 otherwise.
 */
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

/**
 * @brief Reads a line of input from the user with backspace and Ctrl+C
 *        handling.
 * @param buf Buffer to store the input line.
 * @param size Size of the buffer.
 * @return 0 on success, -1 on error, -2 if Ctrl+C was detected.
 */
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

/**
 * @brief Gets the current local date from the system clock.
 * @param date Pointer to a Date structure to fill.
 * @return 0 on success, -1 on failure.
 */
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

/**
 * @brief Displays the user menu and reads the selected menu option.
 * @param currentUser Pointer to the currently logged-in user.
 * @return Selected menu option on success, -2 if Ctrl+C is detected, or -1 on
 *         invalid input.
 */
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
    return -2;
  }
  if (status != 0) {
    return -1;
  }

  if (sscanf(buf, "%d", &choice) != 1) {
    return -1;
  }

  return choice;
}

/**
 * @brief Handles the main user menu flow after login.
 * @param currentUser Pointer to the currently logged-in user.
 * @return 0 on success, -1 if the user pointer is invalid.
 */
int userMenu(User *currentUser) {
  int status = 0;
  int choice = -1;
  int shouldLogout = 0;

  // get the role from the user
  if (currentUser == NULL) {
    status = -1;
  }

  if (status == 0 && isValidRole(currentUser->role) == 0) {
    status = -1;
  }

  if (status == 0) {
    printf("\nWelcome, %s! Your role is: %s\n", currentUser->user,
           currentUser->role == SUPPLIER   ? "Donor"
           : currentUser->role == RECEIVER ? "Recipient"
                                           : "Unknown");
  }

  while (status == 0 && shouldLogout == 0) {
    choice = readUserMenuChoice(currentUser);

    switch (choice) {
    case -2:
      printf("Logout requested.\n");
      shouldLogout = 1;
      break;
    case 1:
      printProfile(currentUser);
      break;
    case 2:
      if (currentUser->role == SUPPLIER) {
        viewOwnDonations(currentUser);
      } else {
        printf("\n=== Your Requests ===\n");
        printf("Request viewing is not yet implemented.\n");
      }
      break;
    case 3:
      if (currentUser->role == SUPPLIER) {
        createDonationFlow(currentUser);
      } else {
        printf("\n=== Create Request ===\n");
        printf("Request creation is not yet implemented.\n");
      }
      break;
    case 4:
      viewAllDonationsList();
      break;
    case 5:
      printf("Logging out...\n");
      shouldLogout = 1;
      break;
    default:
      printf("Invalid choice. Please try again.\n");
      break;
    }
  }

  return status;
}