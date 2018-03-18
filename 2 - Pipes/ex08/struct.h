#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int number;
	char string[10];
} s1;
void fail_fork(int p);
void fail_pipe(int p);
#endif
