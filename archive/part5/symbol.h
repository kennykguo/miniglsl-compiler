#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "ast.h"

// entry per var
typedef struct symbol_entry {
    char *name; // var name
    int type_code; // INT_T, FLOAT_T, VEC_T eg
    int vec_size; // 0 for scalars and 0-2 for vec2/3/4
    int is_const;
    int is_write_only; // for result variables
    int is_read_only; // for uniform/attribute variables
} symbol_entry;

// symbol table one scope
typedef struct symbol_table {
    symbol_entry **entries; // array of pointers
    int size; // hash table size
    int count; // number of entries
    struct symbol_table *parent; // parent scope 
} symbol_table;

symbol_table* symbol_table_create(symbol_table *parent);
void symbol_table_free(symbol_table *table);
int symbol_table_insert(symbol_table *table, symbol_entry *entry);
symbol_entry* symbol_table_lookup(symbol_table *table, char *name);
symbol_entry* symbol_table_lookup_local(symbol_table *table, char *name);  // current scope only

#endif
