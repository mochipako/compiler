
int printf();

struct st{
    int x;
    int y;
    int z;
    struct st* s;
    char s;
    char t;
    char u;
    char* name;
};

int main(){
    struct st test;
    struct st test2;
    test.y = 3;
    return 0;
}