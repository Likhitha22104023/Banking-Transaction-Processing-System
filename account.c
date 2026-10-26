#include "account.h"
static Account accountList[MAX_ACCOUNTS];
static int accountCount = 0;

ErrorCode createAccount(int id, const char *name, long long openingBalance)
{
    if (id <= 0 || name == NULL || name[0] == '\0' ||
        openingBalance < 0) return E001_INVALID_TRANSACTION;
    if (accountCount >= MAX_ACCOUNTS) return E004_MEMORY_ALLOCATION_FAILURE;
    if (findAccount(id) != NULL) return E001_INVALID_TRANSACTION;

    Account *acc = &accountList[accountCount];
    acc->accountId = id;
    snprintf(acc->holderName, sizeof(acc->holderName), "%s", name);
    acc->balance = openingBalance;
    acc->active = true;
    if (pthread_mutex_init(&acc->lock, NULL) != 0)
        return E005_LOCK_ACQUISITION_FAILURE;
    accountCount++;
    printf("Account created successfully.\\n");
    printf("Account ID: %d\\n", acc->accountId);
    printf("Account Holder: %s\\n", acc->holderName);
    printf("Opening Balance: %lld\\n", acc->balance);
    return E_SUCCESS;
}
Account *findAccount(int id)
{
    for (int i = 0; i < accountCount; i++)
        if (accountList[i].accountId == id && accountList[i].active)
            return &accountList[i];
    return NULL;
}
void viewAccounts(void)
{
    int found = 0;
    printf("\\n--- Account List ---\\n");
    for (int i = 0; i < accountCount; i++) {
        if (accountList[i].active) {
            printf("ID: %d | Name: %s | Balance: %lld\\n",
                   accountList[i].accountId,
                   accountList[i].holderName,
                   accountList[i].balance);
            found = 1;
        }
    }
    if (!found) printf("No accounts available.\\n");
}
