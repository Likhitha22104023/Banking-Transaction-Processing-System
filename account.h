#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "Common.h"
ErrorCode createAccount(int id, const char *name, long long openingBalance);
Account *findAccount(int id);
void viewAccounts(void);
#endif
