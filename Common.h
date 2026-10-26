#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

#define MAX_ACCOUNTS 100
#define NAME_LENGTH 50
#define LOG_FILE "transactions.log"
#define ACCOUNT_FILE "accounts.dat"
#define SYSTEM_LOG_FILE "system.log"

typedef enum {
    TRANSACTION_DEPOSIT = 1,
    TRANSACTION_WITHDRAW,
    TRANSACTION_TRANSFER
} TransactionType;

typedef enum {
    E_SUCCESS = 0,
    E001_INVALID_TRANSACTION,
    E002_INSUFFICIENT_BALANCE,
    E003_ACCOUNT_NOT_FOUND,
    E004_MEMORY_ALLOCATION_FAILURE,
    E005_LOCK_ACQUISITION_FAILURE,
    E006_INVALID_FREE,
    E007_DOUBLE_FREE,
    E008_MEMORY_CORRUPTION,
    E009_MEMORY_LEAK,
    E010_MEMORY_THRESHOLD_EXCEEDED,
    E011_AUTHENTICATION_FAILURE,
    E012_ACCESS_DENIED
} ErrorCode;

typedef struct {
    int accountId;
    char holderName[NAME_LENGTH];
    long long balance;
    bool active;
    pthread_mutex_t lock;
} Account;

typedef struct {
    int transactionId;
    TransactionType type;
    int fromAccountId;
    int toAccountId;
    long long amount;
    ErrorCode status;
} Transaction;

#endif
