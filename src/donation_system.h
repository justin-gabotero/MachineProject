#ifndef DONATION_SYSTEM_H
#define DONATION_SYSTEM_H

#include <stdio.h>
#include "user_system.h"

typedef struct {
char username [32]; //username of donor
char foodType [32]; //type of food 
int quantity; 
int yearExpiration;
int monthExpiration;
int dayExpiration;
int pickupLocation [128]
} Donation;

int addDonationPrompt(User donor);
int createDonation(Donation in, Donation out);
void writeDomation(Donation donation);
void loadDonation(Donation list[], int maxCount);
double computeDonationWasteReduction(Donation donation);
double computeTotalWasteReduction(Donation list[], int count);
void computeMonthlyStats(Donation list[], int count, int year, int month);

#endif
