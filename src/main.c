// Project Title: Animo FoodConnect
// Authors: Justin Gabotero, Gabriel Panganiban
// CCPROG2 Machine Project

#include "user_system.h"

#include <stdio.h>

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

  while (!shouldExit) {
    int choice = readMenuChoice();

    switch (choice) {
    case 1: {
      User *currentUser = loginPrompt();
      if (currentUser == NULL) {
        break;
      }
      printf("\nWelcome, %s!\n", currentUser->user);
      printf("Role: %s\n",
             currentUser->role == SUPPLIER ? "Supplier" : "Receiver");
      shouldExit = 1;
      break;
    }
    case 2:
      registerPrompt();
      printf("You can now login with your new account.\n");
      break;
    case 3:
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
