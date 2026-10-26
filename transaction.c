#include "transaction.h"
#include "account.h"
#include "sync.h"
#include <limits.h>

ErrorCode validateTransaction(const Transaction *tx)
{
    if (tx == NULL || tx->amount <= 0) return E001_INVALID_TRANSACTION;
    switch (tx->type) {
    case TRANSACTION_DEPOSIT:
    case TRANSACTION_WITHDRAW:
        if (tx->fromAccountId <= 0) return E001_INVALID_TRANSACTION;
        break;
    case TRANSACTION_TRANSFER:
        if (tx->fromAccountId <= 0 || tx->toAccountId <= 0 ||
            tx->fromAccountId == tx->toAccountId)
            return E001_INVALID_TRANSACTION;
        break;
    default: return E001_INVALID_TRANSACTION;
    }
    return E_SUCCESS;
}
ErrorCode deposit(int accountId, long long amount)
{
    Account *acc;
    ErrorCode result;
    if (amount <= 0) return E001_INVALID_TRANSACTION;
    acc = findAccount(accountId);
    if (acc == NULL) return E003_ACCOUNT_NOT_FOUND;
    result = lockAccount(acc);
    if (result != E_SUCCESS) return result;
    if (acc->balance > LLONG_MAX - amount) {
        unlockAccount(acc); return E001_INVALID_TRANSACTION;
    }
    acc->balance += amount;
    printf("Deposit successful. New balance: %lld\\n", acc->balance);
    unlockAccount(acc);
    return E_SUCCESS;
}
ErrorCode withdraw(int accountId, long long amount)
{
    Account *acc;
    ErrorCode result;
    if (amount <= 0) return E001_INVALID_TRANSACTION;
    acc = findAccount(accountId);
    if (acc == NULL) return E003_ACCOUNT_NOT_FOUND;
    result = lockAccount(acc);
    if (result != E_SUCCESS) return result;
    if (acc->balance < amount) {
        unlockAccount(acc); return E002_INSUFFICIENT_BALANCE;
    }
    acc->balance -= amount;
    printf("Withdrawal successful. New balance: %lld\\n", acc->balance);
    unlockAccount(acc);
    return E_SUCCESS;
}
ErrorCode transfer(int fromId, int toId, long long amount)
{
    Account *from, *to;
    ErrorCode result;
    if (amount <= 0 || fromId == toId) return E001_INVALID_TRANSACTION;
    from = findAccount(fromId); to = findAccount(toId);
    if (from == NULL || to == NULL) return E003_ACCOUNT_NOT_FOUND;
    result = lockAccountsOrdered(from, to);
    if (result != E_SUCCESS) return result;
    if (from->balance < amount) {
        unlockAccountsOrdered(from, to); return E002_INSUFFICIENT_BALANCE;
    }
    if (to->balance > LLONG_MAX - amount) {
        unlockAccountsOrdered(from, to); return E001_INVALID_TRANSACTION;
    }
    from->balance -= amount; to->balance += amount;
    printf("Transfer successful.\\n");
    printf("Source account balance: %lld\\n", from->balance);
    printf("Destination account balance: %lld\\n", to->balance);
    unlockAccountsOrdered(from, to);
    return E_SUCCESS;
}
void *processTransaction(void *arg)
{
    Transaction *tx = (Transaction *)arg;
    if (tx == NULL) return NULL;
    tx->status = validateTransaction(tx);
    if (tx->status != E_SUCCESS) return NULL;
    switch (tx->type) {
    case TRANSACTION_DEPOSIT: tx->status = deposit(tx->fromAccountId, tx->amount); break;
    case TRANSACTION_WITHDRAW: tx->status = withdraw(tx->fromAccountId, tx->amount); break;
    case TRANSACTION_TRANSFER: tx->status = transfer(tx->fromAccountId, tx->toAccountId, tx->amount); break;
    default: tx->status = E001_INVALID_TRANSACTION; break;
    }
    return NULL;
}
