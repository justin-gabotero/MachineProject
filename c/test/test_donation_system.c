#include "../src/donation_system.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define TEST_DONATION_FILE "donation.txt"
#define TEST_BACKUP_FILE "donation.txt.bak_test"
#define EPSILON 0.000001

typedef struct {
  const char *functionName;
  const char *description;
  char input[128];
  char expected[128];
  char actual[128];
  int passed;
} TestResult;

static int fileExists(const char *path) {
  FILE *file = NULL;
  int exists = 0;

  if (path != NULL) {
    file = fopen(path, "r");
    if (file != NULL) {
      exists = 1;
      fclose(file);
    }
  }

  return exists;
}

static Donation makeValidDonation(void) {
  Donation donation;

  memset(&donation, 0, sizeof(donation));
  strcpy(donation.donor.user, "supplier01");
  strcpy(donation.foodType, "Bread");
  donation.zone = ZONE_DLSU_MAIN;
  strcpy(donation.location, "North Gate");
  donation.donationDate.year = 2026;
  donation.donationDate.month = 3;
  donation.donationDate.day = 29;
  donation.expirationDate.year = 2026;
  donation.expirationDate.month = 3;
  donation.expirationDate.day = 30;
  donation.weight = 1000;
  donation.quantity = 1;

  return donation;
}

static void setResult(TestResult *result, const char *functionName,
                      const char *description, const char *input,
                      const char *expected, const char *actual, int passed) {
  if (result != NULL) {
    result->functionName = functionName;
    result->description = description;
    snprintf(result->input, sizeof(result->input), "%s", input);
    snprintf(result->expected, sizeof(result->expected), "%s", expected);
    snprintf(result->actual, sizeof(result->actual), "%s", actual);
    result->passed = passed;
  }
}

static void testCreateDonationNullOut(TestResult *result) {
  Donation input = makeValidDonation();
  int actualValue = createDonation(input, NULL);
  int passed = actualValue == -1;
  char actual[64];

  snprintf(actual, sizeof(actual), "%d", actualValue);
  setResult(result, "createDonation", "null output pointer",
            "out=NULL, weight=1000, qty=1", "-1", actual, passed);
}

static void testCreateDonationInvalidWeight(TestResult *result) {
  Donation input = makeValidDonation();
  Donation out;
  int actualValue = 0;
  int passed = 0;
  char actual[64];

  input.weight = 0;
  actualValue = createDonation(input, &out);
  passed = actualValue == -1;

  snprintf(actual, sizeof(actual), "%d", actualValue);
  setResult(result, "createDonation", "weight=0 edge case", "weight=0, qty=1",
            "-1", actual, passed);
}

static void testCreateDonationInvalidDateMonth(TestResult *result) {
  Donation input = makeValidDonation();
  Donation out;
  int actualValue = 0;
  int passed = 0;
  char actual[64];

  input.expirationDate.month = 13;
  actualValue = createDonation(input, &out);
  passed = actualValue == -1;

  snprintf(actual, sizeof(actual), "%d", actualValue);
  setResult(result, "createDonation", "invalid month edge case",
            "expirationMonth=13", "-1", actual, passed);
}

static void testCreateDonationSuccess(TestResult *result) {
  Donation input = makeValidDonation();
  Donation out;
  int status = -1;
  int copiedFields = 0;
  int passed = 0;
  char actual[64];

  memset(&out, 0, sizeof(out));

  status = createDonation(input, &out);
  copiedFields = strcmp(out.donor.user, input.donor.user) == 0 &&
                 strcmp(out.foodType, input.foodType) == 0 &&
                 out.zone == input.zone &&
                 strcmp(out.location, input.location) == 0 &&
                 out.quantity == input.quantity && out.weight == input.weight;

  passed = status == 0 && copiedFields == 1;
  snprintf(actual, sizeof(actual), "status=%d copied=%d", status, copiedFields);
  setResult(result, "createDonation", "valid donation",
            "valid donor/food/date/zone/location", "status=0 copied=1", actual,
            passed);
}

static void
testComputeDonationWasteReductionInvalidQuantity(TestResult *result) {
  Donation input = makeValidDonation();
  double actualValue = 0.0;
  int passed = 0;
  char actual[64];

  input.quantity = 0;
  actualValue = computeDonationWasteReduction(input);
  passed = fabs(actualValue - 0.0) < EPSILON;

  snprintf(actual, sizeof(actual), "%.2f", actualValue);
  setResult(result, "computeDonationWasteReduction", "quantity=0", "qty=0",
            "0.00", actual, passed);
}

static void testComputeDonationWasteReductionNormal(TestResult *result) {
  Donation input = makeValidDonation();
  double actualValue = 0.0;
  int passed = 0;
  char actual[64];

  input.weight = 2500;
  actualValue = computeDonationWasteReduction(input);
  passed = fabs(actualValue - 2.5) < EPSILON;

  snprintf(actual, sizeof(actual), "%.2f", actualValue);
  setResult(result, "computeDonationWasteReduction", "normal",
            "weight=2500, qty=1", "2.50", actual, passed);
}

static void testComputeTotalWasteReductionMixed(TestResult *result) {
  Donation list[3];
  double actualValue = 0.0;
  int passed = 0;
  char actual[64];

  list[0] = makeValidDonation();
  list[1] = makeValidDonation();
  list[2] = makeValidDonation();

  list[0].weight = 1000;
  list[0].quantity = 1;

  list[1].weight = 700;
  list[1].quantity = 0;

  list[2].weight = 2800;
  list[2].quantity = 4;

  actualValue = computeTotalWasteReduction(list, 3);
  passed = fabs(actualValue - 3.8) < EPSILON;

  snprintf(actual, sizeof(actual), "%.2f", actualValue);
  setResult(result, "computeTotalWasteReduction", "mixed valid/invalid",
            "weights=[1000,700,2800], qty=[1,0,4]", "3.80", actual, passed);
}

static void testLoadDonationSortAndFilter(TestResult *result) {
  Donation list[8];
  FILE *file = NULL;
  int hadOriginal = 0;
  int movedOriginal = 0;
  int writeOk = 0;
  int restored = 0;
  int status = -1;
  char actual[96];

  hadOriginal = fileExists(TEST_DONATION_FILE);
  if (hadOriginal == 1) {
    if (rename(TEST_DONATION_FILE, TEST_BACKUP_FILE) == 0) {
      movedOriginal = 1;
    }
  }

  file = fopen(TEST_DONATION_FILE, "w");
  if (file != NULL) {
    fprintf(file, "alice:Rice:2:2026-03-01:2026-03-05:1200:0:Gate A\n");
    fprintf(file, "bad:Milk:0:2026-03-08:2026-03-10:500:1:Gate B\n");
    fprintf(file, "charlie:Bread:4:2026-03-25:2026-03-29:2500:2:Gate C\n");
    fprintf(file, "malformed line\n");
    fclose(file);
    writeOk = 1;
  }

  memset(list, 0, sizeof(list));
  if (writeOk == 1) {
    loadDonation(list, 8);

    if (strcmp(list[0].donor.user, "charlie") == 0 &&
        strcmp(list[1].donor.user, "alice") == 0 &&
        strlen(list[2].donor.user) == 0) {
      status = 0;
    }
  }

  remove(TEST_DONATION_FILE);

  if (movedOriginal == 1) {
    if (rename(TEST_BACKUP_FILE, TEST_DONATION_FILE) == 0) {
      restored = 1;
    }
  } else {
    restored = 1;
  }

  snprintf(actual, sizeof(actual), "status=%d restored=%d", status, restored);
  setResult(result, "loadDonation", "filters invalid + sorts desc",
            "3 lines + 1 malformed in donation.txt", "status=0 restored=1",
            actual, status == 0 && restored == 1);
}

static void printResults(TestResult results[], int count) {
  int passedCount = 0;

  printf("\n===============================================================\n");
  printf("Donation System Test Results\n");
  printf("===============================================================\n");
  printf("%-20s | %-11s | %-35s | %-12s | %-16s | %-16s | %-3s\n",
         "Function Name", "Test Case #", "Description", "Input",
         "Expected Output", "Actual Output", "P/F");
  printf("----------------------------------------------------------------"
         "---------------------------------------------\n");

  for (int i = 0; i < count; i++) {
    const char *statusTag = results[i].passed ? "P" : "F";

    printf("%-20s | %-11d | %-35s | %-12s | %-16s | %-16s | %-3s\n",
           results[i].functionName, i + 1, results[i].description,
           results[i].input, results[i].expected, results[i].actual, statusTag);

    if (results[i].passed) {
      passedCount++;
    }
  }

  printf("===============================================================\n");
  printf("Summary: %d/%d tests passed\n", passedCount, count);
  printf("===============================================================\n\n");
}

int main(void) {
  TestResult results[8];
  int count = 0;
  int failedCount = 0;

  testCreateDonationNullOut(&results[count++]);
  testCreateDonationInvalidWeight(&results[count++]);
  testCreateDonationInvalidDateMonth(&results[count++]);
  testCreateDonationSuccess(&results[count++]);
  testComputeDonationWasteReductionInvalidQuantity(&results[count++]);
  testComputeDonationWasteReductionNormal(&results[count++]);
  testComputeTotalWasteReductionMixed(&results[count++]);
  testLoadDonationSortAndFilter(&results[count++]);

  printResults(results, count);

  for (int i = 0; i < count; i++) {
    if (results[i].passed == 0) {
      failedCount++;
    }
  }

  return failedCount;
}
