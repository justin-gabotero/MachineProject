// Project Title: Animo FoodConnect
// Authors: Justin Gabotero, Gabriel Panganiban
// CCPROG2 Machine Project

#include "user_input.h"
#include "user_system.h"

#include <stdio.h>

/**
 * @brief Shows the menu for login/registration
 *
 * @return int 0 on normal exit, -1 on error, -2 on `Ctrl+C` detected
 */
static int readMenuChoice(void) {
  char buf[16];
  int choice = -1;
  int status;
  int result = -1;

  printf("\n=== Auth Menu ===\n");
  printf("1. Login\n");
  printf("2. Register\n");
  printf("3. Recover Password\n");
  printf("4. Exit\n");
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

int main(void) {
  int shouldExit = 0;
  int choice = -1;

  // Main loop for the authentication menu. It continues until the user chooses
  // to exit or successfully logs in. The menu allows users to login, register,
  // or recover their password.
  while (shouldExit == 0) {
    choice = readMenuChoice();

    switch (choice) {
    case -2:
      printf("Exiting...\n");
      shouldExit = 1;
      break;
    case 1: {
      // Attempt to log in the user. If successful, it welcomes the user and
      // displays their role. If the login fails, it breaks out of the loop and
      // returns to the menu.
      User *currentUser = loginPrompt();
      if (currentUser != NULL) {
        // Successfully logged in, enter the user menu until logout.
        userMenu(currentUser);
      }
      break;
    }
    case 2:
      // Prompts the user to register a new account. After successful
      // registration, it informs the user that they can now log in with their
      // new account. If the registration fails, it returns to the menu.
      registerPrompt();
      printf("You can now login with your new account.\n");
      break;
    case 3:
      // Prompts the user to recover their password.
      recoverPasswordPrompt();
      break;
    case 4:
      printf("Exiting...\n");
      shouldExit = 1;
      break;
    default:
      printf("Invalid choice. Please try again.\n");
      break;
    }
  }

  return 0;
}