#ifndef USER_FILE_HANDLER_H
#define USER_FILE_HANDLER_H

#include "user_system.h"

int getUser(String user, StringLong pass, User *out);
int usernameExists(String user, int *outExists);
int writeUser(User *user);
int updateUserRecord(User updatedUser);

#endif