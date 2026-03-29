#include "donation_system.h"
#include "user_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Prompts the user for donation details and creates a donation record.
 *
 * This function collects donation information from the user associated with
 * the specified donor and stores the resulting record in `outDonation`.
 *
 * @param donor The `User` representing the donor making the donation.
 * @param[out] outDonation Pointer to a `Donation` structure where the created
 * donation record will be written.
 * @return 0 on success, -1 on invalid input or failed date retrieval.
 */
int addDonationPrompt(User donor, Donation *outDonation) {

  Donation temp;
  char buf[128];
  int status = 0;

  // Check if the output pointer is valid before assigning values.
  if (outDonation == NULL) {
    return -1;
  }

  temp.donor =
      donor; // Set the donor field of the temp variable to the input donor

  if (getCurrentDate(&temp.donationDate) != 0) {
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

  // Ask the user for the pickup location for the food they want to donate
  printf("Pickup Location: ");
  status = readLine(temp.pickupLocation, sizeof(temp.pickupLocation));
  if (status != 0 || strlen(temp.pickupLocation) == 0) {
    return -1;
  }

  *outDonation = temp;
  return 0;
}

/**
 * @brief Validates and creates a donation record from an input donation.
 * @param in Donation containing candidate field values.
 * @param[out] out Pointer to a Donation where validated data is written.
 * @return 0 on success, -1 on invalid fields or null output pointer.
 */
int createDonation(Donation in, Donation *out) {
  // Checks if donation inputs are all valid and have valid values, if not
  // returns -1 to indicate an error

  if (out == NULL) {
    return -1;
  }

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

/**
 * @brief Computes the estimated waste reduction contributed by a donation.
 * @param donation Donation to evaluate.
 * @return Estimated waste reduction value.
 */
double computeDonationWasteReduction(Donation donation) {
  if(donation.weight <= 0 || donation.quantity <= 0){
    return 0.0;
  }

    return donation.weight / 1000.0; // Convert grams to kilograms for waste reduction estimate
  
}

/**
 * @brief Compares two donations by donation date in descending order.
 * @param left Pointer to the left Donation element.
 * @param right Pointer to the right Donation element.
 * @return Negative if left should come before right, positive if after, 0 if
 *         equal.
 */
static int compareDonationDateDesc(const void *left, const void *right) {
  const Donation *a = (const Donation *)left;
  const Donation *b = (const Donation *)right;
  int cmp = 0;

  if (a->donationDate.year != b->donationDate.year) {
    cmp = b->donationDate.year - a->donationDate.year;
  } else if (a->donationDate.month != b->donationDate.month) {
    cmp = b->donationDate.month - a->donationDate.month;
  } else {
    cmp = b->donationDate.day - a->donationDate.day;
  }

  return cmp;
}

/**
 * @brief Appends a donation record to donation.txt.
 * @param donation Donation record to persist.
 */
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

/**
 * @brief Loads donation records from donation.txt into an array.
 * @param[out] list Array of Donation structures that receives loaded records.
 * @param maxCount Maximum number of records to load.
 */
void loadDonation(Donation *list, int maxCount) {
  FILE *file = NULL;
  char line[512];
  int loadedCount = 0;

  // Initialize the list array with empty/default values to ensure all fields
  // are set to known states before loading data from the file.
  if (list != NULL && maxCount > 0) {
    for (int i = 0; i < maxCount; i++) {
      list[i].donor.user[0] = '\0';
      list[i].donor.password[0] = '\0';
      list[i].donor.creationDate.year = 0;
      list[i].donor.creationDate.month = 0;
      list[i].donor.creationDate.day = 0;
      list[i].donor.role = (Role)-1;
      list[i].foodType[0] = '\0';
      list[i].pickupLocation[0] = '\0';
      list[i].donationDate.year = 0;
      list[i].donationDate.month = 0;
      list[i].donationDate.day = 0;
      list[i].expirationDate.year = 0;
      list[i].expirationDate.month = 0;
      list[i].expirationDate.day = 0;
      list[i].weight = 0;
      list[i].quantity = 0;
    }

    // Open donation.txt for reading and load donation records into the list.
    file = fopen("donation.txt", "r");
    if (file != NULL) {
      while (loadedCount < maxCount &&
             fgets(line, sizeof(line), file) != NULL) {
        Donation parsed;
        int matched = 0;

        parsed.donor.user[0] = '\0';
        parsed.donor.password[0] = '\0';
        parsed.donor.creationDate.year = 0;
        parsed.donor.creationDate.month = 0;
        parsed.donor.creationDate.day = 0;
        parsed.donor.role = (Role)-1;
        parsed.foodType[0] = '\0';
        parsed.pickupLocation[0] = '\0';
        parsed.donationDate.year = 0;
        parsed.donationDate.month = 0;
        parsed.donationDate.day = 0;
        parsed.expirationDate.year = 0;
        parsed.expirationDate.month = 0;
        parsed.expirationDate.day = 0;
        parsed.weight = 0;
        parsed.quantity = 0;

        matched =
            sscanf(line, "%31[^:]:%31[^:]:%d:%d-%d-%d:%d-%d-%d:%d:%127[^\n]",
                   parsed.donor.user, parsed.foodType, &parsed.quantity,
                   &parsed.donationDate.year, &parsed.donationDate.month,
                   &parsed.donationDate.day, &parsed.expirationDate.year,
                   &parsed.expirationDate.month, &parsed.expirationDate.day,
                   &parsed.weight, parsed.pickupLocation);

        if (matched == 11 && parsed.quantity > 0 && parsed.weight > 0 &&
            parsed.donationDate.year >= 1970 &&
            parsed.donationDate.year <= 2100 &&
            parsed.donationDate.month >= 1 && parsed.donationDate.month <= 12 &&
            parsed.donationDate.day >= 1 && parsed.donationDate.day <= 31 &&
            parsed.expirationDate.year >= 1970 &&
            parsed.expirationDate.year <= 2100 &&
            parsed.expirationDate.month >= 1 &&
            parsed.expirationDate.month <= 12 &&
            parsed.expirationDate.day >= 1 && parsed.expirationDate.day <= 31) {
          list[loadedCount] = parsed;
          loadedCount++;
        }
      }

      fclose(file);
    }

    // WARN: not sure if we're allowed to use qsort from the standard library,
    // if not we can implement our own.

    // If more than one donation record was loaded, sort the list by donation
    // date in descending order (most recent first) using the
    // compareDonationDateDesc function as the comparison function for qsort.
    if (loadedCount > 1) {
      qsort(list, (size_t)loadedCount, sizeof(Donation),
            compareDonationDateDesc);
    }
  }
}

double computeTotalWasteReduction(Donation list[], int count){
    double totalWaste = 0.0;
    // Iterate through the list of donations and calculate the total waste reduction
    for(int i = 0; i < count; i++){
        totalWaste += computeDonationWasteReduction(list[i]);
    }
    return totalWaste;
}


//Function to compute and print monthly statistics for donations based on a given list of donations, the number of donations in the list, and the specified month and year for which to compute the statistics
//@param list, an array of Donation structs representing the list of donations to analyze
//@param count, the number of donations in the list
//@param year, the year for which to compute the monthly statistics
//@param month, the month for which to compute the monthly statistics  
void computeMonthlyStats(Donation list[], int count, int year, int month){
    int i;
    int monthlyTotalDonations = 0;
    int totalQuantity = 0;
    double totalWaste = 0.0;
    float avgQuantity = 0.0;
    // Iterate through the list of donations and calculate the total donations, total quantity, and total waste reduction for the specified month and year
    for(i = 0; i < count; i++){
        if(list[i].expirationDate.year == year && list[i].expirationDate.month == month){
            //For every donation that matches the specified month and year, increment the monthly total donations, add the quantity to the total quantity, and calculate the waste reduction and add it to the total waste
            monthlyTotalDonations++;
            totalQuantity += list[i].quantity;
            totalWaste += computeDonationWasteReduction(list[i]);
        }
    }
    // If there are any donations, Calculate the average quantity donated per donation for the specified month and year
    if(monthlyTotalDonations > 0){
        avgQuantity = (float)totalQuantity / monthlyTotalDonations;
    }
    //If there are any donations, print the monthly stats including total donations, total quantity donated, and estimated waste reduction. Otherwise, print a message indicating that no donations were found for that month and year
    if(monthlyTotalDonations > 0){
        printf("Monthly Stats for %d-%02d:\n", year, month);
            printf("Total Donations: %d\n", monthlyTotalDonations);
            printf("Total Quantity Donated: %d\n", totalQuantity);
            printf("Estimated Waste Reduction: %.2f kg\n", totalWaste);
            printf("Average Quantity per Donation: %.2f\n", avgQuantity);
        } else {
            printf("No donations found for %d-%02d.\n", year, month);
        }
 }




