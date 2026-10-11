#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/nova.h"

#define MAX_ALIASES 20

typedef struct {
    char alias[32];
    char module[32];
} ModuleAlias;

// Tablas globales
Function functions[MAX_FUNCS];
int func_count = 0;

ModuleAlias module_aliases[MAX_ALIASES];
int alias_count = 0;

// Declaración previa
void execute_block(char lines[][MAX_LINE_LENGTH], int start, int end, Variable *vars, int *var_count, int *returned, int *return_val);

// Traducir alias en la línea (ej: hw.get_cpu_temp() -> get_cpu_temp())
void apply_aliases(char *line) {
    for (int a = 0; a < alias_count; a++) {
        char prefix[64];
        snprintf(prefix, sizeof(prefix), "%s.", module_aliases[a].alias);
        char *p = strstr(line, prefix);
        if (p != NULL) {
            memmove(p, p + strlen(prefix), strlen(p + strlen(prefix)) + 1);
            break;
        }
    }
}

int call_function(const char *func_name, int arg_val, int *returned, int *return_val) {
    for (int f = 0; f < func_count; f++) {
        if (strcmp(functions[f].name, func_name) == 0) {
            Variable local_vars[MAX_VARS];
            int local_var_count = 0;
            
            if (strlen(functions[f].param) > 0) {
                set_var(local_vars, &local_var_count, functions[f].param, arg_val);
            }
            
            int loc_returned = 0;
            int loc_ret_val = 0;
            
            execute_block(functions[f].lines, 0, functions[f].line_count, local_vars, &local_var_count, &loc_returned, &loc_ret_val);
            
            if (returned) *returned = loc_returned;
            return loc_ret_val;
        }
    }
    printf("Error: Function '%s' not found.\n", func_name);
    return 0;
}

void execute_block(char lines[][MAX_LINE_LENGTH], int start, int end, Variable *vars, int *var_count, int *returned, int *return_val) {
    int i = start;
    while (i < end) {
        if (returned && *returned) break;

        char line[MAX_LINE_LENGTH];
        strcpy(line, lines[i]);
        trim(line);

        if (strlen(line) == 0 || strncmp(line, "//", 2) == 0) {
            i++;
            continue;
        }

        // 1. Manejar import y import ... as ... (ej: import hardware as hw)
        if (strncmp(line, "import ", 7) == 0) {
            char mod_name[32] = "", alias_name[32] = "";
            if (sscanf(line, "import %s as %s", mod_name, alias_name) == 2) {
                trim(mod_name);
                trim(alias_name);
                if (alias_count < MAX_ALIASES) {
                    strcpy(module_aliases[alias_count].module, mod_name);
                    strcpy(module_aliases[alias_count].alias, alias_name);
                    alias_count++;
                }
            } else if (sscanf(line, "import %s", mod_name) == 1) {
                trim(mod_name);
            }
            i++;
            continue;
        }

        // 2. Manejar importación de archivos de scripts (#import "file.nv")
        if (strncmp(line, "#import", 7) == 0) {
            char filename[128] = "";
            if (sscanf(line, "#import \"%[^\"]\"", filename) == 1 || sscanf(line, "#import %s", filename) == 1) {
                trim(filename);
                FILE *inc_file = fopen(filename, "r");
                if (inc_file) {
                    char inc_lines[MAX_LINES][MAX_LINE_LENGTH];
                    int inc_count = 0;
                    while (fgets(inc_lines[inc_count], sizeof(inc_lines[inc_count]), inc_file)) {
                        inc_lines[inc_count][strcspn(inc_lines[inc_count], "\n")] = 0;
                        trim(inc_lines[inc_count]);
                        inc_count++;
                    }
                    fclose(inc_file);
                    int dummy_ret = 0, dummy_val = 0;
                    execute_block(inc_lines, 0, inc_count, vars, var_count, &dummy_ret, &dummy_val);
                } else {
                    printf("Error: Could not import file '%s'\n", filename);
                }
            }
            i++;
            continue;
        }

        // 3. Definición de funciones (def)
        if (strncmp(line, "def ", 4) == 0) {
            char fname[32], param[32];
            if (sscanf(line, "def %31[a-zA-Z0-9_](%31[^)])", fname, param) >= 1) {
                trim(fname);
                trim(param);
                
                strcpy(functions[func_count].name, fname);
                strcpy(functions[func_count].param, param);
                functions[func_count].line_count = 0;

                int start_body = i + 1;
                int end_body = start_body;
                int brace_count = 1;

                while (end_body < end) {
                    if (strchr(lines[end_body], '{')) brace_count++;
                    if (strchr(lines[end_body], '}')) {
                        brace_count--;
                        if (brace_count == 0) break;
                    }
                    end_body++;
                }

                for (int l = start_body; l < end_body; l++) {
                    strcpy(functions[func_count].lines[functions[func_count].line_count], lines[l]);
                    functions[func_count].line_count++;
                }

                func_count++;
                i = end_body + 1;
                continue;
            }
        }

        // 4. Return
        if (strncmp(line, "return", 6) == 0) {
            char expr[64];
            if (sscanf(line, "return %s", expr) == 1) {
                trim(expr);
                int rval = isdigit(expr[0]) || (expr[0] == '-' && isdigit(expr[1])) ? atoi(expr) : get_var(vars, *var_count, expr);
                if (return_val) *return_val = rval;
                if (returned) *returned = 1;
            } else {
                if (return_val) *return_val = 0;
                if (returned) *returned = 1;
            }
            i++;
            continue;
        }

        // 5. Bucles while
        if (strncmp(line, "while", 5) == 0) {
            char var_name[32];
            int limit;
            sscanf(line, "while %s > %d", var_name, &limit);

            int start_body = i + 1;
            int end_body = start_body;
            int brace_count = 1;

            while (end_body < end) {
                if (strchr(lines[end_body], '{')) brace_count++;
                if (strchr(lines[end_body], '}')) {
                    brace_count--;
                    if (brace_count == 0) break;
                }
                end_body++;
            }

            while (get_var(vars, *var_count, var_name) > limit) {
                int loop_ret = 0, loop_val = 0;
                execute_block(lines, start_body, end_body, vars, var_count, &loop_ret, &loop_val);
                if (loop_ret) {
                    if (returned) *returned = loop_ret;
                    if (return_val) *return_val = loop_val;
                    break;
                }
            }

            i = end_body + 1;
            continue;
        }

        // Aplicar traducción de alias antes de evaluar la sentencia o función
        apply_aliases(line);

        // 6. Llamadas a funciones con asignación
        char fname_call[32], var_assign[32];
        int arg_call;
        if (sscanf(line, "%s = %31[a-zA-Z0-9_](%d)", var_assign, fname_call, &arg_call) == 3) {
            trim(var_assign);
            trim(fname_call);
            int res = call_function(fname_call, arg_call, NULL, NULL);
            set_var(vars, var_count, var_assign, res);
            i++;
            continue;
        }

        // 7. Instrucción estándar
        execute_statement(line, vars, var_count);
        i++;
    }
}

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

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count = 0;
    while (fgets(lines[line_count], sizeof(lines[line_count]), file)) {
        lines[line_count][strcspn(lines[line_count], "\n")] = 0;
        trim(lines[line_count]);
        line_count++;
    }
    fclose(file);

    Variable vars[MAX_VARS];
    int var_count = 0;
    int main_ret = 0, main_val = 0;

    execute_block(lines, 0, line_count, vars, &var_count, &main_ret, &main_val);
    return 0;
}