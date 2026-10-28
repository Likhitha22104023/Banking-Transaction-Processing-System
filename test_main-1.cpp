#include <gtest/gtest.h>

extern "C" {
#include "Common.h"
#include "account.h"
#include "transaction.h"
#include "security.h"
#include "logger.h"
#include "secalloc.h"
#include "report.h"
}

#include <cstdio>
#include <cstring>

class SecureAllocTest : public ::testing::Test {
protected:
    void SetUp() override {
        secureAllocInit();
    }

    void TearDown() override {
        secureAllocCleanup();
    }
};

// Authentication tests
TEST(AuthenticationTests, AcceptsAdminCredentials) {
    EXPECT_TRUE(authenticateUser("admin", "admin123"));
}

TEST(AuthenticationTests, AcceptsCustomerCredentials) {
    EXPECT_TRUE(authenticateUser("customer", "cust123"));
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

TEST(AuthorizationTests, CustomerCanWithdraw) {
    EXPECT_TRUE(authorizeOperation(ROLE_CUSTOMER, "withdraw"));
}

TEST(AuthorizationTests, CustomerCanTransfer) {
    EXPECT_TRUE(authorizeOperation(ROLE_CUSTOMER, "transfer"));
}

TEST(AuthorizationTests, CustomerCannotCreateAccount) {
    EXPECT_FALSE(authorizeOperation(ROLE_CUSTOMER, "create_account"));
}

TEST(AuthorizationTests, AdminCanCreateAccount) {
    EXPECT_TRUE(authorizeOperation(ROLE_ADMIN, "create_account"));
}

TEST(AuthorizationTests, RejectsNullOperation) {
    EXPECT_FALSE(authorizeOperation(ROLE_ADMIN, nullptr));
}

// Transaction validation tests
TEST(TransactionValidationTests, AcceptsValidDeposit) {
    Transaction tx = {};
    tx.type = TRANSACTION_DEPOSIT;
    tx.fromAccountId = 1;
    tx.amount = 100;
    EXPECT_EQ(validateTransaction(&tx), E_SUCCESS);
}

TEST(TransactionValidationTests, RejectsNullTransaction) {
    EXPECT_EQ(validateTransaction(nullptr), E001_INVALID_TRANSACTION);
}

TEST(TransactionValidationTests, RejectsZeroAmount) {
    Transaction tx = {};
    tx.type = TRANSACTION_DEPOSIT;
    tx.fromAccountId = 1;
    tx.amount = 0;
    EXPECT_EQ(validateTransaction(&tx), E001_INVALID_TRANSACTION);
}

TEST(TransactionValidationTests, RejectsNegativeAmount) {
    Transaction tx = {};
    tx.type = TRANSACTION_WITHDRAW;
    tx.fromAccountId = 1;
    tx.amount = -10;
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

TEST(TransactionValidationTests, RejectsUnknownTransactionType) {
    Transaction tx = {};
    tx.type = static_cast<TransactionType>(99);
    tx.fromAccountId = 1;
    tx.amount = 50;
    EXPECT_EQ(validateTransaction(&tx), E001_INVALID_TRANSACTION);
}

// Account and transaction operation tests use unique IDs because the account
// module stores accounts in a process-wide static array.
TEST(AccountTests, CreatesAndFindsAccount) {
    const int id = 91001;
    EXPECT_EQ(createAccount(id, "Test Account", 500), E_SUCCESS);
    Account *account = findAccount(id);
    ASSERT_NE(account, nullptr);
    EXPECT_STREQ(account->holderName, "Test Account");
    EXPECT_EQ(account->balance, 500);
}

TEST(AccountTests, RejectsInvalidAccountDetails) {
    EXPECT_EQ(createAccount(-1, "Invalid", 100), E001_INVALID_TRANSACTION);
    EXPECT_EQ(createAccount(91002, nullptr, 100), E001_INVALID_TRANSACTION);
    EXPECT_EQ(createAccount(91003, "", 100), E001_INVALID_TRANSACTION);
    EXPECT_EQ(createAccount(91004, "Negative Balance", -1), E001_INVALID_TRANSACTION);
}

TEST(AccountTests, RejectsDuplicateAccountId) {
    const int id = 91005;
    ASSERT_EQ(createAccount(id, "First Account", 100), E_SUCCESS);
    EXPECT_EQ(createAccount(id, "Duplicate Account", 200), E001_INVALID_TRANSACTION);
}

TEST(AccountTests, DepositUpdatesBalance) {
    const int id = 91006;
    ASSERT_EQ(createAccount(id, "Deposit Account", 100), E_SUCCESS);
    EXPECT_EQ(deposit(id, 50), E_SUCCESS);
    ASSERT_NE(findAccount(id), nullptr);
    EXPECT_EQ(findAccount(id)->balance, 150);
}

TEST(AccountTests, DepositRejectsNonPositiveAmount) {
    EXPECT_EQ(deposit(91006, 0), E001_INVALID_TRANSACTION);
    EXPECT_EQ(deposit(91006, -5), E001_INVALID_TRANSACTION);
}

TEST(AccountTests, DepositRejectsMissingAccount) {
    EXPECT_EQ(deposit(91999, 10), E003_ACCOUNT_NOT_FOUND);
}

TEST(AccountTests, WithdrawalUpdatesBalance) {
    const int id = 91007;
    ASSERT_EQ(createAccount(id, "Withdrawal Account", 200), E_SUCCESS);
    EXPECT_EQ(withdraw(id, 75), E_SUCCESS);
    EXPECT_EQ(findAccount(id)->balance, 125);
}

TEST(AccountTests, WithdrawalRejectsInsufficientBalance) {
    const int id = 91008;
    ASSERT_EQ(createAccount(id, "Low Balance Account", 20), E_SUCCESS);
    EXPECT_EQ(withdraw(id, 50), E002_INSUFFICIENT_BALANCE);
    EXPECT_EQ(findAccount(id)->balance, 20);
}

TEST(AccountTests, TransferMovesFundsBetweenAccounts) {
    const int fromId = 91009;
    const int toId = 91010;
    ASSERT_EQ(createAccount(fromId, "Transfer Source", 300), E_SUCCESS);
    ASSERT_EQ(createAccount(toId, "Transfer Destination", 50), E_SUCCESS);
    EXPECT_EQ(transfer(fromId, toId, 100), E_SUCCESS);
    EXPECT_EQ(findAccount(fromId)->balance, 200);
    EXPECT_EQ(findAccount(toId)->balance, 150);
}

TEST(AccountTests, TransferRejectsInsufficientBalanceWithoutChangingBalances) {
    const int fromId = 91011;
    const int toId = 91012;
    ASSERT_EQ(createAccount(fromId, "Transfer Low Source", 10), E_SUCCESS);
    ASSERT_EQ(createAccount(toId, "Transfer Low Destination", 40), E_SUCCESS);
    EXPECT_EQ(transfer(fromId, toId, 25), E002_INSUFFICIENT_BALANCE);
    EXPECT_EQ(findAccount(fromId)->balance, 10);
    EXPECT_EQ(findAccount(toId)->balance, 40);
}

TEST(AccountTests, TransferRejectsSameAccount) {
    const int id = 91013;
    ASSERT_EQ(createAccount(id, "Same Account", 100), E_SUCCESS);
    EXPECT_EQ(transfer(id, id, 10), E001_INVALID_TRANSACTION);
}

TEST(AccountTests, ProcessTransactionDepositsFunds) {
    const int id = 91014;
    ASSERT_EQ(createAccount(id, "Process Transaction Account", 100), E_SUCCESS);
    Transaction tx = {};
    tx.transactionId = 1;
    tx.type = TRANSACTION_DEPOSIT;
    tx.fromAccountId = id;
    tx.amount = 25;
    processTransaction(&tx);
    EXPECT_EQ(tx.status, E_SUCCESS);
    EXPECT_EQ(findAccount(id)->balance, 125);
}

// Logger tests
TEST(LoggerTests, ReturnsCorrectErrorLabels) {
    EXPECT_STREQ(getErrorCodeLabel(E_SUCCESS), "SUCCESS");
    EXPECT_STREQ(getErrorCodeLabel(E001_INVALID_TRANSACTION), "E001");
    EXPECT_STREQ(getErrorCodeLabel(E012_ACCESS_DENIED), "E012");
}

TEST(LoggerTests, ReturnsUnknownForUnrecognizedError) {
    EXPECT_STREQ(getErrorCodeLabel(static_cast<ErrorCode>(99)), "UNKNOWN");
}

TEST(LoggerTests, LogFunctionsAcceptNullSafely) {
    EXPECT_NO_THROW(logTransaction(nullptr));
    EXPECT_NO_THROW(logError(E001_INVALID_TRANSACTION, nullptr));
}

// Secure allocator tests
TEST_F(SecureAllocTest, AllocatesAndFreesMemory) {
    void *memory = secureAlloc(32, "unit-test");
    ASSERT_NE(memory, nullptr);
    std::memset(memory, 0x5A, 32);
    auto *header = static_cast<BlockHeader *>(memory) - 1;
    EXPECT_TRUE(validateBlock(header));
    EXPECT_EQ(secureFree(memory), E_SUCCESS);
}

TEST_F(SecureAllocTest, RejectsZeroSizeAllocation) {
    EXPECT_EQ(secureAlloc(0, "unit-test"), nullptr);
}

TEST_F(SecureAllocTest, AcceptsNullFree) {
    EXPECT_EQ(secureFree(nullptr), E_SUCCESS);
}

TEST_F(SecureAllocTest, DetectsInvalidFree) {
    int value = 10;
    EXPECT_EQ(secureFree(&value), E006_INVALID_FREE);
}

TEST_F(SecureAllocTest, DetectsDoubleFree) {
    void *memory = secureAlloc(16, "unit-test");
    ASSERT_NE(memory, nullptr);
    EXPECT_EQ(secureFree(memory), E_SUCCESS);
    EXPECT_EQ(secureFree(memory), E007_DOUBLE_FREE);
}

TEST_F(SecureAllocTest, RejectsInvalidBlockHeader) {
    EXPECT_FALSE(validateBlock(nullptr));
}

TEST_F(SecureAllocTest, GeneratesMemoryReportWithoutCrashing) {
    void *memory = secureAlloc(8, "report-test");
    ASSERT_NE(memory, nullptr);
    EXPECT_NO_THROW(generateMemoryReport());
    EXPECT_NO_THROW(detectMemoryLeaks());
}
