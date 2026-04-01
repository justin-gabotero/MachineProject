#include "user_system.h"
#include "user_file_handler.h"
#include "user_input.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * @brief Reads and validates a role choice from standard input.
 * @param outRole Output role when a valid choice is entered.
 * @return 0 on success, -1 on cancellation or invalid input stream.
 */
static int readRole(Role *outRole) {
  char buf[16];
  int choice = -1;
  int status = -1;
  int done = 0;

  while (done == 0) {
    // only allow two valid role values
    printf("Role (0 = Supplier, 1 = Receiver): ");
    status = readLine(buf, sizeof(buf));

    if (status == -2) {
      status = -1;
      done = 1;
    } else if (status == 0) {
      if (sscanf(buf, "%d", &choice) != 1) {
        choice = -1;
      }

      if (choice == 0 || choice == 1) {
        *outRole = (choice == 0) ? SUPPLIER : RECEIVER;
        status = 0;
        done = 1;
      }
    }
  }

  return status;
}

/**
 * @brief Replaces a user's fields with new values after validation.
 * @param user Target user object to edit.
 * @param in Source user data to copy from.
 * @return 0 on success, -1 for invalid input, -2 when new username already
 * exists.
 */
int editUser(User *user, User *in) {
  int status = 0;
  int exists = 0;

  if (user == NULL || in == NULL) {
    status = -1;
  }

  if (status == 0 && (strlen(in->user) == 0 || strlen(in->password) == 0)) {
    status = -1;
  }

  if (status == 0 && strcmp(user->user, in->user) != 0) {
    if (usernameExists(in->user, &exists) != 0) {
      status = -1;
    } else if (exists == 1) {
      status = -2;
    }
  }

  if (status == 0) {
    strcpy(user->user, in->user);
    strcpy(user->password, in->password);
    user->creationDate = in->creationDate;
    user->role = in->role;
  }

  return status;
}

/**
 * @brief Registers a user when the username is not already taken.
 * @param user User record to persist.
 * @return 0 on success, -1 on duplicate username or storage failure.
 */
int registerUser(User *user) {
  int exists = 0;
  int status = -1;

  if (user != NULL) {
    if (usernameExists(user->user, &exists) == 0) {
      if (exists == 0) {
        status = writeUser(user);
      }
    }
  }

  return status;
}

/**
 * @brief Runs the interactive registration flow.
 * @return 0 when registration succeeds, -1 otherwise.
 */
int registerPrompt(void) {
  User newUser;
  StringLong confirm;
  int status = 0;
  int input = 0;
  int exists = 0;

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
    if (usernameExists(newUser.user, &exists) != 0) {
      printf("Could not validate username availability.\n");
      status = -1;
      break;
    }
    if (exists == 1) {
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

    if (getCurrentDate(&newUser.creationDate) != 0) {
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

/**
 * @brief Attempts login using username and password credentials.
 * @param user Username input.
 * @param pass Password input.
 * @return Pointer to a static logged-in user on success, otherwise NULL.
 */
User *loginUser(String user, StringLong pass) {
  static User logged;
  User *result = NULL;
  int status = getUser(user, pass, &logged);

  if (status == 0) {
    strcpy(logged.password, pass);
    result = &logged;
  }

  return result;
}

/**
 * @brief Runs the interactive password recovery flow.
 * @return 0 on success, -1 on failure, -2 when cancelled.
 */
int recoverPasswordPrompt(void) {
  String user;
  StringLong newPass;
  User tempUser;
  int status = -1, input;
  int exists = 0;
  int existsStatus = 0;

  printf("\n=== Recover Password ===\n");
  printf("Username: ");
  input = readLine(user, sizeof(user));
  if (input == -2) {
    printf("Password recovery cancelled.\n");
    status = -2;
  }

  if (status == -1) {
    existsStatus = usernameExists(user, &exists);
    if (existsStatus != 0) {
      printf("Could not validate username.\n");
      status = -1;
    }
  }

  if (status == -1 && existsStatus == 0 && exists == 1) {
    printf("User %s found.\n", user);
    printf("Enter new password: ");
    input = readLine(newPass, sizeof(newPass));
    if (input == -2) {
      printf("Password recovery cancelled.\n");
      status = -2;
    }

    if (status == -1) {
      strcpy(tempUser.user, user);
      if (resetUserPassword(&tempUser, newPass) == 0) {
        printf("Password updated successfully.\n");
        status = 0;
      } else {
        printf("Failed to update password.\n");
        status = -1;
      }
    }
  } else if (status == -1 && existsStatus == 0 && exists == 0) {
    printf("Username not found.\n");
  }

  return status;
}

/**
 * @brief Resets a user's password by updating the stored record.
 * @param user User identity containing at least a valid username.
 * @param newPass New password to store.
 * @return 0 on success, -1 on invalid input or update failure.
 */
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

/**
 * @brief Runs the interactive login prompt with up to three attempts.
 * @return Pointer to logged-in user on success, otherwise NULL.
 */
User *loginPrompt(void) {
  String user;
  StringLong pass;
  User *logged = NULL;
  int status;
  int cancelled = 0;
  int attempt = 1;

  while (attempt <= 3 && logged == NULL && cancelled == 0) {
    printf("\n=== Login (%d/3) ===\n", attempt);
    printf("Username: ");
    status = readLine(user, sizeof(user));
    if (status == -2) {
      printf("Login cancelled.\n");
      cancelled = 1;
    }

    if (cancelled == 0) {
      printf("Password: ");
      status = readLine(pass, sizeof(pass));
      if (status == -2) {
        printf("Login cancelled.\n");
        cancelled = 1;
      }
    }

    if (cancelled == 0) {
      logged = loginUser(user, pass);
      if (logged == NULL) {
        printf("Invalid username or password.\n");
      }
    }

    attempt++;
  }

  if (logged == NULL && cancelled == 0) {
    printf("Login failed after 3 attempts.\n");
  }

  return logged;
}