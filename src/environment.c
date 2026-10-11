#include <string.h>
#include "../include/nova.h"

void trim(char *str) {
    char *end;
    while(*str == ' ' || *str == '\t') str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) end--;
    *(end + 1) = 0;
}

int get_var(Variable *vars, int var_count, const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    return 0;
}

void set_var(Variable *vars, int *var_count, const char *name, int value) {
    for (int i = 0; i < *var_count; i++) {
        if (strcmp(vars[*var_count].name, name) == 0) {
            vars[*var_count].value = value;
            return;
        }
    }
    strcpy(vars[*var_count].name, name);
    vars[*var_count].value = value;
    (*var_count)++;
}
