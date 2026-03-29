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


static void testReadIntValid(TestResult *result) {
  int value = 0;
  char actual[64];

  simulateInput("42\n");
  value = readIntInput();

  snprintf(actual, sizeof(actual), "%d", value);
  setResult(result, "readIntInput: valid integer", "42", actual,
            value == 42);
}

static void testReadIntNegative(TestResult *result) {
  int value = 0;
  char actual[64];

  simulateInput("-7\n");
  value = readIntInput();

  snprintf(actual, sizeof(actual), "%d", value);
  setResult(result, "readIntInput: negative integer", "-7", actual,
            value == -7);
}

static void testReadIntInvalid(TestResult *result) {
  int value = 0;
  char actual[64];

  simulateInput("abc\n10\n");  // invalid then valid
  value = readIntInput();

  snprintf(actual, sizeof(actual), "%d", value);
  setResult(result, "readIntInput: invalid then valid", "10", actual,
            value == 10);
}


static void testReadStringNormal(TestResult *result) {
  char buffer[64];
  char actual[64];

  simulateInput("Hello World\n");
  readStringInput(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "%s", buffer);
  setResult(result, "readStringInput: normal string",
            "Hello World", actual,
            strcmp(buffer, "Hello World") == 0);
}

static void testReadStringEmpty(TestResult *result) {
  char buffer[64];
  char actual[64];

  simulateInput("\n");
  readStringInput(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "%s", buffer);
  setResult(result, "readStringInput: empty string",
            "", actual,
            strcmp(buffer, "") == 0);
}

static void testReadStringLong(TestResult *result) {
  char buffer[10]; 
  char actual[64];

  simulateInput("ThisIsAVeryLongInputString\n");
  readStringInput(buffer, sizeof(buffer));

  snprintf(actual, sizeof(actual), "%s", buffer);
  setResult(result, "readStringInput: long input truncated",
            "ThisIsAV", actual,  
            strncmp(buffer, "ThisIsAV", 8) == 0);
}


static void printResults(TestResult results[], int count) {
  int passedCount = 0;

  printf("\n===============================================================\n");
  printf("User Input Test Results\n");
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

  testReadIntValid(&results[count++]);
  testReadIntNegative(&results[count++]);
  testReadIntInvalid(&results[count++]);
  testReadStringNormal(&results[count++]);
  testReadStringEmpty(&results[count++]);
  testReadStringLong(&results[count++]);

  printResults(results, count);

  for (int i = 0; i < count; i++) {
    if (!results[i].passed) {
      failedCount++;
    }
  }

  return failedCount;
}
