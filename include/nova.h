#ifndef NOVA_H
#define NOVA_H

#define MAX_LINE_LENGTH 256
#define MAX_VARS 100
#define MAX_FUNCS 50
#define MAX_LINES 300
#define MAX_HANDLERS 20

typedef struct {
    char name[32];
    int value;
} Variable;

typedef struct {
    char name[32];
    char param[32];
    int start_line;
    int end_line;
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count;
} Function;

// Definición correcta requerida por registry.c
typedef int (*StatementHandler)(const char *line, Variable *vars, int *var_count);
typedef StatementHandler ModuleHandler; // Alias para compatibilidad

// Funciones del entorno y registro
void trim(char *str);
int get_var(Variable *vars, int var_count, const char *name);
void set_var(Variable *vars, int *var_count, const char *name, int value);
void register_handler(StatementHandler handler);
int execute_statement(const char *line, Variable *vars, int *var_count);

#endif