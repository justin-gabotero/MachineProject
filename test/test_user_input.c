#include "../src/user_input.h"

#include <stdio.h>
#include <string.h>

#define TEST_INPUT_FILE "test_input.txt"

typedef struct {
  const char *name;
  char expected[128];
  char actual[128];
  int passed;
} TestResult;

static void setResult(TestResult *result, const char *name,
                      const char *expected, const char *actual, int passed) {
  if (result != NULL) {
    result->name = name;
    snprintf(result->expected, sizeof(result->expected), "%s", expected);
    snprintf(result->actual, sizeof(result->actual), "%s", actual);
    result->passed = passed;
  }
}

static void simulateInput(const char *input) {
  FILE *file = fopen(TEST_INPUT_FILE, "w");
  if (file != NULL) {
    fprintf(file, "%s", input);
    fclose(file);
  }
  freopen(TEST_INPUT_FILE, "r", stdin);
}

static void testReadLineNormal(TestResult *result) {
  char buffer[64];
  int status = -1;
  char actual[128];

  simulateInput("Hello World\n");
  status = readLine(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "status=%d value=%s", status, buffer);
  setResult(result, "readLine: normal string", "status=0 value=Hello World",
            actual, status == 0 && strcmp(buffer, "Hello World") == 0);
}

static void testReadLineEmpty(TestResult *result) {
  char buffer[64];
  int status = -1;
  char actual[128];

  simulateInput("\n");
  status = readLine(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "status=%d value=%s", status, buffer);
  setResult(result, "readLine: empty string", "status=0 value=", actual,
            status == 0 && strcmp(buffer, "") == 0);
}

static void testReadLineColonEscaped(TestResult *result) {
  char buffer[64];
  int status = -1;
  char actual[128];

  simulateInput("A:B\n");
  status = readLine(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "status=%d value=%s", status, buffer);
  setResult(result, "readLine: colon escaped", "status=0 value=A%3AB", actual,
            status == 0 && strcmp(buffer, "A%3AB") == 0);
}

static void testSelectZoneMenuValid(TestResult *result) {
  int status = -1;
  char actual[64];

  simulateInput("2\n");
  status = selectZoneMenu();

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "selectZoneMenu: valid choice", "1", actual, status == 1);
}

static void testSelectZoneMenuInvalid(TestResult *result) {
  int status = -1;
  char actual[64];

  simulateInput("9\n");
  status = selectZoneMenu();

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "selectZoneMenu: invalid choice", "-1", actual,
            status == -1);
}

static void testSelectDonationSortMenuValid(TestResult *result) {
  int status = -1;
  char actual[64];

  simulateInput("3\n");
  status = selectDonationSortMenu();

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "selectDonationSortMenu: valid choice", "2", actual,
            status == 2);
}

static void printResults(TestResult results[], int count) {
  int passedCount = 0;

  printf("\n=== user_input tests ===\n\n");
  printf("===============================================================\n");
  printf("%-3s | %-46s | %-16s | %-16s\n", "#", "Test", "Expected", "Actual");
  printf("----------------------------------------------------------------"
         "---------------\n");

  for (int i = 0; i < count; i++) {
    const char *statusTag = results[i].passed ? "PASS" : "FAIL";

    printf("%-3d | %-46s | %-16s | %-16s\n", i + 1, results[i].name,
           results[i].expected, results[i].actual);
    printf("      Result: %s\n", statusTag);

    if (results[i].passed) {
      passedCount++;
    }
  }

  printf("===============================================================\n");
  printf("Summary: %d/%d tests passed\n", passedCount, count);
  printf("===============================================================\n\n");
}

int main(void) {
  TestResult results[6];
  int count = 0;
  int failedCount = 0;

  testReadLineNormal(&results[count++]);
  testReadLineEmpty(&results[count++]);
  testReadLineColonEscaped(&results[count++]);
  testSelectZoneMenuValid(&results[count++]);
  testSelectZoneMenuInvalid(&results[count++]);
  testSelectDonationSortMenuValid(&results[count++]);

  printResults(results, count);

  for (int i = 0; i < count; i++) {
    if (!results[i].passed) {
      failedCount++;
    }
  }

  remove(TEST_INPUT_FILE);

  return failedCount;
}
