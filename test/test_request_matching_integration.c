#include "../src/donation_system.h"

#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 512

static int countLoadedDonations(Donation list[], int max) {
  int count = 0;

  for (int i = 0; i < max; i++) {
    if (strlen(list[i].donor.user) > 0) {
      count++;
    }
  }

  return count;
}

static int countLoadedRequests(Request list[], int max) {
  int count = 0;

  for (int i = 0; i < max; i++) {
    if (strlen(list[i].requester.user) > 0) {
      count++;
    }
  }

  return count;
}

static void printCaseResult(int index, const char *description,
                            const char *input, int passed) {
  printf("%-20s | %-11d | %-35s | %-12s | %-16s | %-16s | %-3s\n",
         "integration", index, description, input, "PASS",
         passed ? "PASS" : "FAIL", passed ? "P" : "F");
}

static int writeInputFile(const char *path, const char *content) {
  FILE *file = fopen(path, "w");
  int status = -1;

  if (file != NULL) {
    fputs(content, file);
    fclose(file);
    status = 0;
  }

  return status;
}

int main(void) {
  User supplier;
  User receiver;
  Donation donations[MAX_ITEMS];
  Request requests[MAX_ITEMS];
  int requestStatus = -1;
  int donationStatus = -1;
  int requestStatusInitial = -1;
  int donationStatusInitial = -1;
  int requestStatusInvalid = -1;
  int donationStatusInvalid = -1;
  int donationStatusLarge = -1;
  int donationStatusNonNumeric = -1;
  int donationCount = 0;
  int requestCount = 0;
  int donationCountAfterInitial = 0;
  int requestCountAfterInitial = 0;
  int donationCountAfterInvalid = 0;
  int requestCountAfterInvalid = 0;
  int donationCountAfterLarge = 0;
  int donationCountAfterNonNumeric = 0;
  int case1Passed = 0;
  int case2Passed = 0;
  int case3Passed = 0;
  int case4Passed = 0;
  int case5Passed = 0;
  int case6Passed = 0;
  int case7Passed = 0;
  int passedCount = 0;
  int totalCases = 7;
  int requestCountBeforeInvalid = 0;
  int donationCountBeforeInvalid = 0;
  int donationCountBeforeNonNumeric = 0;

  memset(&supplier, 0, sizeof(supplier));
  memset(&receiver, 0, sizeof(receiver));
  memset(donations, 0, sizeof(donations));
  memset(requests, 0, sizeof(requests));

  strcpy(supplier.user, "sup_test");
  supplier.role = SUPPLIER;

  strcpy(receiver.user, "recv_test");
  receiver.role = RECEIVER;

  remove("donation.txt");
  remove("request.txt");

  if (writeInputFile("req_input.txt", "2\n") != 0) {
    printf("FAIL: cannot create req_input.txt\n");
    return 1;
  }

  if (freopen("req_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for request flow\n");
    return 1;
  }

  requestStatus = createRequestFlow(&receiver);
  requestStatusInitial = requestStatus;

  if (writeInputFile("don_input.txt", "Bread\n"
                                      "5\n"
                                      "2000\n"
                                      "12-31-2026\n"
                                      "2\n"
                                      "WH Taft\n") != 0) {
    printf("FAIL: cannot create don_input.txt\n");
    return 1;
  }

  if (freopen("don_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for donation flow\n");
    return 1;
  }

  donationStatus = createDonationFlow(&supplier);
  donationStatusInitial = donationStatus;

  loadDonation(donations, MAX_ITEMS);
  loadRequest(requests, MAX_ITEMS);
  donationCount = countLoadedDonations(donations, MAX_ITEMS);
  requestCount = countLoadedRequests(requests, MAX_ITEMS);
  donationCountAfterInitial = donationCount;
  requestCountAfterInitial = requestCount;

  requestCountBeforeInvalid = requestCount;
  donationCountBeforeInvalid = donationCount;

  if (writeInputFile("req_invalid_input.txt", "9\n") != 0) {
    printf("FAIL: cannot create req_invalid_input.txt\n");
    return 1;
  }

  if (freopen("req_invalid_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for invalid request flow\n");
    return 1;
  }

  requestStatus = createRequestFlow(&receiver);
  requestStatusInvalid = requestStatus;
  loadRequest(requests, MAX_ITEMS);
  requestCount = countLoadedRequests(requests, MAX_ITEMS);
  requestCountAfterInvalid = requestCount;

  if (writeInputFile("don_invalid_input.txt", "Rice\n"
                                              "0\n"
                                              "1000\n"
                                              "12-31-2026\n"
                                              "2\n"
                                              "Edge Invalid Quantity\n") != 0) {
    printf("FAIL: cannot create don_invalid_input.txt\n");
    return 1;
  }

  if (freopen("don_invalid_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for invalid donation flow\n");
    return 1;
  }

  donationStatus = createDonationFlow(&supplier);
  donationStatusInvalid = donationStatus;
  loadDonation(donations, MAX_ITEMS);
  donationCount = countLoadedDonations(donations, MAX_ITEMS);
  donationCountAfterInvalid = donationCount;

  if (writeInputFile("don_large_input.txt", "Canned Goods\n"
                                            "1000000\n"
                                            "2000000000\n"
                                            "12-31-2026\n"
                                            "5\n"
                                            "Large Volume Hub\n") != 0) {
    printf("FAIL: cannot create don_large_input.txt\n");
    return 1;
  }

  if (freopen("don_large_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for large donation flow\n");
    return 1;
  }

  donationStatus = createDonationFlow(&supplier);
  donationStatusLarge = donationStatus;
  loadDonation(donations, MAX_ITEMS);
  donationCount = countLoadedDonations(donations, MAX_ITEMS);
  donationCountAfterLarge = donationCount;

  donationCountBeforeNonNumeric = donationCountAfterLarge;

  if (writeInputFile("don_non_numeric_input.txt",
                     "2395029gheiorgpa\n"
                     "voehso\n"
                     "1000\n"
                     "12-31-2026\n"
                     "2\n"
                     "Invalid Quantity Case\n") != 0) {
    printf("FAIL: cannot create don_non_numeric_input.txt\n");
    return 1;
  }

  if (freopen("don_non_numeric_input.txt", "r", stdin) == NULL) {
    printf("FAIL: cannot redirect stdin for non-numeric donation flow\n");
    return 1;
  }

  donationStatus = createDonationFlow(&supplier);
  donationStatusNonNumeric = donationStatus;
  loadDonation(donations, MAX_ITEMS);
  donationCount = countLoadedDonations(donations, MAX_ITEMS);
  donationCountAfterNonNumeric = donationCount;

  case1Passed = (requestStatusInitial == 0 && requestCountAfterInitial >= 1);
  case2Passed = (donationStatusInitial == 0 && donationCountAfterInitial >= 1);
  case3Passed = (zoneMatch(ZONE_TAFT, ZONE_TAFT) == 1 &&
                 zoneMatch(ZONE_MALATE, ZONE_DLSU_MAIN) == 1 &&
                 zoneMatch(ZONE_PACO, ZONE_DLSU_MAIN) == 0);
  case4Passed = (requestStatusInvalid == -1 &&
                 requestCountAfterInvalid == requestCountBeforeInvalid);
  case5Passed = (donationStatusInvalid == -1 &&
                 donationCountAfterInvalid == donationCountBeforeInvalid);
  case6Passed = (donationStatusLarge == 0 &&
                 donationCountAfterLarge >= donationCountBeforeInvalid + 1);
  case7Passed = (donationStatusNonNumeric == -1 &&
                 donationCountAfterNonNumeric == donationCountBeforeNonNumeric);

  printf("\n==================================================================="
         "============================\n");
  printf("Request Matching Integration Test Results\n");
  printf("====================================================================="
         "==========================\n");
  printf("%-20s | %-11s | %-35s | %-12s | %-16s | %-16s | %-3s\n",
         "Function Name", "Test Case #", "Description", "Input",
         "Expected Output", "Actual Output", "P/F");
  printf("---------------------------------------------------------------------"
         "--------------------------\n");

  printCaseResult(1, "request creation and persistence", "zoneChoice=2",
                  case1Passed);
  printCaseResult(2, "donation creation and persistence",
                  "food=Bread,qty=5,wt=2000,zone=2", case2Passed);
  printCaseResult(3, "zone matching matrix accuracy",
                  "Taft-Taft,Malate-Main,Paco-Main", case3Passed);
  printCaseResult(4, "invalid request zone number rejected", "zoneChoice=9",
                  case4Passed);
  printCaseResult(5, "invalid donation quantity rejected",
                  "food=Rice,qty=0,wt=1000,zone=2", case5Passed);
  printCaseResult(6, "large valid donation accepted",
                  "food=Canned,qty=1000000,wt=2000000000,zone=5", case6Passed);
  printCaseResult(7, "non-numeric quantity rejected",
                  "food=2395029gheiorgpa,qty=voehso", case7Passed);

  if (case1Passed) {
    passedCount++;
  }
  if (case2Passed) {
    passedCount++;
  }
  if (case3Passed) {
    passedCount++;
  }
  if (case4Passed) {
    passedCount++;
  }
  if (case5Passed) {
    passedCount++;
  }
  if (case6Passed) {
    passedCount++;
  }
  if (case7Passed) {
    passedCount++;
  }

  printf("Summary: %d/%d tests passed\n", passedCount, totalCases);

  remove("req_input.txt");
  remove("don_input.txt");
  remove("req_invalid_input.txt");
  remove("don_invalid_input.txt");
  remove("don_large_input.txt");
  remove("don_non_numeric_input.txt");
  remove("donation.txt");
  remove("request.txt");

  if (passedCount == totalCases) {
    printf("INTEGRATION RESULT: PASS\n");
    return 0;
  }

  printf("INTEGRATION RESULT: FAIL\n");
  return 1;
}
