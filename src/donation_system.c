#include "donation_system.h"
#include "user_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DONATIONS 512

/**
 * @brief Retrieves the human-readable name of a zone based on its enum value.
 *
 * @param zone The enum value representing the zone.
 * @return const char*
 */
const char *getZoneName(int zone) {
  const char *status = NULL;

  if (zone >= 0 && zone < NUM_ZONES) {
    switch (zone) {
    case ZONE_DLSU_MAIN:
      status = "DLSU Main";
      break;
    case ZONE_TAFT:
      status = "Taft Avenue";
      break;
    case ZONE_MALATE:
      status = "Malate";
      break;
    case ZONE_PACO:
      status = "Paco";
      break;
    case ZONE_ERMITA:
      status = "Ermita";
      break;
    default:
      status = "Unknown";
      break;
    }
  }

  return status;
}

/**
 * @brief Checks if a donation has been successfully loaded with valid data.
 *
 * @param donation Pointer to the donation to check.
 * @return int 1 if the donation is considered loaded with valid data, 0
 * otherwise.
 */
static int isLoadedDonation(const Donation *donation) {
  int status = 0;

  if (donation != NULL) {
    if (strlen(donation->donor.user) > 0 && strlen(donation->foodType) > 0 &&
        donation->zone >= 0 && donation->zone < NUM_ZONES &&
        strlen(donation->location) > 0 && donation->quantity > 0 &&
        donation->weight > 0) {
      status = 1;
    }
  }

  return status;
}

/**
 * @brief Prints a date in the format YYYY-MM-DD.
 *
 * @param date Pointer to the date to print.
 */
static void printDate(const Date *date) {
  if (date != NULL) {
    printf("%04d-%02d-%02d", date->year, date->month, date->day);
  }
}

/**
 * @brief Prints the details of a single donation entry.
 *
 * @param donation Pointer to the donation to print.
 * @param index The index of the donation in the list.
 */
static void printDonationEntry(const Donation *donation, int index) {
  if (donation != NULL) {
    printf("%d. Donor: %s\n", index, donation->donor.user);
    printf("   Food Type: %s\n", donation->foodType);
    printf("   Quantity: %d\n", donation->quantity);
    printf("   Weight (g): %d\n", donation->weight);
    printf("   Donation Date: ");
    printDate(&donation->donationDate);
    printf("\n");
    printf("   Expiration Date: ");
    printDate(&donation->expirationDate);
    printf("\n");
    printf("   Zone: %s\n", getZoneName(donation->zone));
    printf("   Specific Location: %s\n", donation->location);
  }
}

// Forward declarations for comparison functions
static int compareDonationDateDesc(const void *left, const void *right);
static int compareDonationFoodTypeAsc(const void *left, const void *right);
static int compareDonationDonorNameAsc(const void *left, const void *right);

/**
 * @brief Displays a list of all donations in the system with sort options.
 */
void viewAllDonationsList(void) {
  Donation donations[MAX_DONATIONS];
  Donation sorted[MAX_DONATIONS];
  int totalLoaded = 0;
  int shown = 0;
  int sortChoice = -1;

  loadDonation(donations, MAX_DONATIONS);

  // Count loaded donations and copy to sorted array
  for (int i = 0; i < MAX_DONATIONS; i++) {
    if (isLoadedDonation(&donations[i])) {
      sorted[totalLoaded] = donations[i];
      totalLoaded++;
    }
  }

  if (totalLoaded == 0) {
    printf("\n=== All Donations ===\n");
    printf("No donations found.\n");
  } else {
    sortChoice = selectDonationSortMenu();

    if (sortChoice == 0) {
      // Sort by date (newest first) - already in this order from loadDonation
      qsort(sorted, (size_t)totalLoaded, sizeof(Donation),
            compareDonationDateDesc);
      printf("\n=== All Donations (Sorted by Date - Newest First) ===\n");
    } else if (sortChoice == 1) {
      // Sort by food type (A-Z)
      qsort(sorted, (size_t)totalLoaded, sizeof(Donation),
            compareDonationFoodTypeAsc);
      printf("\n=== All Donations (Sorted by Food Type - A-Z) ===\n");
    } else if (sortChoice == 2) {
      // Sort by donor name (A-Z)
      qsort(sorted, (size_t)totalLoaded, sizeof(Donation),
            compareDonationDonorNameAsc);
      printf("\n=== All Donations (Sorted by Donor Name - A-Z) ===\n");
    } else {
      printf("\n=== All Donations ===\n");
      printf("Invalid sort selection.\n");
    }

    if (sortChoice >= 0 && sortChoice <= 2) {
      for (int i = 0; i < totalLoaded; i++) {
        shown++;
        printDonationEntry(&sorted[i], shown);
      }
    }
  }
}

/**
 * @brief Displays a list of donations made by the current user.
 *
 * @param currentUser
 */
void viewOwnDonations(const User *currentUser) {
  Donation donations[MAX_DONATIONS];
  int shown = 0;

  loadDonation(donations, MAX_DONATIONS);

  printf("\n=== Your Donations ===\n");
  for (int i = 0; i < MAX_DONATIONS; i++) {
    if (isLoadedDonation(&donations[i]) && currentUser != NULL &&
        strcmp(donations[i].donor.user, currentUser->user) == 0) {
      shown++;
      printDonationEntry(&donations[i], shown);
    }
  }

  if (shown == 0) {
    printf("No donations found for your account.\n");
  }
}

/**
 * @brief Handles the flow for creating a new donation, including prompting the
 * user for details and validating the input before saving the donation record.
 *
 * @param currentUser Pointer to the logged-in user for creating the donation.
 * @return int
 */
int createDonationFlow(const User *currentUser) {
  Donation input;
  Donation validated;
  int status = -1;

  if (currentUser != NULL && currentUser->role == SUPPLIER) {
    printf("\n=== Create Donation ===\n");
    status = addDonationPrompt(*currentUser, &input);
    if (status == -2) {
      printf("Donation creation cancelled.\n");
    } else if (status != 0) {
      printf("Invalid donation details.\n");
    }

    if (status == 0) {
      status = createDonation(input, &validated);
      if (status != 0) {
        printf("Donation could not be created.\n");
      }
    }

    if (status == 0) {
      writeDonation(validated);
      printf("Donation created successfully.\n");
    }
  }

  return status;
}

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
    status = -1;
  }

  if (status == 0) {
    temp.donor =
        donor; // Set the donor field of the temp variable to the input donor

    if (getCurrentDate(&temp.donationDate) != 0) {
      status = -1;
    }
  }

  if (status == 0) {
    // Ask the user for the type of food they want to donate
    printf("Food type: ");
    status = readLine(temp.foodType, sizeof(temp.foodType));
    if (status != 0 || strlen(temp.foodType) == 0) {
      status = -1;
    }
  }

  if (status == 0) {
    // Ask the user for the quantity of food they want to donate
    printf("Quantity: ");
    status = readLine(buf, sizeof(buf));
    if (status != 0 || strlen(buf) == 0) {
      status = -1;
    }
  }
  if (status == 0 &&
      (sscanf(buf, "%d", &temp.quantity) != 1 || temp.quantity <= 0)) {
    status = -1;
  }

  if (status == 0) {
    // Ask the user for the weight of food they want to donate
    printf("Total Weight (grams): ");
    status = readLine(buf, sizeof(buf));
    if (status != 0 || strlen(buf) == 0) {
      status = -1;
    }
  }
  if (status == 0 &&
      (sscanf(buf, "%d", &temp.weight) != 1 || temp.weight <= 0)) {
    status = -1;
  }

  if (status == 0) {
    // Ask the user for the expiration date of the food they want to donate
    printf("Expiration Date(MM-DD-YYYY): ");
    status = readLine(buf, sizeof(buf));
    if (status != 0 || strlen(buf) == 0) {
      status = -1;
    }
  }
  if (status == 0 &&
      sscanf(buf, "%d-%d-%d", &temp.expirationDate.month,
             &temp.expirationDate.day, &temp.expirationDate.year) != 3) {
    status = -1;
  }

  if (status == 0) {
    // Ask the user to select a zone from the menu
    status = selectZoneMenu();
    if (status >= 0 && status < NUM_ZONES) {
      temp.zone = status;
      status = 0;
    } else {
      status = -1;
    }
  }

  if (status == 0) {
    // Ask the user for the specific location details
    printf("Specific address/location: ");
    status = readLine(temp.location, sizeof(temp.location));
    if (status != 0 || strlen(temp.location) == 0) {
      status = -1;
    }
  }

  if (status == 0) {
    *outDonation = temp;
  }

  if (status != 0) {
    status = -1;
  }

  return status;
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
  int status = 0;

  if (out == NULL) {
    status = -1;
  }

  if (status == 0 &&
      (in.weight <= 0 || in.quantity <= 0 || strlen(in.foodType) == 0 ||
       strlen(in.donor.user) == 0 || in.zone < 0 || in.zone >= NUM_ZONES ||
       strlen(in.location) == 0)) {
    status = -1;
  }

  // checking date validity
  if (status == 0 &&
      (in.expirationDate.year < 0 || in.expirationDate.month < 1 ||
       in.expirationDate.month > 12 || in.expirationDate.day < 1 ||
       in.expirationDate.day > 31)) {
    status = -1;
  }

  // If all inputs are valid, copy the in variable to the output variable
  if (status == 0) {
    *out = in;
  }

  return status;
}

/**
 * @brief Computes the estimated waste reduction contributed by a donation.
 * @param donation Donation to evaluate.
 * @return Estimated waste reduction value.
 */
double computeDonationWasteReduction(Donation donation) {
  double wasteReduction = 0.0;

  if (donation.weight > 0 && donation.quantity > 0) {
    wasteReduction =
        donation.weight / 1000.0; // Convert grams to kilograms estimate
  }

  return wasteReduction;
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
 * @brief Compares two donations by food type in ascending alphabetical order.
 * @param left Pointer to the left Donation element.
 * @param right Pointer to the right Donation element.
 * @return Negative if left should come before right, positive if after, 0 if
 * equal.
 */
static int compareDonationFoodTypeAsc(const void *left, const void *right) {
  const Donation *a = (const Donation *)left;
  const Donation *b = (const Donation *)right;

  return strcmp(a->foodType, b->foodType);
}

/**
 * @brief Compares two donations by donor name in ascending alphabetical order.
 * @param left Pointer to the left Donation element.
 * @param right Pointer to the right Donation element.
 * @return Negative if left should come before right, positive if after, 0 if
 * equal.
 */
static int compareDonationDonorNameAsc(const void *left, const void *right) {
  const Donation *a = (const Donation *)left;
  const Donation *b = (const Donation *)right;

  return strcmp(a->donor.user, b->donor.user);
}

/**
 * @brief Appends a donation record to donation.txt.
 * @param donation Donation record to persist.
 */
void writeDonation(Donation donation) {
  FILE *file = fopen("donation.txt", "a");
  if (file != NULL) {
    // username:foodType:quantity:donationDate:expirationDate:weight:zone:location
    // example line: john_doe:bread:2:2024-06-01:2024-06-05:500:0:North Gate
    fprintf(file, "%s:%s:%d:%d-%02d-%02d:%d-%02d-%02d:%d:%d:%s\n",
            donation.donor.user, donation.foodType, donation.quantity,
            donation.donationDate.year, donation.donationDate.month,
            donation.donationDate.day, donation.expirationDate.year,
            donation.expirationDate.month, donation.expirationDate.day,
            donation.weight, donation.zone, donation.location);

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
      list[i].zone = -1;
      list[i].location[0] = '\0';
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
        parsed.zone = -1;
        parsed.location[0] = '\0';
        parsed.donationDate.year = 0;
        parsed.donationDate.month = 0;
        parsed.donationDate.day = 0;
        parsed.expirationDate.year = 0;
        parsed.expirationDate.month = 0;
        parsed.expirationDate.day = 0;
        parsed.weight = 0;
        parsed.quantity = 0;

        matched =
            sscanf(line, "%31[^:]:%31[^:]:%d:%d-%d-%d:%d-%d-%d:%d:%d:%127[^\n]",
                   parsed.donor.user, parsed.foodType, &parsed.quantity,
                   &parsed.donationDate.year, &parsed.donationDate.month,
                   &parsed.donationDate.day, &parsed.expirationDate.year,
                   &parsed.expirationDate.month, &parsed.expirationDate.day,
                   &parsed.weight, &parsed.zone, parsed.location);

        if (matched == 12 && parsed.quantity > 0 && parsed.weight > 0 &&
            parsed.zone >= 0 && parsed.zone < NUM_ZONES &&
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

    if (loadedCount > 1) {
      qsort(list, (size_t)loadedCount, sizeof(Donation),
            compareDonationDateDesc);
    }
  }
}
/**
 * @brief Computes the total waste reduction for a list of donation records.
 * @param list Array of Donation structures.
 * @param count Number of donation records in the array.
 * @return Total waste reduction.
 */
double computeTotalWasteReduction(Donation list[], int count) {
  double totalWaste = 0.0;
  // Iterate through the list of donations and calculate the total waste
  // reduction
  for (int i = 0; i < count; i++) {
    totalWaste += computeDonationWasteReduction(list[i]);
  }
  return totalWaste;
}

/**
 * @brief Computes and prints monthly statistics for donations in a given month
 * and year.
 * @param list Array of Donation structures.
 * @param count Number of donation records in the array.
 * @param year The year for which to compute statistics.
 * @param month The month for which to compute statistics.
 */
void computeMonthlyStats(Donation list[], int count, int year, int month) {
  int i;
  int monthlyTotalDonations = 0;
  int totalQuantity = 0;
  double totalWaste = 0.0;
  float avgQuantity = 0.0;
  // Iterate through the list of donations and calculate the total donations,
  // total quantity, and total waste reduction for the specified month and year
  for (i = 0; i < count; i++) {
    if (list[i].expirationDate.year == year &&
        list[i].expirationDate.month == month) {
      // For every donation that matches the specified month and year, increment
      // the monthly total donations, add the quantity to the total quantity,
      // and calculate the waste reduction and add it to the total waste
      monthlyTotalDonations++;
      totalQuantity += list[i].quantity;
      totalWaste += computeDonationWasteReduction(list[i]);
    }
  }
  // Calculate the average quantity per month.
  if (monthlyTotalDonations > 0) {
    avgQuantity = (float)totalQuantity / monthlyTotalDonations;
  }
  // If there are any donations, print the monthly statistics including total.
  if (monthlyTotalDonations > 0) {
    printf("Monthly Stats for %d-%02d:\n", year, month);
    printf("Total Donations: %d\n", monthlyTotalDonations);
    printf("Total Quantity Donated: %d\n", totalQuantity);
    printf("Estimated Waste Reduction: %.2f kg\n", totalWaste);
    printf("Average Quantity per Donation: %.2f\n", avgQuantity);
  } else {
    printf("No donations found for %d-%02d.\n", year, month);
  }
}
