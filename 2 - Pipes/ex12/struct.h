#ifndef STRUCT_H
#define STRUCT_H
typedef struct{
	int product_code;
	char product_name[1000];
	int price;
} product;

typedef struct{
	int child;
	int code;
} query;
void fail_fork(int p);
void fail_pipe(int p);
#endif
