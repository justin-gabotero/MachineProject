#include "../src/user_system.h"

#include <stdio.h>
#include <string.h>

#define MAX_TEST_USERS 10

typedef struct {
  const char *name;
  char expected[128];
  char actual[128];
  int passed;
} TestResult;

static void setResult(TestResult *result, const char *name, const char *expected, const char *actual, int passed) {
  if (result != NULL) {
    result->name = name;
    snprintf(result->expected, sizeof(result->expected), "%s", expected);
    snprintf(result->actual, sizeof(result->actual), "%s", actual);
    result->passed = passed;
  }
}

static User makeValidUser(const char *username, const char *password) {
  User u;
  memset(&u, 0, sizeof(u));
  strcpy(u.user, username);
  strcpy(u.password, password);
  return u;
}

static void testRegisterUserSuccess(TestResult *result) {
  User users[MAX_TEST_USERS];
  int count = 0;
  int status = 0;
  char actual[64];

  status = registerUser(users, &count, "newuser", "pass123");

  snprintf(actual, sizeof(actual), "status=%d count=%d", status, count);
  setResult(result, "registerUser: valid new user", "status=0 count=1", actual, status == 0 && count == 1);
}

static void testRegisterUserDuplicate(TestResult *result) {
  User users[MAX_TEST_USERS];
  int count = 1;
  int status = 0;
  char actual[64];

  users[0] = makeValidUser("existing", "pass");

  status = registerUser(users, &count, "existing", "pass");

  snprintf(actual, sizeof(actual), "status=%d count=%d", status, count);
  setResult(result, "registerUser: duplicate username", "status=-1 count=1", actual, status == -1 && count == 1);
}

static void testRegisterUserEmpty(TestResult *result) {
  User users[MAX_TEST_USERS];
  int count = 0;
  int status = 0;
  char actual[64];

  status = registerUser(users, &count, "", "");

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "registerUser: empty input", "-1", actual, status == -1);
}


static void testLoginUserSuccess(TestResult *result) {
  User users[2];
  int count = 1;
  User *found = NULL;
  char actual[64];

  users[0] = makeValidUser("user1", "pass");

  found = loginUser(users, count, "user1", "pass");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "loginUser: correct credentials", "found", actual, found != NULL);
}

static void testLoginUserWrongPassword(TestResult *result) {
  User users[2];
  int count = 1;
  User *found = NULL;
  char actual[64];

  users[0] = makeValidUser("user1", "pass");

  found = loginUser(users, count, "user1", "wrong");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "loginUser: wrong password", "NULL", actual, found == NULL);
}

static void testLoginUserNonexistent(TestResult *result) {
  User users[2];
  int count = 1;
  User *found = NULL;
  char actual[64];

  users[0] = makeValidUser("user1", "pass");

  found = loginUser(users, count, "ghost", "pass");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "loginUser: nonexistent user", "NULL", actual, found == NULL);
}

static void testGetUserByUsernameFound(TestResult *result) {
  User users[2];
  int count = 1;
  User *found = NULL;
  char actual[64];

  users[0] = makeValidUser("alpha", "123");

  found = getUserByUsername(users, count, "alpha");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "getUserByUsername: found", "found", actual, found != NULL);
}

static void testGetUserByUsernameNotFound(TestResult *result) {
  User users[2];
  int count = 1;
  User *found = NULL;
  char actual[64];

  users[0] = makeValidUser("alpha", "123");

  found = getUserByUsername(users, count, "beta");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "getUserByUsername: not found", "NULL", actual, found == NULL);
}

static void testGetUserByUsernameEmpty(TestResult *result) {
  User users[2];
  int count = 0;
  User *found = NULL;
  char actual[64];

  found = getUserByUsername(users, count, "any");

  snprintf(actual, sizeof(actual), "%s", found ? "found" : "NULL");
  setResult(result, "getUserByUsername: empty list", "NULL", actual, found == NULL);
}


static void printResults(TestResult results[], int count) {
  int passedCount = 0;
  printf("\n===============================================================\n");
  printf("User System Test Results\n");
  printf("===============================================================\n");
  printf("%-3s | %-46s | %-16s | %-16s\n", "#", "Test", "Expected", "Actual");
  printf("-------------------------------------------------------------------------------\n");

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
  TestResult results[9];
  int count = 0;
  int failedCount = 0;

  testRegisterUserSuccess(&results[count++]);
  testRegisterUserDuplicate(&results[count++]);
  testRegisterUserEmpty(&results[count++]);
  testLoginUserSuccess(&results[count++]);
  testLoginUserWrongPassword(&results[count++]);
  testLoginUserNonexistent(&results[count++]);
  testGetUserByUsernameFound(&results[count++]);
  testGetUserByUsernameNotFound(&results[count++]);
  testGetUserByUsernameEmpty(&results[count++]);
  printResults(results, count);

  for (int i = 0; i < count; i++) {
    if (results[i].passed == 0) {
      failedCount++;
    }
  }

  return failedCount;
}
