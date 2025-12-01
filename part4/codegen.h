#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

// main entry point
void generate_code(node *ast, FILE *output);

#endif