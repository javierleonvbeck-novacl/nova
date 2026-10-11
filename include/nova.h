#ifndef NOVA_H
#define NOVA_H

#define MAX_VARS 100
#define MAX_LINE_LENGTH 256

typedef struct {
    char name[32];
    int value;
} Variable;

typedef int (*StatementHandler)(const char *line, Variable *vars, int *var_count);

void register_handler(StatementHandler handler);
int execute_statement(const char *line, Variable *vars, int *var_count);

int get_var(Variable *vars, int var_count, const char *name);
void set_var(Variable *vars, int *var_count, const char *name, int value);
void trim(char *str);

#endif
