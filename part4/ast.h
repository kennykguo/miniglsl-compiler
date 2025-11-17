
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete
//
// There are many ways of making AST nodes. The approach below is an example
// of a discriminated union. If you choose to use C++, then I suggest looking
// into inheritance

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;

typedef enum {
    UNKNOWN = 0,
    
    // expressions
    BINARY_EXPR_NODE,
    UNARY_EXPR_NODE,
    BOOL_LITERAL_NODE,
    INT_LITERAL_NODE,
    FLOAT_LITERAL_NODE,
    VAR_NODE, // variable reference (x / x[2],..,)
    FUNCTION_NODE, // function call (dp3, rsq, lit,..,)
    CONSTRUCTOR_NODE, // type constructor (vec3(,...,))
    
    // statements
    ASSIGNMENT_NODE,
    IF_STMT_NODE,
    SCOPE_NODE,
    
    // declarations
    DECLARATION_NODE,
    DECLARATIONS_NODE, // list of declarations
    
    // statements list
    STATEMENTS_NODE,
    
    // type nodes
    TYPE_NODE,
    
    // arguments list
    ARGUMENTS_NODE
} node_kind;

struct node_ {
    node_kind kind;
    
    union {
        // binary expression - left op right
        struct {
            int op;
            node *left;
            node *right;
        } binary_expr;
        
        // unary expression - op operand
        struct {
            int op;
            node *operand;
        } unary_expr;
        
        // literals
        struct {
            int value;
        } int_literal;
        
        struct {
            float value;
        } float_literal;
        
        struct {
            int value;  // 0 = false, 1 = true
        } bool_literal;
        
        // variable - identifier or identifier[index]
        struct {
            char *identifier;
            int is_array; // 0 if just 'x', 1 if 'x[i]'
            int array_index; // only valid if is_array == 1
        } variable;
        
        // function call: func_name(args)
        struct {
            int func_id; // DP3=0, LIT=1, RSQ=2
            node *arguments; // ARGUMENTS_NODE
        } function;
        
        // constructor: type(args)
        struct {
            node *type; // TYPE_NODE
            node *arguments; // ARGUMENTS_NODE
        } constructor;
        
        // assignment: var = expr
        struct {
            node *variable; // VAR_NODE
            node *expr; // expression
        } assignment;
        
        // if statement
        struct {
            node *condition; // expression
            node *then_stmt; // statement
            node *else_stmt; // statement or NULL
        } if_stmt;
        
        // scope - { declarations statements }
        struct {
            node *declarations; // DECLARATIONS_NODE
            node *statements; // STATEMENTS_NODE
        } scope;
        
        // declaration - - [const] type id [= expr]
        struct {
            int is_const;
            node *type; // TYPE_NODE
            char *identifier;
            node *initializer; // expression or NULL
        } declaration;
        
        // type
        struct {
            int type_code; // from parser: INT_T, FLOAT_T, etc.
            int vec_size; // 0 for scalars, 1-3 for vec2/3/4
        } type;
        
        // linked list nodes
        struct {
            node *item; // the actual node
            node *next; // next in list or NULL
        } list;
    };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);

#endif /* AST_H_ */
