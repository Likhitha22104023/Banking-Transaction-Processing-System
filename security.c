#include "security.h"

/* Demo credentials only. */
bool authenticateUser(const char *username, const char *password)
{
    FILE *file;
    char savedUsername[NAME_LENGTH];
    char savedPassword[NAME_LENGTH];

    if (username == NULL || password == NULL)
        return false;

    /* Existing admin login */
    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "admin123") == 0)
        return true;

    /* Existing customer login */
    if (strcmp(username, "customer") == 0 &&
        strcmp(password, "cust123") == 0)
        return true;

    /* Check registered users */
    file = fopen("users.txt", "r");

    if (file == NULL)
        return false;

    while (fscanf(file, "%49s %49s",
                  savedUsername, savedPassword) == 2)
    {
        if (strcmp(username, savedUsername) == 0 &&
            strcmp(password, savedPassword) == 0)
        {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

bool registerUser(const char *username, const char *password)
{
    FILE *file;
    char savedUsername[NAME_LENGTH];
    char savedPassword[NAME_LENGTH];

    if (username == NULL || password == NULL ||
        username[0] == '\0' || password[0] == '\0')
        return false;

    /* Prevent duplicate built-in usernames */
    if (strcmp(username, "admin") == 0 ||
        strcmp(username, "customer") == 0)
        return false;

    /* Check whether username already exists */
    file = fopen("users.txt", "r");

    if (file != NULL)
    {
        while (fscanf(file, "%49s %49s",
                      savedUsername, savedPassword) == 2)
        {
            if (strcmp(username, savedUsername) == 0)
            {
                fclose(file);
                return false;
            }
        }

        fclose(file);
    }

    /* Save new credentials */
    file = fopen("users.txt", "a");

    if (file == NULL)
        return false;

    if (fprintf(file, "%s %s\n", username, password) < 0)
    {
        fclose(file);
        return false;
    }

    if (fclose(file) != 0)
        return false;

    return true;
}

bool authorizeOperation(UserRole role, const char *operation)
{
    if (operation == NULL)
        return false;

    if (role == ROLE_ADMIN)
        return true;

    if (role == ROLE_CUSTOMER)
    {
        if (strcmp(operation, "view_balance") == 0 ||
            strcmp(operation, "deposit") == 0 ||
            strcmp(operation, "withdraw") == 0 ||
            strcmp(operation, "transfer") == 0)
            return true;
    }

    return false;
}
