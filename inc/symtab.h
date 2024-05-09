

#ifndef SYMTAB_H
#define SYMTAB_H

#include "tinysf.h"

void initialize_symtab();
int sym_exists(const char *name);
tinysf_s get_value(const char *name);
void insert_symbol(const char *name, tinysf_s value);
void teardown_symtab();

#endif
