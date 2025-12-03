#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast = NULL;

static void print_type(int type_code, int vec_size) {
    switch(type_code) {
        case INT_T: printf(" int"); break;
        case FLOAT_T: printf(" float"); break;
        case BOOL_T: printf(" bool"); break;
        case VEC_T: printf(" vec%d", vec_size + 1); break;
        case IVEC_T: printf(" ivec%d", vec_size + 1); break;
        case BVEC_T: printf(" bvec%d", vec_size + 1); break;
        default: break;
    }
}

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


// ast.c
void ast_free(node *ast) {
    if (!ast) return;
    
    switch(ast->kind) {
        case SCOPE_NODE:
            ast_free(ast->scope.declarations);
            ast_free(ast->scope.statements);
            break;
            
        case DECLARATIONS_NODE:
        case STATEMENTS_NODE:
        case ARGUMENTS_NODE:
            ast_free(ast->list.item);
            ast_free(ast->list.next);
            break;
            
        case DECLARATION_NODE:
            ast_free(ast->declaration.type);
            ast_free(ast->declaration.initializer);
            free(ast->declaration.identifier);
            break;
            
        case ASSIGNMENT_NODE:
            ast_free(ast->assignment.variable);
            ast_free(ast->assignment.expr);
            break;
            
        case IF_STMT_NODE:
            ast_free(ast->if_stmt.condition);
            ast_free(ast->if_stmt.then_stmt);
            ast_free(ast->if_stmt.else_stmt);
            break;
            
        case BINARY_EXPR_NODE:
            ast_free(ast->binary_expr.left);
            ast_free(ast->binary_expr.right);
            break;
            
        case UNARY_EXPR_NODE:
            ast_free(ast->unary_expr.operand);
            break;
        
        // base case
        case VAR_NODE:
            free(ast->variable.identifier);
            break;
            
        case FUNCTION_NODE:
            ast_free(ast->function.arguments);
            break;
            
        case CONSTRUCTOR_NODE:
            ast_free(ast->constructor.type);
            ast_free(ast->constructor.arguments);
            break;
            
        case TYPE_NODE:
        case INT_LITERAL_NODE:
        case FLOAT_LITERAL_NODE:
        case BOOL_LITERAL_NODE:
            // no children to free
            break;
            
        default:
            break;
    }
    
    free(ast);
}


void ast_print(node *ast) {
    if (!ast) return;
    
    switch(ast->kind) {
        case SCOPE_NODE:
            printf("(SCOPE ");
            ast_print(ast->scope.declarations);
            printf(" ");
            ast_print(ast->scope.statements);
            printf(")");
            break;
            
        case DECLARATIONS_NODE: {
            printf("(DECLARATIONS");
            node *decl = ast;
            while (decl) {
                printf(" ");
                ast_print(decl->list.item);
                decl = decl->list.next;
            }
            printf(")");
            break;
        }
            
        case STATEMENTS_NODE: {
            printf("(STATEMENTS");
            node *stmt = ast;
            while (stmt) {
                printf(" ");
                ast_print(stmt->list.item);
                stmt = stmt->list.next;
            }
            printf(")");
            break;
        }
            
        case DECLARATION_NODE:
            printf("(DECLARATION %s ", ast->declaration.identifier);
            ast_print(ast->declaration.type);
            if (ast->declaration.initializer) {
                printf(" ");
                ast_print(ast->declaration.initializer);
            }
            printf(")");
            break;
            
        case TYPE_NODE:
            switch(ast->type.type_code) {
                case INT_T: printf("int"); break;
                case FLOAT_T: printf("float"); break;
                case BOOL_T: printf("bool"); break;
                case VEC_T: printf("vec%d", ast->type.vec_size + 1); break;
                case IVEC_T: printf("ivec%d", ast->type.vec_size + 1); break;
                case BVEC_T: printf("bvec%d", ast->type.vec_size + 1); break;
            }
            break;
            
        case ASSIGNMENT_NODE:
            printf("(ASSIGN");
            print_type(ast->assignment.variable->result_type, ast->assignment.variable->result_vec_size);
            printf(" ");
            ast_print(ast->assignment.variable);
            printf(" ");
            ast_print(ast->assignment.expr);
            printf(")");
            break;
            
        case IF_STMT_NODE:
            printf("(IF ");
            ast_print(ast->if_stmt.condition);
            printf(" ");
            ast_print(ast->if_stmt.then_stmt);
            if (ast->if_stmt.else_stmt) {
                printf(" ");
                ast_print(ast->if_stmt.else_stmt);
            }
            printf(")");
            break;
            
        case BINARY_EXPR_NODE:
            printf("(BINARY");
            print_type(ast->result_type, ast->result_vec_size);
            printf(" ");
            switch(ast->binary_expr.op) {
                case AND: printf("&& "); break;
                case OR: printf("|| "); break;
                case EQ: printf("== "); break;
                case NEQ: printf("!= "); break;
                case '<': printf("< "); break;
                case LEQ: printf("<= "); break;
                case '>': printf("> "); break;
                case GEQ: printf(">= "); break;
                case '+': printf("+ "); break;
                case '-': printf("- "); break;
                case '*': printf("* "); break;
                case '/': printf("/ "); break;
                case '^': printf("^ "); break;
            }
            ast_print(ast->binary_expr.left);
            printf(" ");
            ast_print(ast->binary_expr.right);
            printf(")");
            break;
            
        case UNARY_EXPR_NODE:
            printf("(UNARY");
            print_type(ast->result_type, ast->result_vec_size);
            printf(" %c ", ast->unary_expr.op);
            ast_print(ast->unary_expr.operand);
            printf(")");
            break;
            
        case INT_LITERAL_NODE:
            printf("%d", ast->int_literal.value);
            break;
            
        case FLOAT_LITERAL_NODE:
            printf("%f", ast->float_literal.value);
            break;
            
        case BOOL_LITERAL_NODE:
            printf("%s", ast->bool_literal.value ? "true" : "false");
            break;
            
        case VAR_NODE:
            if (ast->variable.is_array) {
                printf("(INDEX");
                print_type(ast->result_type, ast->result_vec_size);
                printf(" %s %d)", ast->variable.identifier, ast->variable.array_index);
            } else {
                printf("%s", ast->variable.identifier);
            }
            break;
            
        case FUNCTION_NODE: {
            printf("(CALL ");
            switch(ast->function.func_id) {
                case DP3: printf("dp3"); break;
                case LIT: printf("lit"); break;
                case RSQ: printf("rsq"); break;
            }
            node *arg = ast->function.arguments;
            while (arg) {
                printf(" ");
                ast_print(arg->list.item);
                arg = arg->list.next;
            }
            printf(")");
            break;
        }
            
        case CONSTRUCTOR_NODE: {
            printf("(CALL ");
            ast_print(ast->constructor.type);
            node *arg = ast->constructor.arguments;
            while (arg) {
                printf(" ");
                ast_print(arg->list.item);
                arg = arg->list.next;
            }
            printf(")");
            break;
        }
            
        default:
            break;
    }
}