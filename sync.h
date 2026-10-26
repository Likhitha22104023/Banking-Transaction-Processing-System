#ifndef SYNC_H
#define SYNC_H
#include "Common.h"
ErrorCode lockAccount(Account *acc);
void unlockAccount(Account *acc);
ErrorCode lockAccountsOrdered(Account *a, Account *b);
void unlockAccountsOrdered(Account *a, Account *b);
#endif
