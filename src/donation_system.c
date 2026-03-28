#include "donation_system.h"
#include "user_input.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

// THOUGHT: readCreationDate and readDonationDate are currently identical, we
// can probably refactor them into a single function that can be used for both
// purposes.
static int readDonationDate(Date *outDate) {
  int status = -1;
  time_t now = 0;
  struct tm *localNow = NULL;

  if (outDate != NULL) {
    now = time(NULL);
    if (now != (time_t)-1) {
      localNow = localtime(&now);
      if (localNow != NULL) {
        outDate->year = localNow->tm_year + 1900;
        outDate->month = localNow->tm_mon + 1;
        outDate->day = localNow->tm_mday;
        status = 0;
      }
    }
  }

  return status;
}

// Function to prompt the user for donation details and create a Donation record
// @param donor, the User struct representing the donor making the donation
// @param outDonation, a pointer to a Donation struct where the created donation
// record will be stored
int addDonationPrompt(User donor, Donation *outDonation) {

  Donation temp;
  char buf[128];
  int status = 0;
  temp.donor =
      donor; // Set the donor field of the temp variable to the input donor

  if (readDonationDate(&temp.donationDate) != 0) {
    return -1;
  }

  // Ask the user for the type of food they want to donate
  printf("Food type: ");
  status = readLine(temp.foodType, sizeof(temp.foodType));
  if (status != 0 || strlen(temp.foodType) == 0) {
    return -1;
  }

  // Ask the user for the quantity of food they want to donate
  printf("Quantity: ");
  status = readLine(buf, sizeof(buf));
  if (status != 0 || strlen(buf) == 0) {
    return -1;
  }
  if (sscanf(buf, "%d", &temp.quantity) != 1 || temp.quantity <= 0) {
    return -1;
  }

  // Ask the user for the weight of food they want to donate
  printf("Total Weight (grams): ");
  status = readLine(buf, sizeof(buf));
  if (status != 0 || strlen(buf) == 0) {
    return -1;
  }
  if (sscanf(buf, "%d", &temp.weight) != 1 || temp.weight <= 0) {
    return -1;
  }

  // Ask the user for the expiration date of the food they want to donate
  printf("Expiration Date(MM-DD-YYYY): ");
  status = readLine(buf, sizeof(buf));
  if (status != 0 || strlen(buf) == 0) {
    return -1;
  }
  if (sscanf(buf, "%d-%d-%d", &temp.expirationDate.month,
             &temp.expirationDate.day, &temp.expirationDate.year) != 3) {
    return -1;
  }

  // Ask the user for the expiration date of the food they want to donate
  printf("Pickup Location; ");
  status = readLine(temp.pickupLocation, sizeof(temp.pickupLocation));
  if (status != 0 || strlen(temp.pickupLocation) == 0) {
    return -1;
  }

  *outDonation = temp;
  return 0;
}

// TODO: refactor to not pre-terminateate the output variable, instead return a
// new variable and assign it to the output variable in the caller function,
// this way we can avoid unnecessary copying of the struct.

// Function to create a Donation record from a given Donation struct, checks if
// the input values are valid and if so copies the input to the output variable
// @param in, a Donation struct containing the details of the donation to be
// created
// @param out, a Donation struct where the created donation record will be
// stored
int createDonation(Donation in, Donation *out) {
  // Checks if donation inputs are all valid and have valid values, if not
  // returns -1 to indicate an error
  if (in.weight <= 0 || in.quantity <= 0 || strlen(in.foodType) == 0 ||
      strlen(in.donor.user) == 0 || strlen(in.pickupLocation) == 0) {
    return -1;
  }

  // checking date validity
  if (in.expirationDate.year < 0 || in.expirationDate.month < 1 ||
      in.expirationDate.month > 12 || in.expirationDate.day < 1 ||
      in.expirationDate.day > 31) {
    return -1;
  }

  // If all inputs are valid, copy the in variable to the output variable
  *out = in;
  return 0;
}

double computeDonationWasteReduction(Donation donation) {
  return 0; // TODO: Placeholder implementation, replace with actual waste
            // reduction calculation logic later.
}

// Append the given donation record to donation.txt
// @param donation, the Donation struct containing the details of the donation
// to be written to the file.
void writeDonation(Donation donation) {
  FILE *file = fopen("donation.txt", "a");
  if (file != NULL) {
    // username:foodType:quantity:donationDate:expirationDate:weight:pickupLocation
    // example line: john_doe:bread:2:2024-06-01:2024-06-05:500:123 Main St
    fprintf(file, "%s:%s:%d:%d-%02d-%02d:%d-%02d-%02d:%d:%s\n",
            donation.donor.user, donation.foodType, donation.quantity,
            donation.donationDate.year, donation.donationDate.month,
            donation.donationDate.day, donation.expirationDate.year,
            donation.expirationDate.month, donation.expirationDate.day,
            donation.weight, donation.pickupLocation);
    fclose(file);
  } else {
    printf("Error: Could not open donation.txt for writing.\n");
  }
}
