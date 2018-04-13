#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int number;
	char name[1000];
} student;
void fail_unmap(int r);
void fail_close(int r);
void fail_unlink(int r);
#endif
