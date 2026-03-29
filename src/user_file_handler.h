#ifndef USER_FILE_HANDLER_H
#define USER_FILE_HANDLER_H

#include "user_system.h"

// This file contains functions for handling user data stored in a file.
// It provides functions to read user data, check if a username exists,
// write new user data, and update existing user records.

// Reads a user record by username/password.
// Returns 0 on success, -1 on invalid input or file/read errors,
// and -2 when credentials do not match any record.
int getUser(String user, StringLong pass, User *out);
// Checks if a username already exists in the file.
// Returns 0 on success and writes 0/1 to outExists, or -1 on invalid input.
int usernameExists(String user, int *outExists);
// Writes a new user record to the file.
int writeUser(User *user);
// Updates an existing user record in the file with new data.
int updateUserRecord(User updatedUser);

#endif