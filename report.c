#include "report.h"
#include "secalloc.h"
void generateTransactionReport(void)
{
    FILE *file;
    char line[512];
    file = fopen(LOG_FILE, "r");
    printf("\\n--- Transaction Report ---\\n");
    if (file == NULL) {
        printf("No transaction log found yet.\\n");
        return;
    }
    while (fgets(line, sizeof(line), file) != NULL) printf("%s", line);
    fclose(file);
}
void generateMemoryUsageReport(void)
{
    printf("\\n--- Memory Usage Report ---\\n");
    generateMemoryReport();
}
void generateLeakReport(void)
{
    printf("\\n--- Memory Leak Report ---\\n");
    detectMemoryLeaks();
}
