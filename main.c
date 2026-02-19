// Project Title: Animo FoodConnect
// Authors: Justin Gabotero, Gabriel Panganiban
// CCPROG2 Machine Project

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67

typedef char String[32];
typedef char StringLong[100];
typedef char StringBuffer[255];

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
/// Encryption Handling
///

void xorEncrypt(char *input, char *output) {
  int len = strlen(input);
  for (int i = 0; i < len; i++) {
    output[i] = input[i] ^ XOR_KEY;
  }
  output[len] = '\0'; // end string
};

///
/// File Handling for user.txt
///

int getUser(String user, StringLong pass, User *out) {
  StringBuffer buf;
  FILE *userFile;
  int i, roleNum, lineCount = 0;

  StringLong encryptedPass, decryptedPass;

  userFile = fopen("user.txt", "r");

  // user data format stored in user.txt, one user per line
  // username:encrypted_password:YYYY-MM-DD:role
  while (fgets(buf, sizeof(buf), userFile)) {
    sscanf(buf, "%[^:]:%[^:]:%d-%d-%d:%d", out->user, encryptedPass,
           &out->creationDate.year, &out->creationDate.month,
           &out->creationDate.day, &roleNum);
    // decrypt password from file

    xorEncrypt(pass, decryptedPass); // inverse to decrypt

    // compare if input username and password match
    if (strcmp(out->user, user) == 0 && strcmp(decryptedPass, pass) == 0) {
      out->role = (Role)roleNum;

      fclose(userFile);
      return roleNum; // returns 0 = supplier, 1 = receiver
    }
  }

  fclose(userFile);
  printf("Username and Password does not match!\n");

  return -1; // not found
}

//
//  User Account Handling (Register / Login)
//

int registerUser() {
  return 0; // TODO: IMPL
}

///
///
///

int main(void) { return 0; }
