#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/nova.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: nova <script.nv>\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    Variable vars[MAX_VARS];
    int var_count = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        trim(line);

        if (strlen(line) == 0 || strncmp(line, "//", 2) == 0) {
            continue;
        }

        execute_statement(line, vars, &var_count);
    }

    fclose(file);
    return 0;
}
