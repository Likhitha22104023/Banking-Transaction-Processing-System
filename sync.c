#include "sync.h"
ErrorCode lockAccount(Account *acc)
{
    if (acc == NULL) return E003_ACCOUNT_NOT_FOUND;
    if (pthread_mutex_lock(&acc->lock) != 0)
        return E005_LOCK_ACQUISITION_FAILURE;
    return E_SUCCESS;
}
void unlockAccount(Account *acc)
{
    if (acc != NULL) pthread_mutex_unlock(&acc->lock);
}
ErrorCode lockAccountsOrdered(Account *a, Account *b)
{
    Account *first, *second;
    if (a == NULL || b == NULL || a == b)
        return E001_INVALID_TRANSACTION;
    if (a->accountId < b->accountId) { first = a; second = b; }
    else { first = b; second = a; }
    if (pthread_mutex_lock(&first->lock) != 0)
        return E005_LOCK_ACQUISITION_FAILURE;
    if (pthread_mutex_lock(&second->lock) != 0) {
        pthread_mutex_unlock(&first->lock);
        return E005_LOCK_ACQUISITION_FAILURE;
    }
    return E_SUCCESS;
}
void unlockAccountsOrdered(Account *a, Account *b)
{
    if (a == NULL || b == NULL || a == b) return;
    if (a->accountId < b->accountId) {
        pthread_mutex_unlock(&b->lock);
        pthread_mutex_unlock(&a->lock);
    } else {
        pthread_mutex_unlock(&a->lock);
        pthread_mutex_unlock(&b->lock);
    }
}
