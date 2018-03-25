#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	char id[50];
	char password[50];
} user;
void fail_fork(int p);
void fail_pipe(int p);
#endif
