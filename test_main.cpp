
#include <gtest/gtest.h>

extern "C" {
#include "../Common.h"
#include "../security.h"
#include "../transaction.h"
#include "../logger.h"
}

// Authentication tests
TEST(AuthenticationTests, AcceptsAdminCredentials) {
    EXPECT_TRUE(authenticateUser("admin", "admin123"));
}

TEST(AuthenticationTests, RejectsIncorrectPassword) {
    EXPECT_FALSE(authenticateUser("admin", "wrong"));
}

TEST(AuthenticationTests, RejectsNullCredentials) {
    EXPECT_FALSE(authenticateUser(nullptr, "password"));
    EXPECT_FALSE(authenticateUser("admin", nullptr));
}

// Authorization tests
TEST(AuthorizationTests, CustomerCanDeposit) {
    EXPECT_TRUE(authorizeOperation(ROLE_CUSTOMER, "deposit"));
}

TEST(AuthorizationTests, CustomerCannotCreateAccount) {
    EXPECT_FALSE(authorizeOperation(ROLE_CUSTOMER, "create_account"));
}

TEST(AuthorizationTests, AdminCanCreateAccount) {
    EXPECT_TRUE(authorizeOperation(ROLE_ADMIN, "create_account"));
}

// Transaction validation tests
TEST(TransactionValidationTests, AcceptsValidDeposit) {
    Transaction tx = {};
    tx.type = TRANSACTION_DEPOSIT;
    tx.fromAccountId = 1;
    tx.amount = 100;

    EXPECT_EQ(validateTransaction(&tx), E_SUCCESS);
}

TEST(TransactionValidationTests, RejectsZeroAmount) {
    Transaction tx = {};
    tx.type = TRANSACTION_DEPOSIT;
    tx.fromAccountId = 1;
    tx.amount = 0;

    EXPECT_EQ(validateTransaction(&tx), E001_INVALID_TRANSACTION);
}

TEST(TransactionValidationTests, RejectsTransferToSameAccount) {
    Transaction tx = {};
    tx.type = TRANSACTION_TRANSFER;
    tx.fromAccountId = 1;
    tx.toAccountId = 1;
    tx.amount = 50;

    EXPECT_EQ(validateTransaction(&tx), E001_INVALID_TRANSACTION);
}

TEST(TransactionValidationTests, RejectsNullTransaction) {
    EXPECT_EQ(validateTransaction(nullptr), E001_INVALID_TRANSACTION);
}

// Logger tests
TEST(LoggerTests, ReturnsSuccessLabel) {
    EXPECT_STREQ(getErrorCodeLabel(E_SUCCESS), "SUCCESS");
}
