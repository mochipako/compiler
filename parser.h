#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "vector_map.h"
#include "semantic.h"

struct SymTable;

enum type{
    TYPE_UNKNOWN,
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_LONG,
    TYPE_PTR
};

typedef struct Type{
    int ty;
    struct Type *ptrof;
}Type_t;

typedef struct Node{
    int op;
    struct Node *lhs;
    struct Node *rhs;

    //op == AST_INT
    int val; 

    //op == AST_ID
    char *name;

    //op == AST_FUNC
    int num_arg;
    //int num_var;
    //Map_t* var;

    //char** arg_name;
    //int arg_type[6];
    //char* arg_name[6];
    Vector_t* args;

    //for
    struct Node* lfor;
    struct Node* mfor;
    struct Node* rfor;

    //if
    struct Node* else_stmt;

    Type_t *type;

    struct SymTable* sym_table;

    Vector_t *unary;
}Node_t;

enum ast_kind{
    AST_INT,  
    AST_EQ, 
    AST_NEQ,
    AST_ID,
    AST_POST_FIX,
    AST_ARG_LIST,
    AST_FUNC,
    AST_COMP_STMT,
    AST_DEC,
    AST_EXPR,
    AST_WHILE,
    AST_IF,
    AST_ELSE,
    AST_DO,
    AST_FOR,
    AST_FUNC_DEC,
    AST_UNARY_EXPR,
    AST_UNARY_OP,
    AST_PARA_LIST
};


Node_t* new_node(int op, Node_t* lhs, Node_t* rhs);
Node_t* new_node_num(int val);
Node_t* new_node_DEC(char* name);
Node_t* new_node_UOP(int* unary);
void add_type(Node_t* node, Type_t* type);
void error(Token_t* tk);
void dump_node(Node_t* node, int num);

Node_t* primary_expr();
Node_t* postfix_expr();
Node_t* arg_expr_list();
Node_t* unary_expr();
Node_t* cast_expr();
Node_t* mul_expr();
Node_t* add_expr();
Node_t* equ_expr();
Node_t* assign_expr();
Node_t* expr();

Node_t* declaration();
Type_t* type_specifier();
Node_t* declarator();
Node_t* direct_declarator();
Vector_t* get_paras();
Node_t* para_declaration();

Node_t* stmt();
Node_t* compound_stmt();
Node_t* block_item();
Node_t* expr_stmt();
Node_t* sel_stmt();
Node_t* iter_stmt();

Node_t* translation_unit();
Node_t* function_definition();


extern char *ast_name[];
extern Map_t* global_var;

#endif
