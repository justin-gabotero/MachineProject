// Project Title: Animo FoodConnect
// CCPROG2 Machine Project

#include <stdio.h>

typedef char String[32];
typedef char StringLong[100];
typedef char StringBuffer[255];

typedef enum { SUPPLIER, RECEIVER } Role;

typedef struct {
  int year;
  int month;
  int day;
} Date;

// TODO: separate user handling into another file.
typedef struct {
  String user;
  StringLong password;
  Date creationDate;
  Role role;
} User;

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
  }

  // decrypt password from file

  // compare if input username and password match
  // return 1; if success

  fclose(userFile);

  return 0;
}

//
//  User Account Hadling (Register / Login)
//

int registerUser() {
  return 0; // TODO: IMPL
}

///
///
///

int main(void) { return 0; }
