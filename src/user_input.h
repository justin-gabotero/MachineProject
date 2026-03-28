#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "donation_system.h"

// helper function for reading user input from console with support for
// backspace and ctrl+c this is for the menu system functions, selecting a
// process, etc.

// Reads a line of input from the user into the provided buffer.
int readLine(char *buf, int size);
int getCurrentDate(Date *date);

#endif