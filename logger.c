#include "logger.h"
#include <time.h>
const char *getErrorCodeLabel(ErrorCode error)
{
    switch (error) {
        case E_SUCCESS: return "SUCCESS";
        case E001_INVALID_TRANSACTION: return "E001";
        case E002_INSUFFICIENT_BALANCE: return "E002";
        case E003_ACCOUNT_NOT_FOUND: return "E003";
        case E004_MEMORY_ALLOCATION_FAILURE: return "E004";
        case E005_LOCK_ACQUISITION_FAILURE: return "E005";
        case E006_INVALID_FREE: return "E006";
        case E007_DOUBLE_FREE: return "E007";
        case E008_MEMORY_CORRUPTION: return "E008";
        case E009_MEMORY_LEAK: return "E009";
        case E010_MEMORY_THRESHOLD_EXCEEDED: return "E010";
        case E011_AUTHENTICATION_FAILURE: return "E011";
        case E012_ACCESS_DENIED: return "E012";
        default: return "UNKNOWN";
    }
}
void logTransaction(const Transaction *tx)
{
    FILE *file;
    time_t now;
    if (tx == NULL) return;
    file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        logError(E004_MEMORY_ALLOCATION_FAILURE, "Unable to open transaction log");
        return;
    }
    now = time(NULL);
    fprintf(file, "Time: %lld | ID: %d | Type: %d | From: %d | To: %d | Amount: %lld | Status: %s\\n",
            (long long)now, tx->transactionId, (int)tx->type,
            tx->fromAccountId, tx->toAccountId, tx->amount,
            getErrorCodeLabel(tx->status));
    fclose(file);
}
void logError(ErrorCode error, const char *message)
{
    FILE *file = fopen(SYSTEM_LOG_FILE, "a");
    time_t now;
    if (file == NULL) {
        printf("Warning: Unable to write system log.\\n");
        return;
    }
    now = time(NULL);
    fprintf(file, "Time: %lld | Error: %s | Message: %s\\n",
            (long long)now, getErrorCodeLabel(error),
            message != NULL ? message : "No details");
    fclose(file);
}
