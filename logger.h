#ifndef LOGGER_H
#define LOGGER_H
#include "Common.h"
void logTransaction(const Transaction *tx);
void logError(ErrorCode error, const char *message);
const char *getErrorCodeLabel(ErrorCode error);
#endif
