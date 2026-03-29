#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "donation_system.h"

int readLine(char *buf, int size);
int getCurrentDate(Date *date);
int selectZoneMenu(void);
int selectDonationSortMenu(void);
int userMenu(User *currentUser);

#endif