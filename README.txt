Banking Transaction Processing System with SecureAlloc
=======================================================

This folder contains the current C prototype source files.

Files:
- main.c: menu and program entry point
- account.c / account.h: account creation and lookup
- transaction.c / transaction.h: deposit, withdrawal, transfer
- sync.c / sync.h: account mutex locking
- secalloc.c / secalloc.h: SecureAlloc prototype and memory reports
- security.c / security.h: demo authentication and role checks
- logger.c / logger.h: transaction and error logs
- report.c / report.h: report functions
- Common.h: shared data types and definitions

Compile with GCC in an environment that supports POSIX threads:
    gcc -Wall -Wextra -std=c11 *.c -pthread -o banking_system

Run:
    ./banking_system

Demo credentials:
    Admin:    admin / admin123
    Customer: customer / cust123

Important:
This is a learning prototype, not a production banking application.
The current version does not yet implement account persistence despite defining
an accounts.dat filename. SecureAlloc tracking is not yet integrated throughout
the application and its tracking structures are not thread-safe. Do not use
these demo credentials or this software for real financial transactions.
