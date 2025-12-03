// semantic.c
#include "semantic.h"
#include "symbol.h"
#include "parser.tab.h"
#include <stdlib.h>
#include "common.h"
#include <string.h>
#include <stdio.h>

static symbol_table *current_scope = NULL;
static int error_count = 0;

static int check_node(node *ast);
static void check_declaration(node *decl);
static int check_statement(node *stmt);
static int check_expression(node *expr);
static int get_expression_type(node *expr, int *vec_size);
static int types_match(int type1, int size1, int type2, int size2);
static int get_base_type(int type_code);
static int is_arithmetic_type(int type_code);
static void print_type(int type_code, int vec_size);
static int count_arguments(node *args);


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


// helper to add predefined variable
static void add_predefined(char *name, int type, int vec_size, int is_const, int write_only, int read_only) {
    symbol_entry *entry = (symbol_entry*)malloc(sizeof(symbol_entry));
    entry->name = strdup(name);
    entry->type_code = type;
    entry->vec_size = vec_size;
    entry->is_const = is_const;
    entry->is_write_only = write_only;
    entry->is_read_only = read_only;
    symbol_table_insert(current_scope, entry);
}

int semantic_check(node *ast) {
    error_count = 0;
    current_scope = symbol_table_create(NULL);
    
    // add predefined result variables (write-only)
    add_predefined("gl_FragColor", VEC_T, 3, 0, 1, 0);
    add_predefined("gl_FragDepth", BOOL_T, 0, 0, 1, 0);
    add_predefined("gl_FragCoord", VEC_T, 3, 0, 0, 1);

    // add predefined attribute variables (read-only)
    add_predefined("gl_TexCoord", VEC_T, 3, 0, 0, 1);
    add_predefined("gl_Color", VEC_T, 3, 0, 0, 1);
    add_predefined("gl_Secondary", VEC_T, 3, 0, 0, 1);
    add_predefined("gl_FogFragCoord", VEC_T, 3, 0, 0, 1);

    // add predefined uniform variables (read-only, const)
    add_predefined("gl_Light_Half", VEC_T, 3, 1, 0, 1);
    add_predefined("gl_Light_Ambient", VEC_T, 3, 1, 0, 1);
    add_predefined("gl_Material_Shininess", VEC_T, 3, 1, 0, 1);
    add_predefined("env1", VEC_T, 3, 1, 0, 1);
    add_predefined("env2", VEC_T, 3, 1, 0, 1);
    add_predefined("env3", VEC_T, 3, 1, 0, 1);
    
    check_node(ast);
    
    symbol_table_free(current_scope);
    return error_count;
}


// helper function to process declarations in correct order
static void process_declarations_in_order(node *decls) {
    if (!decls) return;
    // recurse to end first (reverses the order)
    process_declarations_in_order(decls->list.next);
    // then process this declaration
    check_declaration(decls->list.item);
}




static void process_statements_in_order(node *stmts) {
    if (!stmts) return;
    process_statements_in_order(stmts->list.next);
    check_statement(stmts->list.item);
}


static int check_node(node *ast) {
    if (!ast) return 0;
    
    switch(ast->kind) {
        case SCOPE_NODE: {
            symbol_table *new_scope = symbol_table_create(current_scope);
            symbol_table *old_scope = current_scope;
            current_scope = new_scope;
            
            // PASS 1: Add all declarations to symbol table (without checking initializers)
            node *curr_decl = ast->scope.declarations;
            while (curr_decl) {
                node *decl = curr_decl->list.item;
                if (decl && decl->kind == DECLARATION_NODE) {
                    symbol_entry *entry = (symbol_entry*)malloc(sizeof(symbol_entry));
                    entry->name = strdup(decl->declaration.identifier);
                    entry->type_code = decl->declaration.type->type.type_code;
                    entry->vec_size = decl->declaration.type->type.vec_size;
                    entry->is_const = decl->declaration.is_const;
                    entry->is_write_only = 0;
                    entry->is_read_only = 0;
                    
                    if (!symbol_table_insert(current_scope, entry)) {
                        fprintf(errorFile, "error - variable '%s' already declared in this scope\\n", entry->name);
                        error_count++;
                        errorOccurred = 1;
                        free(entry->name);
                        free(entry);
                    }
                }
                curr_decl = curr_decl->list.next;
            }
            
            // PASS 2: Check initializers now that all variables are in symbol table
            process_declarations_in_order(ast->scope.declarations);
            
            //process statements  
            node *stmts = ast->scope.statements;
            process_statements_in_order(ast->scope.statements);

            symbol_table_free(current_scope);
            current_scope = old_scope;
            break;
        }
        default:
            break;
    }
    
    return 0;
}

static void check_declaration(node *decl) {
    if (!decl) return;
    
    // Symbol already added in pass 1, just check initializer
    symbol_entry *entry = symbol_table_lookup(current_scope, decl->declaration.identifier);
    if (!entry) return; // Already reported error in pass 1
    
    // check initializer if present
    if (decl->declaration.initializer) {
        int expr_type, expr_size;
        if (check_expression(decl->declaration.initializer)) {
            expr_type = get_expression_type(decl->declaration.initializer, &expr_size);
            
            // check type match
            if (!types_match(entry->type_code, entry->vec_size, expr_type, expr_size)) {
                fprintf(errorFile, "error - type mismatch in initialization of '%s'\n", entry->name);
                error_count++;
                errorOccurred = 1;
            }
            
            // check const initialization rules
            if (entry->is_const) {
                // const must be initialized with literal or uniform
                if (decl->declaration.initializer->kind != INT_LITERAL_NODE &&
                    decl->declaration.initializer->kind != FLOAT_LITERAL_NODE &&
                    decl->declaration.initializer->kind != BOOL_LITERAL_NODE) {
                    // check if it's a uniform variable
                    if (decl->declaration.initializer->kind == VAR_NODE) {
                        symbol_entry *var = symbol_table_lookup(current_scope, 
                            decl->declaration.initializer->variable.identifier);
                        if (!var || !var->is_const || !var->is_read_only) {
                            fprintf(errorFile, "error - const variable '%s' must be initialized with literal or uniform\n", 
                                entry->name);
                            error_count++;
                            errorOccurred = 1;
                        }
                    } else {
                        fprintf(errorFile, "error - const variable '%s' must be initialized with literal or uniform\n", 
                            entry->name);
                        error_count++;
                        errorOccurred = 1;
                    }
                }
            }
        }
    }
}

static int check_statement(node *stmt) {
    if (!stmt) return 0;
    
    switch(stmt->kind) {
        case DECLARATION_NODE:
            // Handle declarations that appear mixed with statements (C99 style)
            check_declaration(stmt);
            break;
            
        case ASSIGNMENT_NODE: {
            // check variable exists
            char *var_name = stmt->assignment.variable->variable.identifier;
            symbol_entry *var = symbol_table_lookup(current_scope, var_name);

            if (!var) {
                fprintf(errorFile, "error - undeclared variable '%s'\n", var_name);
                error_count++;
                errorOccurred = 1;
                return 1;
            }

            // check if const
            if (var->is_const) {
                fprintf(errorFile, "error - cannot assign to const variable '%s'\n", var_name);
                error_count++;
                errorOccurred = 1;
                return 1;
            }

            // check if read-only (attributes / uniforms)
            if (var->is_read_only) {
                fprintf(errorFile, "error - cannot assign to read-only variable '%s'\n", var_name);
                error_count++;
                errorOccurred = 1;
                return 1;
            }

            // lhs can be indexed: temp[0], color[2], gl_FragColor[3], etc.
            if (stmt->assignment.variable->variable.is_array) {
                int elements = var->vec_size + 1;   // vec_size=3 → 4 components
                int idx = stmt->assignment.variable->variable.array_index;

                if (idx < 0 || idx >= elements) {
                    fprintf(errorFile,
                            "error - array index %d out of bounds for variable '%s'\n",
                            idx, var_name);
                    error_count++;
                    errorOccurred = 1;
                    return 1;
                }
            }

            // now check rhs expression normally (this *can* read write-only vars, but
            // check_expression prevents that already)
            if (!check_expression(stmt->assignment.expr)) {
                return 1;
            }

            // compute types for lhs and rhs using get_expression_type
            int lhs_type, lhs_size;
            int rhs_type, rhs_size;

            lhs_type = get_expression_type(stmt->assignment.variable, &lhs_size);
            rhs_type = get_expression_type(stmt->assignment.expr, &rhs_size);

            if (!types_match(lhs_type, lhs_size, rhs_type, rhs_size)) {
                fprintf(errorFile, "error - type mismatch in assignment to '%s'\n",
                        var_name);
                error_count++;
                errorOccurred = 1;
            }

            break;
        }


        
        case IF_STMT_NODE:
            // check condition is bool
            if (check_expression(stmt->if_stmt.condition)) {
                int cond_type, cond_size;
                cond_type = get_expression_type(stmt->if_stmt.condition, &cond_size);
                if (cond_type != BOOL_T || cond_size != 0) {
                    fprintf(errorFile, "error - if condition must be bool type\n");
                    error_count++;
                    errorOccurred = 1;
                }
            }
            check_statement(stmt->if_stmt.then_stmt);
            check_statement(stmt->if_stmt.else_stmt);
            break;
            
        case SCOPE_NODE:
            check_node(stmt);
            break;
            
        default:
            break;
    }
    
    return 0;
}

static int check_expression(node *expr) {
    if (!expr) return 0;
    
    switch(expr->kind) {
        case VAR_NODE: {
            symbol_entry *var = symbol_table_lookup(current_scope, expr->variable.identifier);
            if (!var) {
                fprintf(errorFile, "error - undeclared variable '%s'\n", expr->variable.identifier);
                error_count++;
                errorOccurred = 1;
                return 0;
            }
            
            if (var->is_write_only) {
                fprintf(errorFile, "error - cannot read from write-only variable '%s'\n", expr->variable.identifier);
                error_count++;
                errorOccurred = 1;
                return 0;
            }
            
            if (expr->variable.is_array) {
                int elements = var->vec_size + 1;  // vec_size=3 → 4 components
                if (expr->variable.array_index < 0 ||
                    expr->variable.array_index >= elements) {
                    fprintf(errorFile, "error - array index %d out of bounds for variable '%s'\n",
                            expr->variable.array_index, expr->variable.identifier);
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }

            // STORE TYPE
            expr->result_type = get_expression_type(expr, &(expr->result_vec_size));
            return 1;
        }
        
        case BINARY_EXPR_NODE: {
            if (!check_expression(expr->binary_expr.left) || 
                !check_expression(expr->binary_expr.right)) {
                return 0;
            }
            
            int left_type, left_size, right_type, right_size;
            left_type = get_expression_type(expr->binary_expr.left, &left_size);
            right_type = get_expression_type(expr->binary_expr.right, &right_size);
            
            int op = expr->binary_expr.op;
            
            if (op == AND || op == OR) {
                if (left_type != BOOL_T || right_type != BOOL_T) {
                    fprintf(errorFile, "error - logical operators require bool operands\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (left_size != right_size) {
                    fprintf(errorFile, "error - vector sizes must match for logical operators\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }
            else if (op == '<' || op == LEQ || op == '>' || op == GEQ) {
                if (!is_arithmetic_type(left_type) || !is_arithmetic_type(right_type)) {
                    fprintf(errorFile, "error - comparison operators require arithmetic types\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (left_size != 0 || right_size != 0) {
                    fprintf(errorFile, "error - comparison operators only work on scalars\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (get_base_type(left_type) != get_base_type(right_type)) {
                    fprintf(errorFile, "error - comparison operand types must match\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }
            else if (op == EQ || op == NEQ) {
                if (get_base_type(left_type) != get_base_type(right_type)) {
                    fprintf(errorFile, "error - equality operand types must match\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (left_size != right_size) {
                    fprintf(errorFile, "error - vector sizes must match\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }
            else if (op == '+' || op == '-' || op == '*' || op == '/' || op == '^') {
                if (!is_arithmetic_type(left_type) || !is_arithmetic_type(right_type)) {
                    fprintf(errorFile, "error - arithmetic operators require arithmetic types\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (get_base_type(left_type) != get_base_type(right_type)) {
                    fprintf(errorFile, "error - operand base types must match\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (op == '*') {
                    if (!(left_size == right_size || left_size == 0 || right_size == 0)) {
                        fprintf(errorFile, "error - invalid vector sizes for multiplication\n");
                        error_count++;
                        errorOccurred = 1;
                        return 0;
                    }
                } else if (op == '/' || op == '^') {
                    if (left_size != 0 || right_size != 0) {
                        fprintf(errorFile, "error - division and power only work on scalars\n");
                        error_count++;
                        errorOccurred = 1;
                        return 0;
                    }
                } else {
                    if (left_size != right_size) {
                        fprintf(errorFile, "error - vector sizes must match\n");
                        error_count++;
                        errorOccurred = 1;
                        return 0;
                    }
                }
            }
            
            // STORE TYPE
            expr->result_type = get_expression_type(expr, &(expr->result_vec_size));
            return 1;
        }
            
        case UNARY_EXPR_NODE: {
            if (!check_expression(expr->unary_expr.operand)) {
                return 0;
            }
            int operand_type, operand_size;
            operand_type = get_expression_type(expr->unary_expr.operand, &operand_size);
            
            if (expr->unary_expr.op == '!') {
                if (operand_type != BOOL_T) {
                    fprintf(errorFile, "error - logical NOT requires bool operand\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            } else if (expr->unary_expr.op == '-') {
                if (!is_arithmetic_type(operand_type)) {
                    fprintf(errorFile, "error - unary minus requires arithmetic type\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }
            // STORE TYPE
            expr->result_type = get_expression_type(expr, &(expr->result_vec_size));
            return 1;
        }
            
        case INT_LITERAL_NODE:
            // STORE TYPE
            expr->result_type = INT_T;
            expr->result_vec_size = 0;
            return 1;
            
        case FLOAT_LITERAL_NODE:
            // STORE TYPE
            expr->result_type = FLOAT_T;
            expr->result_vec_size = 0;
            return 1;
            
        case BOOL_LITERAL_NODE:
            // STORE TYPE
            expr->result_type = BOOL_T;
            expr->result_vec_size = 0;
            return 1;
            
        case CONSTRUCTOR_NODE: {
            int cons_type = expr->constructor.type->type.type_code;
            int cons_size = expr->constructor.type->type.vec_size;
            int arg_count = count_arguments(expr->constructor.arguments);
            
            if (cons_size == 0) {
                if (arg_count != 1) {
                    fprintf(errorFile, "error - scalar constructor needs 1 argument\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                node *arg = expr->constructor.arguments->list.item;
                check_expression(arg);
                int arg_type, arg_size;
                arg_type = get_expression_type(arg, &arg_size);
                if (!types_match(cons_type, 0, arg_type, arg_size)) {
                    fprintf(errorFile, "error - constructor argument type mismatch\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            } else {
                int expected_args = cons_size + 1; // vec2(size=1)=2 args, vec3(size=2)=3 args, vec4(size=3)=4 args
                if (arg_count != expected_args) {
                    fprintf(errorFile, "error - vector constructor needs %d arguments\n", expected_args);
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                node *arg_node = expr->constructor.arguments;
                while (arg_node) {
                    node *arg = arg_node->list.item;
                    check_expression(arg);
                    int arg_type, arg_size;
                    arg_type = get_expression_type(arg, &arg_size);
                    if (get_base_type(arg_type) != get_base_type(cons_type) || arg_size != 0) {
                        fprintf(errorFile, "error - constructor argument type mismatch\n");
                        error_count++;
                        errorOccurred = 1;
                        return 0;
                    }
                    arg_node = arg_node->list.next;
                }
            }
            // STORE TYPE
            expr->result_type = get_expression_type(expr, &(expr->result_vec_size));
            return 1;
        }
            
        case FUNCTION_NODE: {
            int func_id = expr->function.func_id;
            int arg_count = count_arguments(expr->function.arguments);
            
            if (func_id == DP3) {
                if (arg_count != 2) {
                    fprintf(errorFile, "error - dp3 requires 2 arguments\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                node *arg1 = expr->function.arguments->list.item;
                node *arg2 = expr->function.arguments->list.next->list.item;
                check_expression(arg1);
                check_expression(arg2);
                int type1, size1, type2, size2;
                type1 = get_expression_type(arg1, &size1);
                type2 = get_expression_type(arg2, &size2);
                
                // vec3 has size=2, vec4 has size=3
                if (!((size1 == 2 || size1 == 3) && (size2 == 2 || size2 == 3))) {
                    fprintf(errorFile, "error - dp3 requires vec3/vec4 or ivec3/ivec4 arguments\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                if (!types_match(type1, size1, type2, size2)) {
                    fprintf(errorFile, "error - dp3 arguments must have same type\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            } else if (func_id == RSQ) {
                if (arg_count != 1) {
                    fprintf(errorFile, "error - rsq requires 1 argument\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                node *arg = expr->function.arguments->list.item;
                check_expression(arg);
                int arg_type, arg_size;
                arg_type = get_expression_type(arg, &arg_size);
                if (!((arg_type == FLOAT_T || arg_type == INT_T) && arg_size == 0)) {
                    fprintf(errorFile, "error - rsq requires float or int scalar\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            } else if (func_id == LIT) {
                if (arg_count != 1) {
                    fprintf(errorFile, "error - lit requires 1 argument\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
                node *arg = expr->function.arguments->list.item;
                check_expression(arg);
                int arg_type, arg_size;
                arg_type = get_expression_type(arg, &arg_size);
                if (arg_type != VEC_T || arg_size != 3) {  // changed from 2 to 3
                    fprintf(errorFile, "error - lit requires vec4 argument\n");
                    error_count++;
                    errorOccurred = 1;
                    return 0;
                }
            }
            // STORE TYPE
            expr->result_type = get_expression_type(expr, &(expr->result_vec_size));
            return 1;
        }
            
        default:
            return 1;
    }
}

static int get_expression_type(node *expr, int *vec_size) {
    *vec_size = 0;
    
    if (!expr) return -1;
    
    switch(expr->kind) {
        case INT_LITERAL_NODE:
            return INT_T;
        case FLOAT_LITERAL_NODE:
            return FLOAT_T;
        case BOOL_LITERAL_NODE:
            return BOOL_T;
            
        case VAR_NODE: {
            symbol_entry *var = symbol_table_lookup(current_scope, expr->variable.identifier);
            if (var) {
                if (expr->variable.is_array) {
                    // indexing into vector returns BASE SCALAR type
                    *vec_size = 0;
                    return get_base_type(var->type_code);  // changed this line
                } else {
                    *vec_size = var->vec_size;
                    return var->type_code;
                }
            }
            return -1;
        }
            
        case BINARY_EXPR_NODE: {
            int left_type, left_size;
            left_type = get_expression_type(expr->binary_expr.left, &left_size);
            
            int op = expr->binary_expr.op;
            
            // comparison/logical ops return bool
            if (op == '<' || op == LEQ || op == '>' || op == GEQ || 
                op == EQ || op == NEQ || op == AND || op == OR) {
                *vec_size = left_size; // preserve vector size for logical ops
                return BOOL_T;
            }
            
            // arithmetic ops return type of operands
            // for *, if one is scalar and other is vector, result is vector
            if (op == '*') {
                int right_type, right_size;
                right_type = get_expression_type(expr->binary_expr.right, &right_size);
                *vec_size = (left_size > right_size) ? left_size : right_size;
                // FIX: return vector type, not scalar type
                if (left_size > right_size) {
                    return left_type;  // left is vector
                } else if (right_size > left_size) {
                    return right_type; // right is vector
                } else {
                    return left_type;  // both same size
                }
            }
            
            *vec_size = left_size;
            return left_type;
        }
            
        case UNARY_EXPR_NODE:
            return get_expression_type(expr->unary_expr.operand, vec_size);
            
        case CONSTRUCTOR_NODE:
            *vec_size = expr->constructor.type->type.vec_size;
            return expr->constructor.type->type.type_code;
            
        case FUNCTION_NODE: {
            int func_id = expr->function.func_id;
            if (func_id == DP3) {
                // returns int or float depending on argument type
                node *arg = expr->function.arguments->list.item;
                int arg_type, arg_size;
                arg_type = get_expression_type(arg, &arg_size);
                *vec_size = 0;
                return (arg_type == IVEC_T) ? INT_T : FLOAT_T;
            } else if (func_id == RSQ) {
                *vec_size = 0;
                return FLOAT_T;
            } else if (func_id == LIT) {
                *vec_size = 3;
                return VEC_T;
            }
            return -1;
        }
            
        default:
            return -1;
    }
}

static int types_match(int type1, int size1, int type2, int size2) {
    return (type1 == type2 && size1 == size2);
}

static int get_base_type(int type_code) {
    switch(type_code) {
        case INT_T:
        case IVEC_T:
            return INT_T;
        case FLOAT_T:
        case VEC_T:
            return FLOAT_T;
        case BOOL_T:
        case BVEC_T:
            return BOOL_T;
        default:
            return type_code;
    }
}

static int is_arithmetic_type(int type_code) {
    return (type_code == INT_T || type_code == FLOAT_T || 
            type_code == IVEC_T || type_code == VEC_T);
}

static int count_arguments(node *args) {
    int count = 0;
    while (args) {
        count++;
        args = args->list.next;
    }
    return count;
}