#include "user_input.h"
#include "user_system.h"

#include <stdio.h>
#include <string.h>
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
  int result = 0;
  int readFailed = 0;

  // CSI-u sequences start with ESC [
  if (ch == '[') {
    // read until the final control character (e.g., 'u')
    while (i < (int)sizeof(seq) - 1 && readFailed == 0) {
      ch = getchar();
      if (ch == EOF) {
        readFailed = 1;
      } else {
        seq[i++] = (char)ch;
        if (ch >= '@' && ch <= '~') {
          break;
        }
      }
    }
    seq[i] = '\0';

    // only handle the CSI-u keyboard protocol payload
    if (readFailed == 0 && i > 0 && seq[i - 1] == 'u') {
      // supported formats: <key>; <modifier>u OR <key>u
      *modifier = 1;
      if (sscanf(seq, "%d;%du", keyCode, modifier) == 2) {
        result = 1;
      } else if (sscanf(seq, "%du", keyCode) == 1) {
        result = 1;
      }
    }
  }

  return result;
}

/**
 * @brief Reads a line of input from the user with backspace and Ctrl+C
 *        handling.
 * @param buf Buffer to store the input line.
 * @param size Size of the buffer.
 * @return 0 on success, -1 on error, -2 if Ctrl+C was detected.
 */
int readLine(char *buf, int size) {
  int ch = 0;
  int idx = 0;
  int status = -1;
  int done = 0;

  // invalid buffer input
  if (buf == NULL || size <= 1) {
    status = -1;
    done = 1;
  }

  // read input one character at a time for custom key handling
  while (done == 0) {
    ch = getchar();

    // input stream ended
    if (ch == EOF) {
      buf[0] = '\0';
      status = -1;
      done = 1;
    } else if (ch == '\n') {
      // end of line reached
      buf[idx] = '\0';
      status = 0;
      done = 1;
    } else if (ch == 3) {
      // raw ctrl+c support
      buf[0] = '\0';
      status = -2;
      done = 1;
    } else if (ch == 27) {
      // esc
      int keyCode = 0;
      int modifier = 1;

      if (readCsiUKey(&keyCode, &modifier)) {
        // ctrl+c
        if (keyCode == 99 && modifier == 5) {
          buf[0] = '\0';
          status = -2;
          done = 1;
        }

        // backspace
        if (done == 0 && (keyCode == 127 || keyCode == 8)) {
          if (idx > 0) {
            idx--;
            printf("\b \b");
            fflush(stdout);
          }
        }
      }
    } else if (ch == 8 || ch == 127) {
      // raw backspace support
      if (idx > 0) {
        idx--;
        printf("\b \b");
        fflush(stdout);
      }
    } else if (ch == ':') {
      // escape ':' as "%3A" so colon-delimited file formats won't break if user
      // input contains colons.
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

  return status;
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
 * @brief Displays zone selection menu and returns the selected zone.
 * @return Zone enum value (0-5) on success, -1 on invalid input.
 */
int selectZoneMenu(void) {
  int choice = 0;
  char buf[32];
  int status = 0;
  int selection = -1;

  printf("\nSelect location zone:\n");
  printf("[1] DLSU Main\n");
  printf("[2] DLSU SFC\n");
  printf("[3] Taft Avenue\n");
  printf("[4] Malate\n");
  printf("[5] Paco\n");
  printf("[6] Ermita\n");
  printf("Choice: ");

  status = readLine(buf, sizeof(buf));

  if (status == 0 && strlen(buf) > 0) {
    if (sscanf(buf, "%d", &choice) == 1 && choice >= 1 && choice <= NUM_ZONES) {
      selection = choice - 1; // Convert to 0-based index
    } else {
      status = -1;
    }
  } else {
    status = -1;
  }

  if (status == 0) {
    status = selection;
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
  int result = -1;

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
    result = -2;
  }

  if (result == -1 && status == 0 && sscanf(buf, "%d", &choice) == 1) {
    result = choice;
  }

  return result;
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