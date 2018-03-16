#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int number;
	char string[1000];
} s1;
void fail_fork(int p);
void fail_pipe(int p);
#endif
