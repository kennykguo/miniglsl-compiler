#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"

#define HASH_SIZE 101 // random #

// hash function
static unsigned int hash(char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str;
        str++;
    }
    return hash % HASH_SIZE;
}

symbol_table* symbol_table_create(symbol_table *parent) {
    symbol_table *table = (symbol_table*)malloc(sizeof(symbol_table));
    table->entries = (symbol_entry**)calloc(HASH_SIZE, sizeof(symbol_entry*));
    table->size = HASH_SIZE;
    table->count = 0;
    table->parent = parent;
    return table;
}

void symbol_table_free(symbol_table *table) {
    if (!table) return;
    for (int i = 0; i < table->size; i++) {
        if (table->entries[i]) {
            free(table->entries[i]->name);
            free(table->entries[i]);
        }
    }
    free(table->entries);
    free(table);
}

int symbol_table_insert(symbol_table *table, symbol_entry *entry) {
    // check if already exists in current scope
    if (symbol_table_lookup_local(table, entry->name)) {
        return 0;  // duplicate declaration
    }
    // find empty slot  using linear probing
    unsigned int index = hash(entry->name);
    while (table->entries[index] != NULL) {
        index = (index + 1) % table->size;
    }
    table->entries[index] = entry;
    table->count++;
    return 1;  // success
}

symbol_entry* symbol_table_lookup_local(symbol_table *table, char *name) {
    if (!table) return NULL;
    unsigned int index = hash(name);
    int probes = 0;
    while (table->entries[index] != NULL && probes < table->size) {
        if (strcmp(table->entries[index]->name, name) == 0) {
            return table->entries[index];
        }
        index = (index + 1) % table->size;
        probes++;
    }
    return NULL;
}

symbol_entry* symbol_table_lookup(symbol_table *table, char *name) {
    // current scope search
    symbol_entry *entry = symbol_table_lookup_local(table, name);
    if (entry) return entry;
    // not found then search parent scopes
    if (table->parent) {
        return symbol_table_lookup(table->parent, name);
    }
    return NULL;
}