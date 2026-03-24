#include "user_file_handler.h"

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67
#define USER_LINE_BUF_SIZE 512
#define MAX_PASSWORD_LEN 127
#define MAX_HEX_PASS_LEN (MAX_PASSWORD_LEN * 2)

// helper function to XOR encrypt/decrypt password bytes
static void xorBytes(const char *input, int len, unsigned char *output) {
  for (int i = 0; i < len; i++) {
    output[i] = ((unsigned char)input[i]) ^ XOR_KEY;
  }
}

// helper functions for hex encoding/decoding to store XORed passwords in text
// format
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

// check if a string is a valid hex string (even length and only hex chars)
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

// convert binary data to hex string (output should have enough space for null
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

// convert hex string back to binary data, output should have enough space for
// the decoded bytes, outLen will be set to the number of decoded bytes
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

// XOR encrypt the input string and store the result in output
static void xorEncrypt(char *input, char *output) {
  int len = (int)strlen(input);
  xorBytes(input, len, (unsigned char *)output);
  output[len] = '\0';
}

// read user data from user.txt and check if the provided username and password
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
      // backward compatibility for existing raw XOR entries.  (NOTE: not sure
      // if i should keep this later on.)
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

// check if a username already exists in user.txt (used for registration to
// prevent duplicates)
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
    if (sscanf(buf, "%31[^:]", parsedUser) == 1 &&
        strcmp(parsedUser, user) == 0) {
      fclose(userFile);
      return 1;
    }
  }

  fclose(userFile);
  return 0;
}

// write a new user record to user.txt, password will be XOR encrypted and
// stored in hex format for text safety.  returns 0 on success, -1 on failure
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

// update an existing user record in user.txt by matching the username.
int updateUserRecord(User updatedUser) {
  FILE *sourceFile = NULL;
  FILE *tempFile = NULL;
  int status = -1;
  int found = 0;
  int passLen = (int)strlen(updatedUser.password);
  char line[USER_LINE_BUF_SIZE];
  String parsedUser;
  char encryptedPass[MAX_HEX_PASS_LEN + 1];
  char selectedEncryptedPass[MAX_HEX_PASS_LEN + 1];
  Date parsedDate;
  Date selectedDate;
  int parsedRole;
  int selectedRole;
  unsigned char xorPass[MAX_PASSWORD_LEN];

  // only proceed if there's a valid username to match for update
  if (strlen(updatedUser.user) > 0) {
    // open user.txt for reading and a temp file for writing the updated data
    sourceFile = fopen("user.txt", "r");
    tempFile = fopen("user.tmp", "w");

    // read through each user record, write to temp file with updates for the
    // matching username
    if (sourceFile != NULL && tempFile != NULL) {
      // user data format in user.txt:
      while (fgets(line, sizeof(line), sourceFile) != NULL) {
        int parsed = sscanf(line, "%31[^:]:%254[^:]:%d-%d-%d:%d", parsedUser,
                            encryptedPass, &parsedDate.year, &parsedDate.month,
                            &parsedDate.day, &parsedRole);

        // if this line matches the username we want to update, write the
        // updated
        if (parsed == 6 && strcmp(parsedUser, updatedUser.user) == 0) {
          strcpy(selectedEncryptedPass, encryptedPass);
          selectedDate = parsedDate;
          selectedRole = parsedRole;

          // if a new password is provided, encrypt it and prepare to update the
          // record
          if (passLen > 0 && passLen <= MAX_PASSWORD_LEN) {
            xorBytes(updatedUser.password, passLen, xorPass);
            if (hexEncode(xorPass, passLen, selectedEncryptedPass,
                          sizeof(selectedEncryptedPass)) != 0) {
              selectedEncryptedPass[0] = '\0';
            }
          }

          // validate and use updated date and role if provided, otherwise keep
          // existing
          if (updatedUser.creationDate.year > 0 &&
              updatedUser.creationDate.month >= 1 &&
              updatedUser.creationDate.month <= 12 &&
              updatedUser.creationDate.day >= 1 &&
              updatedUser.creationDate.day <= 31) {
            selectedDate = updatedUser.creationDate;
          }

          // only allow valid role updates, otherwise keep existing
          if ((int)updatedUser.role == SUPPLIER ||
              (int)updatedUser.role == RECEIVER) {
            selectedRole = (int)updatedUser.role;
          }

          // if we have a valid new encrypted password, write the updated record
          // to the temp file, otherwise write the original line unmodified
          if (selectedEncryptedPass[0] != '\0') {
            fprintf(tempFile, "%s:%s:%04d-%02d-%02d:%d\n", updatedUser.user,
                    selectedEncryptedPass, selectedDate.year,
                    selectedDate.month, selectedDate.day, selectedRole);
            found = 1;
          } else {
            fputs(line, tempFile);
          }
        } else {
          fputs(line, tempFile);
        }
      }
    }

    // close files and if we found the user to update, replace the original file
    // with the temp file containing the updates
    if (sourceFile != NULL) {
      fclose(sourceFile);
    }
    if (tempFile != NULL) {
      fclose(tempFile);
    }

    // if we found the user and successfully replaced the original file, return
    // success, otherwise clean up the temp file and return failure
    if (found == 1) {
      if (remove("user.txt") == 0 && rename("user.tmp", "user.txt") == 0) {
        status = 0;
      } else {
        remove("user.tmp");
      }
    } else {
      remove("user.tmp");
    }
  }

  return status;
}