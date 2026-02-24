// Project Title: Animo FoodConnect
// Authors: Justin Gabotero, Gabriel Panganiban
// CCPROG2 Machine Project

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67

typedef char String[32];
typedef char StringLong[128];

typedef enum { SUPPLIER, RECEIVER } Role;

typedef struct {
  int year;
  int month;
  int day;
} Date;

typedef struct {
  String user;
  StringLong password;
  Date creationDate;
  Role role;
} User;

///
/// Function Prototypes
///

void xorEncrypt(char *input, char *output);
int getUser(String user, StringLong pass, User *out);
int editUser(User *user, User *in);
int writeUser(User *user);
int registerUser();
User *loginUser(StringLong user, StringLong pass);

///
/// Encryption Handling
///

void xorEncrypt(char *input, char *output) {
  int len = strlen(input);
  for (int i = 0; i < len; i++) {
    output[i] = input[i] ^ XOR_KEY;
  }
  output[len] = '\0'; // end string
}

///
/// File Handling for user.txt
///

int getUser(String user, StringLong pass, User *out) {
  StringLong buf;
  FILE *userFile;

  String parsedUser;
  Date parsedDate;
  Role parsedRole;

  StringLong encryptedPass, decryptedPass;

  userFile = fopen("user.txt", "r");

  // user data format stored in user.txt, one user per line
  // username:encrypted_password:YYYY-MM-DD:role
  while (fgets(buf, sizeof(buf), userFile)) {
    sscanf(buf, "%[^:]:%[^:]:%d-%d-%d:%d", parsedUser, encryptedPass,
           &parsedDate.year, &parsedDate.month, &parsedDate.day,
           (int *)&parsedRole);

    // decrypt password from file
    xorEncrypt(encryptedPass, decryptedPass); // inverse to decrypt

    // compare if input username and password match
    if (strcmp(parsedUser, user) == 0 && strcmp(decryptedPass, pass) == 0) {
      strcpy(out->user, parsedUser);
      out->creationDate = parsedDate;
      out->role = parsedRole;

      fclose(userFile);
      return (int)out->role; // returns 0 = supplier, 1 = receiver
    }
  }

  fclose(userFile);
  printf("Username and Password does not match!\n");

  return -1; // not found
}

int editUser(User *user, User *in) {
  return 0; // TODO: IMPL
}

int writeUser(User *user) {
  FILE *userFile;

  userFile = fopen("user.txt", "a");
  return 0;
}

//
//  User Account Handling (Register / Login)
//

int registerUser() {
  return 0; // TODO: IMPL
}

User *loginUser(String user, StringLong pass) {
  User *logged;
  int role = getUser(user, pass, logged);
  if (role > -1) {
    return logged;
  } else {
    printf("Could not login, username and password does not match!\n");
  }

  return 0; //
}

///
///
///

int main(void) { return 0; }
