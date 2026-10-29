#ifndef SECURITY_H
#define SECURITY_H

#include "Common.h"

typedef enum { ROLE_CUSTOMER = 1, ROLE_ADMIN } UserRole;

bool authenticateUser(const char *username, const char *password);
bool authorizeOperation(UserRole role, const char *operation);
bool registerUser(const char *username, const char *password);

#endif
