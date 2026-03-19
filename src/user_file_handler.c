#include "user_file_handler.h"

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67
#define USER_LINE_BUF_SIZE 512
#define MAX_PASSWORD_LEN 127
#define MAX_HEX_PASS_LEN (MAX_PASSWORD_LEN * 2)

static void xorBytes(const char *input, int len, unsigned char *output) {
  for (int i = 0; i < len; i++) {
    output[i] = ((unsigned char)input[i]) ^ XOR_KEY;
  }
}

static int hexValue(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return 10 + (ch - 'a');
  }
  if (ch >= 'A' && ch <= 'F') {
    return 10 + (ch - 'A');
  }
  return -1;
}

static int isHexString(const char *input) {
  int len = (int)strlen(input);

  if (len == 0 || len % 2 != 0) {
    return 0;
  }

  for (int i = 0; i < len; i++) {
    if (hexValue(input[i]) < 0) {
      return 0;
    }
  }

  return 1;
}

static int hexEncode(const unsigned char *input, int len, char *output,
                     int outputSize) {
  const char hexDigits[] = "0123456789ABCDEF";

  if (outputSize < (len * 2) + 1) {
    return -1;
  }

  for (int i = 0; i < len; i++) {
    output[i * 2] = hexDigits[(input[i] >> 4) & 0x0F];
    output[i * 2 + 1] = hexDigits[input[i] & 0x0F];
  }

  output[len * 2] = '\0';
  return 0;
}

static int hexDecode(const char *input, unsigned char *output, int outputSize,
                     int *outLen) {
  int len = (int)strlen(input);

  if (len % 2 != 0 || outputSize < len / 2) {
    return -1;
  }

  for (int i = 0; i < len; i += 2) {
    int hi = hexValue(input[i]);
    int lo = hexValue(input[i + 1]);

    if (hi < 0 || lo < 0) {
      return -1;
    }

    output[i / 2] = (unsigned char)((hi << 4) | lo);
  }

  *outLen = len / 2;
  return 0;
}

static void xorEncrypt(char *input, char *output) {
  int len = (int)strlen(input);
  xorBytes(input, len, (unsigned char *)output);
  output[len] = '\0';
}

int getUser(String user, StringLong pass, User *out) {
  char buf[USER_LINE_BUF_SIZE];
  FILE *userFile;

  String parsedUser;
  Date parsedDate;
  Role parsedRole;

  char encryptedPass[MAX_HEX_PASS_LEN + 1];
  StringLong decryptedPass;
  unsigned char xorPass[MAX_PASSWORD_LEN];

  userFile = fopen("user.txt", "r");
  if (userFile == NULL) {
    return -1;
  }

  // user data format stored in user.txt, one user per line
  // username:encrypted_password:YYYY-MM-DD:role
  while (fgets(buf, sizeof(buf), userFile)) {
    int parsed = sscanf(buf, "%31[^:]:%254[^:]:%d-%d-%d:%d", parsedUser,
                        encryptedPass, &parsedDate.year, &parsedDate.month,
                        &parsedDate.day, (int *)&parsedRole);
    if (parsed != 6) {
      continue;
    }

    // new format: HEX(XOR(raw_password)).
    if (isHexString(encryptedPass)) {
      int xorLen = 0;
      if (hexDecode(encryptedPass, xorPass, sizeof(xorPass), &xorLen) != 0) {
        continue;
      }

      for (int i = 0; i < xorLen; i++) {
        decryptedPass[i] = (char)(xorPass[i] ^ XOR_KEY);
      }
      decryptedPass[xorLen] = '\0';
    } else {
      // backward compatibility for existing raw XOR entries.  (NOTE: not sure if i should keep this later on.)
      xorEncrypt(encryptedPass, decryptedPass);
    }

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
  unsigned char xorPass[MAX_PASSWORD_LEN];
  char encryptedPass[MAX_HEX_PASS_LEN + 1];
  int passLen = (int)strlen(user->password);

  if (passLen > MAX_PASSWORD_LEN) {
    printf("Password too long.\n");
    return -1;
  }

  // store HEX(XOR(password)) so the file remains text-safe
  xorBytes(user->password, passLen, xorPass);
  if (hexEncode(xorPass, passLen, encryptedPass, sizeof(encryptedPass)) != 0) {
    printf("Could not encode password.\n");
    return -1;
  }

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