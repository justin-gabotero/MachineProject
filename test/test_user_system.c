#include "../src/user_system.h"

#include <stdio.h>
#include <string.h>

#define TEST_USER_FILE "user.txt"
#define TEST_USER_BACKUP "user.txt.bak_test"

typedef struct {
  const char *name;
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

static void setResult(TestResult *result, const char *name,
                      const char *expected, const char *actual, int passed) {
  if (result != NULL) {
    result->name = name;
    snprintf(result->expected, sizeof(result->expected), "%s", expected);
    snprintf(result->actual, sizeof(result->actual), "%s", actual);
    result->passed = passed;
  }
}

static User makeValidUser(const char *username, const char *password,
                          Role role) {
  User user;

  memset(&user, 0, sizeof(user));
  strcpy(user.user, username);
  strcpy(user.password, password);
  user.creationDate.year = 2026;
  user.creationDate.month = 3;
  user.creationDate.day = 30;
  user.role = role;

  return user;
}

static User *loginAs(const char *username, const char *password) {
  String userBuf;
  StringLong passBuf;
  User *logged = NULL;

  strcpy(userBuf, username);
  strcpy(passBuf, password);
  logged = loginUser(userBuf, passBuf);

  return logged;
}

static int resetPasswordAs(User *user, const char *newPassword) {
  StringLong passBuf;
  int status = -1;

  strcpy(passBuf, newPassword);
  status = resetUserPassword(user, passBuf);

  return status;
}

static void resetTestUserFile(void) { remove(TEST_USER_FILE); }

static void testRegisterUserSuccess(TestResult *result) {
  User user = makeValidUser("newuser", "pass123", SUPPLIER);
  User *logged = NULL;
  int status = -1;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  status = registerUser(&user);
  logged = loginAs("newuser", "pass123");
  passed = status == 0 && logged != NULL;

  snprintf(actual, sizeof(actual), "status=%d login=%s", status,
           logged != NULL ? "found" : "NULL");
  setResult(result, "registerUser: valid new user", "status=0 login=found",
            actual, passed);
}

static void testRegisterUserDuplicate(TestResult *result) {
  User user = makeValidUser("existing", "pass", RECEIVER);
  int status = -1;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  registerUser(&user);
  status = registerUser(&user);
  passed = status == -1;

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "registerUser: duplicate username", "-1", actual, passed);
}

static void testRegisterUserNullInput(TestResult *result) {
  int status = -1;
  int passed = 0;
  char actual[64];

  status = registerUser(NULL);
  passed = status == -1;

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "registerUser: null input", "-1", actual, passed);
}

static void testLoginUserSuccess(TestResult *result) {
  User user = makeValidUser("login_ok", "secret", SUPPLIER);
  User *found = NULL;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  registerUser(&user);
  found = loginAs("login_ok", "secret");
  passed = found != NULL;

  snprintf(actual, sizeof(actual), "%s", found != NULL ? "found" : "NULL");
  setResult(result, "loginUser: correct credentials", "found", actual, passed);
}

static void testLoginUserWrongPassword(TestResult *result) {
  User user = makeValidUser("login_wrong", "secret", RECEIVER);
  User *found = NULL;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  registerUser(&user);
  found = loginAs("login_wrong", "badpass");
  passed = found == NULL;

  snprintf(actual, sizeof(actual), "%s", found != NULL ? "found" : "NULL");
  setResult(result, "loginUser: wrong password", "NULL", actual, passed);
}

static void testLoginUserNonexistent(TestResult *result) {
  User *found = NULL;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  found = loginAs("ghost", "pass");
  passed = found == NULL;

  snprintf(actual, sizeof(actual), "%s", found != NULL ? "found" : "NULL");
  setResult(result, "loginUser: nonexistent user", "NULL", actual, passed);
}

static void testResetPasswordSuccess(TestResult *result) {
  User user = makeValidUser("resetme", "oldpass", SUPPLIER);
  User target;
  User *logged = NULL;
  int status = -1;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  registerUser(&user);

  memset(&target, 0, sizeof(target));
  strcpy(target.user, "resetme");

  status = resetPasswordAs(&target, "newpass");
  logged = loginAs("resetme", "newpass");
  passed = status == 0 && logged != NULL;

  snprintf(actual, sizeof(actual), "status=%d login=%s", status,
           logged != NULL ? "found" : "NULL");
  setResult(result, "resetUserPassword: valid update", "status=0 login=found",
            actual, passed);
}

static void testEditUserEmptyInput(TestResult *result) {
  User current = makeValidUser("edituser", "oldpass", RECEIVER);
  User in = makeValidUser("", "", RECEIVER);
  int status = -1;
  int passed = 0;
  char actual[64];

  status = editUser(&current, &in);
  passed = status == -1;

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "editUser: empty input", "-1", actual, passed);
}

static void testEditUserDuplicateUsername(TestResult *result) {
  User existing = makeValidUser("target_user", "pass1", SUPPLIER);
  User current = makeValidUser("current_user", "pass2", RECEIVER);
  User in = makeValidUser("target_user", "newpass", RECEIVER);
  int status = -1;
  int passed = 0;
  char actual[64];

  resetTestUserFile();
  registerUser(&existing);
  registerUser(&current);

  status = editUser(&current, &in);
  passed = status == -2;

  snprintf(actual, sizeof(actual), "%d", status);
  setResult(result, "editUser: duplicate username", "-2", actual, passed);
}

static void printResults(TestResult results[], int count) {
  int passedCount = 0;

  printf("\n===============================================================\n");
  printf("User System Test Results\n");
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
  TestResult results[9];
  int count = 0;
  int failedCount = 0;
  int hadOriginal = 0;
  int movedOriginal = 0;
  int restored = 0;

  hadOriginal = fileExists(TEST_USER_FILE);
  if (hadOriginal == 1) {
    if (rename(TEST_USER_FILE, TEST_USER_BACKUP) == 0) {
      movedOriginal = 1;
    }
  }

  testRegisterUserSuccess(&results[count++]);
  testRegisterUserDuplicate(&results[count++]);
  testRegisterUserNullInput(&results[count++]);
  testLoginUserSuccess(&results[count++]);
  testLoginUserWrongPassword(&results[count++]);
  testLoginUserNonexistent(&results[count++]);
  testResetPasswordSuccess(&results[count++]);
  testEditUserEmptyInput(&results[count++]);
  testEditUserDuplicateUsername(&results[count++]);

  printResults(results, count);

  for (int i = 0; i < count; i++) {
    if (results[i].passed == 0) {
      failedCount++;
    }
  }

  remove(TEST_USER_FILE);

  if (movedOriginal == 1) {
    if (rename(TEST_USER_BACKUP, TEST_USER_FILE) == 0) {
      restored = 1;
    }
  } else {
    restored = 1;
  }

  if (restored == 0) {
    printf("Warning: could not restore original user.txt\n");
    failedCount++;
  }

  return failedCount;
}
