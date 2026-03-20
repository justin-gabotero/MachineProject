#include "user_system.h"
#include "user_file_handler.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

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

static int readRole(Role *outRole) {
  char buf[16];
  int choice = -1;
  int status;

  while (choice != 0 && choice != 1) {
    // only allow two valid role values
    printf("Role (0 = Supplier, 1 = Receiver): ");
    status = readLine(buf, sizeof(buf));
    if (status == -2) {
      return -1;
    }
    if (status != 0) {
      continue;
    }

    if (sscanf(buf, "%d", &choice) != 1) {
      choice = -1;
    }
  }

  *outRole = (choice == 0) ? SUPPLIER : RECEIVER;
  return 0;
}

static int readCreationDate(Date *outDate) {
  int status = -1;
  time_t now = 0;
  struct tm *localNow = NULL;

  if (outDate != NULL) {
    now = time(NULL);
    if (now != (time_t)-1) {
      localNow = localtime(&now);
      if (localNow != NULL) {
        outDate->year = localNow->tm_year + 1900;
        outDate->month = localNow->tm_mon + 1;
        outDate->day = localNow->tm_mday;
        status = 0;
      }
    }
  }

  return status;
}

int editUser(User *user, User *in) {
  int status = 0;

  if (user == NULL || in == NULL) {
    status = -1;
  }

  if (status == 0 && (strlen(in->user) == 0 || strlen(in->password) == 0)) {
    status = -1;
  }

  if (status == 0 && strcmp(user->user, in->user) != 0 &&
      usernameExists(in->user)) {
    status = -2;
  }

  if (status == 0) {
    strcpy(user->user, in->user);
    strcpy(user->password, in->password);
    user->creationDate = in->creationDate;
    user->role = in->role;
  }

  return status;
}

int registerUser(User *user) {
  int status = -1;
  if (usernameExists(user->user)) {
    return status;
  }
  status = writeUser(user);
  return status;
}

int registerPrompt(void) {
  User newUser;
  StringLong confirm;
  int status = 0;
  int input = 0;

  newUser.user[0] = '\0';
  newUser.password[0] = '\0';
  confirm[0] = '\0';

  printf("\n=== Register ===\n");

  do {
    printf("Username: ");
    input = readLine(newUser.user, sizeof(newUser.user));
    if (input == -2) {
      printf("Registration cancelled.\n");
      status = -1;
      break;
    }
    if (input != 0 || strlen(newUser.user) == 0) {
      printf("Username cannot be empty.\n");
      status = -1;
      break;
    }
    if (usernameExists(newUser.user)) {
      printf("Username already exists.\n");
      status = -1;
      break;
    }

    printf("Password: ");
    input = readLine(newUser.password, sizeof(newUser.password));
    if (input == -2) {
      printf("Registration cancelled.\n");
      status = -1;
      break;
    }
    if (input != 0 || strlen(newUser.password) == 0) {
      printf("Password cannot be empty.\n");
      status = -1;
      break;
    }

    printf("Confirm Password: ");
    input = readLine(confirm, sizeof(confirm));
    if (input == -2) {
      printf("Registration cancelled.\n");
      status = -1;
      break;
    }
    if (input != 0 || strcmp(newUser.password, confirm) != 0) {
      printf("Passwords do not match.\n");
      status = -1;
      break;
    }

    if (readRole(&newUser.role) != 0) {
      printf("Registration cancelled.\n");
      status = -1;
      break;
    }

    if (readCreationDate(&newUser.creationDate) != 0) {
      printf("Registration cancelled.\n");
      status = -1;
      break;
    }

    if (registerUser(&newUser) != 0) {
      printf("Registration failed.\n");
      status = -1;
      break;
    }

    printf("Registration successful.\n");
  } while (0);

  return status;
}

User *loginUser(String user, StringLong pass) {
  static User logged;
  User *result = NULL;
  int role = getUser(user, pass, &logged);

  if (role > -1) {
    strcpy(logged.password, pass);
    result = &logged;
  }

  return result;
}

int recoverPasswordPrompt(void) {
  String user;
  StringLong newPass;
  User tempUser;
  int status = -1, input;

  printf("\n=== Recover Password ===\n");
  printf("Username: ");
  input = readLine(user, sizeof(user));
  if (input == -2) {
    printf("Password recovery cancelled.\n");
    status = 1;
  }

  if (status == -1 && usernameExists(user)) {
    printf("Username exists.\n");
    printf("Enter new password: ");
    input = readLine(newPass, sizeof(newPass));
    if (input == -2) {
      printf("Password recovery cancelled.\n");
      status = 1;
    }

    if (status == -1) {
      strcpy(tempUser.user, user);
      if (resetUserPassword(&tempUser, newPass) == 0) {
        printf("Password updated successfully.\n");
        status = 0;
      }
    }

    if (status != 0) {
      printf("Failed to update password.\n");
    }
  } else if (status == -1) {
    printf("Username not found.\n");
  }

  return status;
}

int resetUserPassword(User *user, StringLong newPass) {
  int status = -1;
  User updatedUser;

  if (user != NULL && strlen(user->user) > 0 && strlen(newPass) > 0) {
    updatedUser.user[0] = '\0';
    updatedUser.password[0] = '\0';
    updatedUser.creationDate.year = 0;
    updatedUser.creationDate.month = 0;
    updatedUser.creationDate.day = 0;
    strcpy(updatedUser.user, user->user);
    strcpy(updatedUser.password, newPass);
    updatedUser.role = (Role)-1;
    status = updateUserRecord(updatedUser);
  }

  return status;
}

User *loginPrompt(void) {
  String user;
  StringLong pass;
  User *logged = NULL;
  int status;

  for (int attempt = 1; attempt <= 3; attempt++) {
    printf("\n=== Login (%d/3) ===\n", attempt);
    printf("Username: ");
    status = readLine(user, sizeof(user));
    if (status == -2) {
      printf("Login cancelled.\n");
      return NULL;
    }

    printf("Password: ");
    status = readLine(pass, sizeof(pass));
    if (status == -2) {
      printf("Login cancelled.\n");
      return NULL;
    }

    logged = loginUser(user, pass);
    if (logged != NULL) {
      return logged;
    }

    printf("Invalid username or password.\n");
  }

  printf("Login failed after 3 attempts.\n");
  return NULL;
}