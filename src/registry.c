#include "../include/nova.h"

#define MAX_HANDLERS 20
static StatementHandler handlers[MAX_HANDLERS];
static int handler_count = 0;

void register_handler(StatementHandler handler) {
    if (handler_count < MAX_HANDLERS) {
        handlers[handler_count++] = handler;
    }
}

int execute_statement(const char *line, Variable *vars, int *var_count) {
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i](line, vars, var_count)) {
            return 1;
        }
    }
    return 0;
}
