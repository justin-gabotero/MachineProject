#ifndef USER_FILE_HANDLER_H
#define USER_FILE_HANDLER_H

#include "user_system.h"

// This file contains functions for handling user data stored in a file.
// It provides functions to read user data, check if a username exists,
// write new user data, and update existing user records.

// Reads a user record from the file based on the username and password.
int getUser(String user, StringLong pass, User *out);
// Checks if a username already exists in the file.
int usernameExists(String user);
// Writes a new user record to the file.
int writeUser(User *user);
// Updates an existing user record in the file with new data.
int updateUserRecord(User updatedUser);

#endif