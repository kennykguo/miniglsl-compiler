#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof(node));
  ast->kind = kind;
  
  va_start(args, kind);
  
  switch(kind) {
    case BINARY_EXPR_NODE:
      ast->binary_expr.op = va_arg(args, int);
      ast->binary_expr.left = va_arg(args, node *);
      ast->binary_expr.right = va_arg(args, node *);
      break;
        
    case UNARY_EXPR_NODE:
      ast->unary_expr.op = va_arg(args, int);
      ast->unary_expr.operand = va_arg(args, node *);
      break;
        
    case INT_LITERAL_NODE:
      ast->int_literal.value = va_arg(args, int);
      break;
        
    case FLOAT_LITERAL_NODE:
      ast->float_literal.value = va_arg(args, double);
      break;
        
    case BOOL_LITERAL_NODE:
      ast->bool_literal.value = va_arg(args, int);
      break;
        
    case VAR_NODE:
      ast->variable.identifier = va_arg(args, char *);
      ast->variable.is_array = va_arg(args, int);
      if (ast->variable.is_array) {
          ast->variable.array_index = va_arg(args, int);
      }
      break;
      
    case FUNCTION_NODE:
      ast->function.func_id = va_arg(args, int);
      ast->function.arguments = va_arg(args, node *);
      break;
        
    case CONSTRUCTOR_NODE:
      ast->constructor.type = va_arg(args, node *);
      ast->constructor.arguments = va_arg(args, node *);
      break;
        
    case ASSIGNMENT_NODE:
      ast->assignment.variable = va_arg(args, node *);
      ast->assignment.expr = va_arg(args, node *);
      break;

    case IF_STMT_NODE:
      ast->if_stmt.condition = va_arg(args, node *);
      ast->if_stmt.then_stmt = va_arg(args, node *);
      ast->if_stmt.else_stmt = va_arg(args, node *);
      break;
      
    case SCOPE_NODE:
      ast->scope.declarations = va_arg(args, node *);
      ast->scope.statements = va_arg(args, node *);
      break;
        
    case DECLARATION_NODE:
      ast->declaration.is_const = va_arg(args, int);
      ast->declaration.type = va_arg(args, node *);
      ast->declaration.identifier = va_arg(args, char *);
      ast->declaration.initializer = va_arg(args, node *);
      break;
        
    case TYPE_NODE:
      ast->type.type_code = va_arg(args, int);
      ast->type.vec_size = va_arg(args, int);
      break;

    case ARGUMENTS_NODE:
    case DECLARATIONS_NODE:
    case STATEMENTS_NODE:
      ast->list.item = va_arg(args, node *);
      ast->list.next = va_arg(args, node *);
      break;
        
    default: 
      break;
  }
  
  va_end(args);
  return ast;
}


void ast_free(node *ast) {

}

void ast_print(node * ast) {

}
