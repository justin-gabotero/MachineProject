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

// This handles the user system, including registration, login, and password
// recovery. It defines the User struct and related functions for managing user
// accounts.

// Edits the user information based on input and updates the user record.
int editUser(User *user, User *in);
// Registers a new user with the provided information.
int registerUser(User *user);
// Prompts the user for login credentials and attempts to log them in.
int registerPrompt(void);
// Logs in a user with the provided username and password, returning a User
// pointer on success or NULL on failure.
User *loginUser(String user, StringLong pass);
// Prompts the user for login credentials and attempts to log them in, returning
// a User pointer on success or NULL on failure.
// TODO: use int return value and output parameter instead of returning a
// pointer.
User *loginPrompt(void);
// Prompts the user for their username and initiates the password recovery
// process. Returns 0 on success, -1 on failure, and -2 when cancelled.
int recoverPasswordPrompt(void);
// Resets the user's password to the new password provided.
int resetUserPassword(User *user, StringLong newPass);

#endif