#ifndef TRANSACTION_H
#define TRANSACTION_H
#include "Common.h"
ErrorCode validateTransaction(const Transaction *tx);
ErrorCode deposit(int accountId, long long amount);
ErrorCode withdraw(int accountId, long long amount);
ErrorCode transfer(int fromId, int toId, long long amount);
void *processTransaction(void *arg);
#endif
