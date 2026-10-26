#include "security.h"
/* Demo credentials only. Do not use these credentials in a real banking system. */
bool authenticateUser(const char *username, const char *password)
{
    if (username == NULL || password == NULL) return false;
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) return true;
    if (strcmp(username, "customer") == 0 && strcmp(password, "cust123") == 0) return true;
    return false;
}
bool authorizeOperation(UserRole role, const char *operation)
{
    if (operation == NULL) return false;
    if (role == ROLE_ADMIN) return true;
    if (role == ROLE_CUSTOMER) {
        if (strcmp(operation, "view_balance") == 0 ||
            strcmp(operation, "deposit") == 0 ||
            strcmp(operation, "withdraw") == 0 ||
            strcmp(operation, "transfer") == 0) return true;
    }
    return false;
}
