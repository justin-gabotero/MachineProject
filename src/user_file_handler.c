#include "user_file_handler.h"

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67

void xorEncrypt(char *input, char *output) {
  int len = strlen(input);
  for (int i = 0; i < len; i++) {
    output[i] = input[i] ^ XOR_KEY;
  }
  output[len] = '\0';
}

int getUser(String user, StringLong pass, User *out) {
  StringLong buf;
  FILE *userFile;

  String parsedUser;
  Date parsedDate;
  Role parsedRole;

  StringLong encryptedPass, decryptedPass;

  userFile = fopen("user.txt", "r");
  if (userFile == NULL) {
    return -1;
  }

  // user data format stored in user.txt, one user per line
  // username:encrypted_password:YYYY-MM-DD:role
  while (fgets(buf, sizeof(buf), userFile)) {
    int parsed = sscanf(buf, "%31[^:]:%127[^:]:%d-%d-%d:%d", parsedUser,
                        encryptedPass, &parsedDate.year, &parsedDate.month,
                        &parsedDate.day, (int *)&parsedRole);
    if (parsed != 6) {
      continue;
    }

    // decrypt password from file
    xorEncrypt(encryptedPass, decryptedPass);

    // compare if input username and password match
    if (strcmp(parsedUser, user) == 0 && strcmp(decryptedPass, pass) == 0) {
      strcpy(out->user, parsedUser);
      out->creationDate = parsedDate;
      out->role = parsedRole;

      fclose(userFile);
      return (int)out->role;
    }
  }

  fclose(userFile);
  return -1;
}

int usernameExists(String user) {
  StringLong buf;
  FILE *userFile;
  String parsedUser;

  // open user database for read checks
  userFile = fopen("user.txt", "r");
  if (userFile == NULL) {
    return 0;
  }

  // each line starts with username before the first ':'
  while (fgets(buf, sizeof(buf), userFile)) {
    // compare parsed username with requested username
    if (sscanf(buf, "%31[^:]", parsedUser) == 1 && strcmp(parsedUser, user) == 0) {
      fclose(userFile);
      return 1;
    }
  }

  fclose(userFile);
  return 0;
}

int writeUser(User *user) {
  FILE *userFile;
  StringLong encryptedPass;

  // store encrypted password in file, not plain text
  xorEncrypt(user->password, encryptedPass);

  // add new user entry to the end of user.txt
  userFile = fopen("user.txt", "a");
  if (userFile == NULL) {
    printf("Could not open user.txt\n");
    return -1;
  }

  // user data format in user.txt:
  // username:encrypted_password:YYYY-MM-DD:role
  fprintf(userFile, "%s:%s:%04d-%02d-%02d:%d\n", user->user, encryptedPass,
          user->creationDate.year, user->creationDate.month,
          user->creationDate.day, (int)user->role);

  fclose(userFile);
  return 0;
}