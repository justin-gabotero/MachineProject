#ifndef DONATION_SYSTEM_H
#define DONATION_SYSTEM_H

#include "user_system.h"
#include <stdio.h>

// Zone definitions for DLSU-focused location system
enum Zone {
  ZONE_DLSU_MAIN = 0,
  ZONE_TAFT = 1,
  ZONE_MALATE = 2,
  ZONE_PACO = 3,
  ZONE_ERMITA = 4
};

#define NUM_ZONES 5

// Symmetric distance matrix: values = relative distance (0 = same, higher =
// farther)
static const int ZONE_DISTANCE[NUM_ZONES][NUM_ZONES] = {
    {0, 1, 2, 3, 2}, // DLSU Main
    {1, 0, 1, 2, 1}, // Taft
    {2, 1, 0, 1, 1}, // Malate
    {3, 2, 1, 0, 2}, // Paco
    {2, 1, 1, 2, 0}, // Ermita
};

typedef struct {
  User donor;          // the donor user
  String foodType;     // type of food
  int zone;            // zone enum value (0-4) for distance calculations
  StringLong location; // detailed address text
  Date donationDate;
  Date expirationDate;
  int weight; // weight of the donated food in grams
  int quantity;
} Donation;

// Helper function to get zone name from enum
const char *getZoneName(int zone);

int addDonationPrompt(User donor, Donation *outDonation);
int createDonation(Donation in, Donation *out);
int createDonationFlow(const User *currentUser);
void writeDonation(Donation donation);
void loadDonation(Donation list[], int maxCount);
void viewOwnDonations(const User *currentUser);
void viewAllDonationsList(void);
double computeDonationWasteReduction(Donation donation);
double computeTotalWasteReduction(Donation list[], int count);
void computeMonthlyStats(Donation list[], int count, int year, int month);

#endif
