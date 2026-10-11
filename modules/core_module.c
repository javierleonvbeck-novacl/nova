#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/nova.h"

// Handle print statement
int handle_print(const char *line, Variable *vars, int *var_count) {
    if (strncmp(line, "print(", 6) == 0) {
        char content[256];
        strcpy(content, line + 6);
        content[strlen(content) - 1] = '\0';
        trim(content);

        if (content[0] == '"' && content[strlen(content)-1] == '"') {
            content[strlen(content)-1] = '\0';
            printf("%s\n", content + 1);
        } else {
            printf("%d\n", get_var(vars, *var_count, content));
        }
        return 1;
    }
    return 0;
}

// Handle direct variable assignment (e.g., counter = 5 or counter = counter - 1)
int handle_assignment(const char *line, Variable *vars, int *var_count) {
    char *eq = strchr(line, '=');
    if (eq != NULL) {
        // Extract variable name before '='
        char name[32];
        int name_len = eq - line;
        if (name_len >= sizeof(name)) name_len = sizeof(name) - 1;
        strncpy(name, line, name_len);
        name[name_len] = '\0';
        trim(name);

        // Extract expression after '='
        char expr[128];
        strcpy(expr, eq + 1);
        trim(expr);

        // Check for subtraction operation (e.g., counter - 1)
        char var_name[32];
        int sub;
        if (sscanf(expr, "%s - %d", var_name, &sub) == 2) {
            int val = get_var(vars, *var_count, var_name);
            set_var(vars, var_count, name, val - sub);
        } else {
            // Direct number assignment or variable copy
            if (isdigit(expr[0]) || (expr[0] == '-' && isdigit(expr[1]))) {
                set_var(vars, var_count, name, atoi(expr));
            } else {
                int val = get_var(vars, *var_count, expr);
                set_var(vars, var_count, name, val);
            }
        }
        return 1;
    }
    return 0;
}

// Core module entry point combining basic features
int handle_core(const char *line, Variable *vars, int *var_count) {
    if (handle_print(line, vars, var_count)) return 1;
    if (handle_assignment(line, vars, var_count)) return 1;
    return 0;
}

__attribute__((constructor)) void init_core_module() {
    register_handler(handle_core);
}
