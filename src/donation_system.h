#ifndef DONATION_SYSTEM_H
#define DONATION_SYSTEM_H

#include "user_system.h"
#include <stdio.h>

typedef struct {
  User donor;      // the donor user
  String foodType; // type of food
  StringLong pickupLocation;
  Date expirationDate;
  int weight; // weight of the donated food in grams
  int quantity;
} Donation;

int addDonationPrompt(User donor, Donation *outDonation);
int createDonation(Donation in, Donation *out);
void writeDonation(Donation donation);
void loadDonation(Donation list[], int maxCount);
double computeDonationWasteReduction(Donation donation);
double computeTotalWasteReduction(Donation list[], int count);
void computeMonthlyStats(Donation list[], int count, int year, int month);

#endif
