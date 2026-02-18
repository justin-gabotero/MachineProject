// Project Title: AnimoConnect
// Author: Gabotero, Justin L.
// CCPROG2 Machine Project

#include <stdio.h>

typedef char String[32];
typedef char StringLong[100];
typedef char StringBuffer[255];

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
} User;

int countFileLines(FILE *fp) {
  int i, lineCount = 0;

  if (fp == NULL) {
    printf("Could not read file.\n");
    return 1;
  }

  for (i = getc(fp); i != EOF; i = getc(fp)) {
    if (i == '\n') {
      lineCount++;
    }
  }

  return lineCount;
}

int userList() {
  StringBuffer buf;
  FILE *userFile;
  int i, lineCount = 0;

  userFile = fopen("user.txt", "r");
  lineCount = countFileLines(userFile);

  fclose(userFile);

  return 0;
}

int registerUser() {
  return 0; // TODO: IMPL
}

int main(void) { return 0; }
