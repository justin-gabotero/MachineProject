#ifndef USER_FILE_HANDLER_H
#define USER_FILE_HANDLER_H

#include "user_system.h"

void xorEncrypt(char *input, char *output);
int getUser(String user, StringLong pass, User *out);
int usernameExists(String user);
int writeUser(User *user);

#endif