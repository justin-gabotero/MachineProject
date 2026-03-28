// Project Title: Animo FoodConnect
// Authors: Justin Gabotero, Gabriel Panganiban
// CCPROG2 Machine Project

#include "user_input.h"
#include "user_system.h"

#include <stdio.h>

// This is the main entry point for the Animo FoodConnect application. It
// provides a simple command-line interface for users to log in, register, or
// recover their password. The main function displays a menu and handles user
// input to navigate through the authentication options.
static int readMenuChoice(void) {
  char buf[16];
  int choice = -1;
  int status;

  printf("\n=== Auth Menu ===\n");
  printf("1. Login\n");
  printf("2. Register\n");
  printf("3. Recover Password\n");
  printf("4. Exit\n");
  printf("Choice: ");

  status = readLine(buf, sizeof(buf));
  if (status == -2) {
    return 3;
  }
  if (status != 0) {
    return -1;
  }

  if (sscanf(buf, "%d", &choice) != 1) {
    return -1;
  }

  return choice;
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
    case 1: {
      // Attempt to log in the user. If successful, it welcomes the user and
      // displays their role. If the login fails, it breaks out of the loop and
      // returns to the menu.
      User *currentUser = loginPrompt();
      if (currentUser != NULL) {
        printf("\nWelcome, %s!\n", currentUser->user);
        printf("Role: %s\n",
               currentUser->role == SUPPLIER ? "Supplier" : "Receiver");
        shouldExit = 1;
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