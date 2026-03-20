#ifndef USER_SYSTEM_H
#define USER_SYSTEM_H

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

int editUser(User *user, User *in);
int registerUser(User *user);
int registerPrompt(void);
User *loginUser(String user, StringLong pass);
User *loginPrompt(void);
User *recoverPasswordPrompt(void);
int readLine(char *buf, int size);

#endif