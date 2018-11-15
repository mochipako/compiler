#include <stdio.h>
#include <assert.h>
#include "codegen.h"

int rsp_allign;
Map_t* var;
int num_jmp;

void codegen(Node_t* node){
    if(node == NULL){
        assert(0);
    }

    int num_arg;
    int tmp_num_jmp;
    switch(node->op){
        case AST_INT:
            printf("  movl $%d, %%eax\n", node->val);
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '+':
        case '-':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            if(node->op == '+'){
                printf("  addl %%ebx, %%eax\n");
            }
            else if(node->op == '-'){
                printf("  subl %%ebx, %%eax\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '*':
        case '/':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            if(node->op == '*'){
                printf("  imul %%ebx, %%eax\n");
            }
            else if(node->op == '/'){
                printf("  cltd\n");
                printf("  div %%ebx\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case AST_EQ:
        case AST_NEQ:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  popq %%rbx\n");
            printf("  popq %%rax\n");
            rsp_allign -= 8;
            printf("  cmpl %%eax, %%ebx\n");
            if(node->op == AST_EQ)   
                printf("  sete %%al\n");
            if(node->op == AST_NEQ)   
                printf("  setne %%al\n");
            printf("  movzbl %%al, %%eax\n");
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '=':
            codegen_lval(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            printf("  movl %%ebx, (%%rax)\n");
            //do not need this.
            //printf("  pushq %%rax\n");
            //rsp_allign += 4;
            break;

        case AST_ID:
            printf("  movl -%d(%%rbp), %%eax\n", *(int*)map_search(var, node->name));
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case AST_POST_FIX:
            num_arg = 0;
            if(node->rhs != NULL && node->rhs->op == AST_ARG_LIST) 
                num_arg = codegen_arg(node->rhs, 0);
            for(int arg = num_arg-1; arg >= 0; arg--){
                //max:6
                if(arg == 0) printf("  pop %%rdi\n");
                else if(arg == 1) printf("  pop %%rsi\n");
                else if(arg == 2) printf("  pop %%rdx\n");
                else if(arg == 3) printf("  pop %%rcx\n");
                else if(arg == 4) printf("  pop %%r8\n");
                else if(arg == 5) printf("  pop %%r9\n");
            }
            rsp_allign -= 4*num_arg;
            int diff_allign = rsp_allign % 16;
            int rem = (diff_allign != 0) ? 16 - (diff_allign) : 0;
            if(rem > 0) printf("  subq $%d, %%rsp\n", rem);
            printf("  call %s\n", node->lhs->name);
            if(rem > 0) printf("  addq $%d, %%rsp\n", rem);
            printf("  pushq %%rax\n");
            break;

        case AST_FUNC:
            printf("\n%s:\n", node->name);
            printf("  pushq %%rbp\n");
            printf("  movq %%rsp, %%rbp\n");

            var = node->var;
            rsp_allign = 4;
            if(node->num_var > 0){
                printf("  subq $%d, %%rsp\n", 8*(1+node->num_var));
                rsp_allign += 8*(1+node->num_var);
            }
            //printf("num_arg:%d\n", node->num_arg);
            for(int arg = 0; arg < node->num_arg; arg++){
                //max:6
                Node_t* node_arg = (Node_t*)vector_get(node->args->decl, arg);
                if(arg == 0)
                    printf("  movq %%rdi, -%d(%%rbp)\n", *(int*)map_search(var, node_arg->name));
                else if(arg == 1)
                    printf("  movq %%rsi, -%d(%%rbp)\n", *(int*)map_search(var, node_arg->name));
                else if(arg == 2)
                    printf("  movq %%rdx, -%d(%%rbp)\n", *(int*)map_search(var, node_arg->name));
                else if(arg == 3)
                    printf("  movq %%rcx, -%d(%%rbp)\n", *(int*)map_search(var, node_arg->name));
                else if(arg == 4)
                    printf("  movq %%r8, -%d(%%rbp)\n",  *(int*)map_search(var, node_arg->name));
                else if(arg == 5)
                    printf("  movq %%r9, -%d(%%rbp)\n",  *(int*)map_search(var, node_arg->name));
            }

            codegen(node->rhs);
            printf("  pop %%rax\n");

            printf("  movq %%rbp, %%rsp\n");
            printf("  pop %%rbp\n");
            printf("  ret\n");
            break;

        case AST_COMP_STMT:
            codegen_comp_stmt(node);
            break;

        case AST_WHILE:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            printf(".L%dbegin:\n", tmp_num_jmp);
            codegen(node->lhs);
            printf("  pop %%rax\n");
            printf("  cmp $0, %%rax\n");
            printf("  je .L%dend\n", tmp_num_jmp);
            codegen(node->rhs);
            printf("  jmp .L%dbegin\n", tmp_num_jmp);
            printf(".L%dend:\n", tmp_num_jmp);
            break;

        case AST_IF:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            if(node->else_stmt == NULL){
                codegen(node->lhs);
                printf("  pop %%rax\n");
                printf("  cmp $0, %%rax\n");
                printf("  je .L%dend\n", tmp_num_jmp);
                codegen(node->rhs);
                printf(".L%dend:\n", tmp_num_jmp);
            }
            else{
                codegen(node->lhs);
                printf("  pop %%rax\n");
                printf("  cmp $0, %%rax\n");
                printf("  je .L%dlatt\n", tmp_num_jmp);
                codegen(node->rhs);
                printf("  jmp .L%dend\n", tmp_num_jmp);
                printf(".L%dlatt:\n", tmp_num_jmp);
                codegen(node->else_stmt);
                printf(".L%dend:\n", tmp_num_jmp);
            }
            break;

        case AST_FOR:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            if(node->lfor != NULL) codegen(node->lfor);
            printf("  jmp .L%dmid\n", tmp_num_jmp);
            printf(".L%dright:\n", tmp_num_jmp);
            if(node->rfor != NULL) codegen(node->rfor);
            codegen(node->lhs);
            printf(".L%dmid:\n", tmp_num_jmp);
            if(node->mfor != NULL) codegen(node->mfor);
            printf("  pop %%rax\n");
            printf("  cmp $0, %%rax\n");
            printf("  jne .L%dright\n", tmp_num_jmp);
            break;

        default:
            printf("node : %s\n", ast_name[node->op]);
            assert(0);
            break;
    }
}


void codegen_lval(Node_t* node){
    if(node->op == AST_ID){
        printf("  leaq -%d(%%rbp), %%rax\n", *(int*)map_search(var, node->name));
        printf("  pushq  %%rax\n");
        rsp_allign += 4;
    }
    else{
        fprintf(stderr, "not ID lvalue\n");
        assert(0);
    }
}

int codegen_arg(Node_t* node, int num){
    if(node->op != AST_ARG_LIST){
        codegen(node);
        num++;
    }
    else{
        if(node->lhs != NULL){
            num = codegen_arg(node->lhs, num);
        }
        if(node->rhs != NULL){
            num = codegen_arg(node->rhs, num);
        }
    }

    return num;
}

void codegen_comp_stmt(Node_t* node){
    if(node->op != AST_COMP_STMT && node->op != AST_DEC){
        codegen(node);
    }
    else{
        if(node->lhs != NULL){
            codegen_comp_stmt(node->lhs);
        }
        if(node->rhs != NULL){
            codegen_comp_stmt(node->rhs);
        }
    }
}