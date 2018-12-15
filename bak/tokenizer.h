#ifndef TOKEN_H_
#define TOKEN_H_

#include "vector_map.h"

#define NUM_TK 1000

typedef struct Token{
    int kind;
    int value;
    char* name;
}Token_t;

enum token_kind{
    TK_INT,  
    TK_CHAR,  
    TK_KW_INT,  
    TK_KW_CHAR,  
    TK_KW_VOID,  
    TK_STRING,  
    TK_EQ,
    TK_NEQ,
    TK_ID,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_DO,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_RETURN,
    TK_CONT,
    TK_BREAK,
    TK_LSHIFT,
    TK_RSHIFT,
    TK_LOG_AND, //&&
    TK_LOG_OR,  //||
    TK_LEQ,     //<=
    TK_SEQ,     //>=
    TK_SIZEOF,
    TK_STRUCT,
    TK_ENUM,
    TK_ARROW,   //->
    TK_TYPEDEF,   
    TK_EOF
};

void tokenize(char *p);
int get_ident(char *id, char **p);
struct Token* read_token(int n);
struct Token* get_token();
int end_tokens();
void dump_tokens();
void consume_token(char token);
char *map_file(char *filename);

extern Token_t tokens[NUM_TK];
extern int token_idx;
extern int num_tokens;
extern char *token_name[];
extern Map_t* define;

#endif