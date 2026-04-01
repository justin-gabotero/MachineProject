#include "user_file_handler.h"

#include <stdio.h>
#include <string.h>

#define XOR_KEY 0x67
#define USER_LINE_BUF_SIZE 512
#define MAX_PASSWORD_LEN 127
#define MAX_HEX_PASS_LEN (MAX_PASSWORD_LEN * 2)

/**
 * @brief XORs a sequence of bytes using the internal key.
 * @param input Source bytes as a character buffer.
 * @param len Number of bytes to process.
 * @param output Destination buffer for XORed bytes.
 */
static void xorBytes(const char *input, int len, unsigned char *output) {
  for (int i = 0; i < len; i++) {
    output[i] = ((unsigned char)input[i]) ^ XOR_KEY;
  }
}

/**
 * @brief Converts a hex character to its numeric value.
 * @param ch Hex character in [0-9a-fA-F].
 * @return Numeric value 0-15, or -1 if the character is not hex.
 */
static int hexValue(char ch) {
  int value = -1;

  if (ch >= '0' && ch <= '9') {
    value = ch - '0';
  } else if (ch >= 'a' && ch <= 'f') {
    value = 10 + (ch - 'a');
  } else if (ch >= 'A' && ch <= 'F') {
    value = 10 + (ch - 'A');
  }

  return value;
}

/**
 * @brief Validates whether a string is a well-formed hex sequence.
 * @param input Candidate hex string.
 * @return 1 if valid and even-length, otherwise 0.
 */
static int isHexString(const char *input) {
  int len = (int)strlen(input);
  int isValid = 1;

  if (len == 0 || len % 2 != 0) {
    isValid = 0;
  }

  for (int i = 0; i < len && isValid == 1; i++) {
    if (hexValue(input[i]) < 0) {
      isValid = 0;
    }
  }

  return isValid;
}

/**
 * @brief Encodes binary data into uppercase hexadecimal text.
 * @param input Source byte buffer.
 * @param len Number of source bytes.
 * @param output Destination C-string buffer.
 * @param outputSize Capacity of @p output in bytes.
 * @return 0 on success, -1 if the output buffer is too small.
 */
static int hexEncode(const unsigned char *input, int len, char *output,
                     int outputSize) {
  const char hexDigits[] = "0123456789ABCDEF";
  int status = 0;

  if (outputSize < (len * 2) + 1) {
    status = -1;
  }

  if (status == 0) {
    for (int i = 0; i < len; i++) {
      output[i * 2] = hexDigits[(input[i] >> 4) & 0x0F];
      output[i * 2 + 1] = hexDigits[input[i] & 0x0F];
    }

    output[len * 2] = '\0';
  }

  return status;
}

/**
 * @brief Decodes a hexadecimal string into raw bytes.
 * @param input Source hex C-string.
 * @param output Destination byte buffer.
 * @param outputSize Capacity of @p output in bytes.
 * @param outLen Output parameter set to decoded byte count on success.
 * @return 0 on success, -1 on invalid hex input or insufficient output space.
 */
static int hexDecode(const char *input, unsigned char *output, int outputSize,
                     int *outLen) {
  int len = (int)strlen(input);
  int status = 0;

  if (len % 2 != 0 || outputSize < len / 2) {
    status = -1;
  }

  for (int i = 0; i < len && status == 0; i += 2) {
    int hi = hexValue(input[i]);
    int lo = hexValue(input[i + 1]);

    if (hi < 0 || lo < 0) {
      status = -1;
    } else {
      output[i / 2] = (unsigned char)((hi << 4) | lo);
    }
  }

  if (status == 0) {
    *outLen = len / 2;
  }

  return status;
}

/**
 * @brief XOR-transforms a null-terminated string into another buff er.
 * @param input Source plaintext or XOR text.
 * @param output Destination buffer; must have room for input length + null.
 */
static void xorEncrypt(char *input, char *output) {
  int len = (int)strlen(input);
  xorBytes(input, len, (unsigned char *)output);
  output[len] = '\0';
}

/**
 * @brief Authenticates a user from records stored in user.txt.
 * @param user Username to search.
 * @param pass Password to verify.
 * @param out Output user record when credentials match.
 * @return 0 on successful authentication, -1 on I/O or invalid input, -2 when
 * no match is found.
 */
int getUser(String user, StringLong pass, User *out) {
  int status = -2;
  int keepSearching = 1;
  char buf[USER_LINE_BUF_SIZE];
  FILE *userFile = NULL;

  String parsedUser;
  Date parsedDate;
  Role parsedRole;

  char encryptedPass[MAX_HEX_PASS_LEN + 1];
  StringLong decryptedPass;
  unsigned char xorPass[MAX_PASSWORD_LEN];

  if (user == NULL || pass == NULL || out == NULL) {
    status = -1;
  }

  if (status == -2) {
    userFile = fopen("user.txt", "r");
    if (userFile == NULL) {
      status = -1;
    }
  }

  if (status == -2) {
    // user data format stored in user.txt, one user per line
    // username:encrypted_password:YYYY-MM-DD:role
    while (keepSearching == 1 && fgets(buf, sizeof(buf), userFile) != NULL) {
      int parsed = sscanf(buf, "%31[^:]:%254[^:]:%d-%d-%d:%d", parsedUser,
                          encryptedPass, &parsedDate.year, &parsedDate.month,
                          &parsedDate.day, (int *)&parsedRole);
      int decoded = 1;

      if (parsed == 6) {
        // new format: HEX(XOR(raw_password)).
        if (isHexString(encryptedPass)) {
          int xorLen = 0;
          if (hexDecode(encryptedPass, xorPass, sizeof(xorPass), &xorLen) ==
              0) {
            for (int i = 0; i < xorLen; i++) {
              decryptedPass[i] = (char)(xorPass[i] ^ XOR_KEY);
            }
            decryptedPass[xorLen] = '\0';
          } else {
            decoded = 0;
          }
        } else {
          // backward compatibility for existing raw XOR entries.
          xorEncrypt(encryptedPass, decryptedPass);
        }

        // compare if input username and password match
        if (decoded == 1 && strcmp(parsedUser, user) == 0 &&
            strcmp(decryptedPass, pass) == 0) {
          strcpy(out->user, parsedUser);
          out->creationDate = parsedDate;
          out->role = parsedRole;
          status = 0;
          keepSearching = 0;
        }
      }
    }
  }

  if (userFile != NULL) {
    fclose(userFile);
  }

  return status;
}

/**
 * @brief Checks whether a username already exists in user.txt.
 * @param user Username to query.
 * @param outExists Output flag set to 1 when found, otherwise 0.
 * @return 0 on successful check, -1 on invalid input.
 */
int usernameExists(String user, int *outExists) {
  int status = -1;
  int inputsValid = 0;
  StringLong buf;
  FILE *userFile = NULL;
  String parsedUser;

  if (user != NULL && outExists != NULL) {
    inputsValid = 1;
  }

  if (inputsValid == 1) {
    *outExists = 0;
    userFile = fopen("user.txt", "r");

    if (userFile == NULL) {
      // Missing file means no records yet; treat as a successful check.
      status = 0;
    }
  }

  if (userFile != NULL) {
    status = 0;

    // each line starts with username before the first ':'
    while (fgets(buf, sizeof(buf), userFile) != NULL && *outExists == 0) {
      // compare parsed username with requested username
      if (sscanf(buf, "%31[^:]", parsedUser) == 1 &&
          strcmp(parsedUser, user) == 0) {
        *outExists = 1;
      }
    }
  }

  if (userFile != NULL) {
    fclose(userFile);
  }

  return status;
}

/**
 * @brief Appends a user record to user.txt using hex-encoded XOR password.
 * @param user User record to write.
 * @return 0 on success, -1 on validation, encoding, or file-write failure.
 */
int writeUser(User *user) {
  FILE *userFile = NULL;
  unsigned char xorPass[MAX_PASSWORD_LEN];
  char encryptedPass[MAX_HEX_PASS_LEN + 1];
  int status = 0;
  int passLen = 0;

  if (user == NULL) {
    status = -1;
  }

  if (status == 0) {
    passLen = (int)strlen(user->password);
    if (passLen > MAX_PASSWORD_LEN) {
      printf("Password too long.\n");
      status = -1;
    }
  }

  if (status == 0) {
    // store HEX(XOR(password)) so the file remains text-safe
    xorBytes(user->password, passLen, xorPass);
    if (hexEncode(xorPass, passLen, encryptedPass, sizeof(encryptedPass)) !=
        0) {
      printf("Could not encode password.\n");
      status = -1;
    }
  }

  if (status == 0) {
    // add new user entry to the end of user.txt
    userFile = fopen("user.txt", "a");
    if (userFile == NULL) {
      printf("Could not open user.txt\n");
      status = -1;
    }
  }

  if (status == 0) {
    // user data format in user.txt:
    // username:encrypted_password:YYYY-MM-DD:role
    fprintf(userFile, "%s:%s:%04d-%02d-%02d:%d\n", user->user, encryptedPass,
            user->creationDate.year, user->creationDate.month,
            user->creationDate.day, (int)user->role);
  }

  if (userFile != NULL) {
    fclose(userFile);
  }

  return status;
}

/**
 * @brief Updates an existing user record by username using a temporary file.
 * @param updatedUser User fields to apply to the matching record.
 * @return 0 when the record is updated and file replacement succeeds, -1
 * otherwise.
 */
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