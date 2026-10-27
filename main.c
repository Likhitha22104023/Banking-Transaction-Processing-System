#include "Common.h"
#include "account.h"
#include "transaction.h"
#include "security.h"
#include "logger.h"
#include "report.h"
#include "secalloc.h"

static int nextTransactionId = 1;

static void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

static void showResult(ErrorCode result)
{
    if (result == E_SUCCESS)
    {
        printf("Operation completed successfully.\n");
    }
    else
    {
        printf("Operation failed. Error: %s\n",
               getErrorCodeLabel(result));
        logError(result, "Transaction operation failed");
    }
}

static void executeTransaction(TransactionType type)
{
    Transaction tx = {0};
    int accountId = 0;

    tx.transactionId = nextTransactionId++;
    tx.type = type;

    if (type == TRANSACTION_TRANSFER)
    {
        printf("Enter source account ID: ");

        if (scanf("%d", &tx.fromAccountId) != 1)
        {
            clearInputBuffer();
            printf("Invalid account ID.\n");
            return;
        }

        printf("Enter destination account ID: ");

        if (scanf("%d", &tx.toAccountId) != 1)
        {
            clearInputBuffer();
            printf("Invalid account ID.\n");
            return;
        }

        clearInputBuffer();

        if (tx.fromAccountId <= 0 || tx.toAccountId <= 0)
        {
            printf("Invalid account ID.\n");
            return;
        }

        if (tx.fromAccountId == tx.toAccountId)
        {
            printf("Source and destination account cannot be the same.\n");
            return;
        }
    }
    else
    {
        printf("Enter account ID: ");

        if (scanf("%d", &accountId) != 1)
        {
            clearInputBuffer();
            printf("Invalid account ID.\n");
            return;
        }

        clearInputBuffer();

        if (accountId <= 0)
        {
            printf("Invalid account ID.\n");
            return;
        }

        tx.fromAccountId = accountId;
    }

    printf("Enter amount: ");

    if (scanf("%lld", &tx.amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    clearInputBuffer();

    if (tx.amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    tx.status = validateTransaction(&tx);

    if (tx.status == E_SUCCESS)
    {
        switch (tx.type)
        {
            case TRANSACTION_DEPOSIT:
                tx.status = deposit(tx.fromAccountId, tx.amount);
                break;

            case TRANSACTION_WITHDRAW:
                tx.status = withdraw(tx.fromAccountId, tx.amount);
                break;

            case TRANSACTION_TRANSFER:
                tx.status = transfer(
                    tx.fromAccountId,
                    tx.toAccountId,
                    tx.amount
                );
                break;

            default:
                tx.status = E001_INVALID_TRANSACTION;
                break;
        }
    }

    logTransaction(&tx);
    showResult(tx.status);
}

int main(void)
{
    char username[NAME_LENGTH];
    char password[NAME_LENGTH];
    char name[NAME_LENGTH];

    UserRole role;
    int choice = 0;
    int accountId;
    long long openingBalance;

    secureAllocInit();

    printf("========================================\n");
    printf(" Banking Transaction Processing System\n");
    printf("             with SecureAlloc\n");
    printf("========================================\n");

    printf("Username: ");

    if (scanf("%49s", username) != 1)
    {
        secureAllocCleanup();
        return 1;
    }

    printf("Password: ");

    if (scanf("%49s", password) != 1)
    {
        secureAllocCleanup();
        return 1;
    }

    clearInputBuffer();

    if (!authenticateUser(username, password))
    {
        printf("Authentication failed.\n");
        logError(E011_AUTHENTICATION_FAILURE,
                 "Invalid login credentials");
        secureAllocCleanup();
        return 1;
    }

    role = (strcmp(username, "admin") == 0)
               ? ROLE_ADMIN
               : ROLE_CUSTOMER;

    printf("\nLogin successful.\n");

    do
    {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Create Account (Admin)\n");
        printf("2. View All Accounts (Admin)\n");
        printf("3. Deposit\n");
        printf("4. Withdraw\n");
        printf("5. Transfer\n");
        printf("6. Transaction Report (Admin)\n");
        printf("7. Memory Usage Report (Admin)\n");
        printf("8. Memory Leak Report (Admin)\n");
        printf("9. Logout and Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            clearInputBuffer();
            printf("Invalid menu choice.\n");
            continue;
        }

        clearInputBuffer();

        switch (choice)
        {
            case 1:
                if (!authorizeOperation(role, "create_account"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                printf("Enter account ID: ");

                if (scanf("%d", &accountId) != 1)
                {
                    clearInputBuffer();
                    printf("Invalid account ID.\n");
                    break;
                }

                clearInputBuffer();

                if (accountId <= 0)
                {
                    printf("Account ID must be greater than zero.\n");
                    break;
                }

                printf("Enter account holder name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                {
                    printf("Unable to read name.\n");
                    break;
                }

                name[strcspn(name, "\n")] = '\0';

                printf("Enter opening balance: ");

                if (scanf("%lld", &openingBalance) != 1)
                {
                    clearInputBuffer();
                    printf("Invalid balance.\n");
                    break;
                }

                clearInputBuffer();

                if (openingBalance < 0)
                {
                    printf("Opening balance cannot be negative.\n");
                    break;
                }

                showResult(createAccount(accountId, name, openingBalance));
                break;

            case 2:
                if (!authorizeOperation(role, "view_accounts"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                viewAccounts();
                break;

            case 3:
                if (!authorizeOperation(role, "deposit"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                executeTransaction(TRANSACTION_DEPOSIT);
                break;

            case 4:
                if (!authorizeOperation(role, "withdraw"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                executeTransaction(TRANSACTION_WITHDRAW);
                break;

            case 5:
                if (!authorizeOperation(role, "transfer"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                executeTransaction(TRANSACTION_TRANSFER);
                break;

            case 6:
                if (!authorizeOperation(role, "transaction_report"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                generateTransactionReport();
                break;

            case 7:
                if (!authorizeOperation(role, "memory_report"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                generateMemoryUsageReport();
                break;

            case 8:
                if (!authorizeOperation(role, "leak_report"))
                {
                    showResult(E012_ACCESS_DENIED);
                    break;
                }

                generateLeakReport();
                break;

            case 9:
                printf("Logging out. Goodbye!\n");
                break;

            default:
                printf("Invalid menu choice.\n");
                break;
        }

    } while (choice != 9);

    secureAllocCleanup();

    return 0;
}
