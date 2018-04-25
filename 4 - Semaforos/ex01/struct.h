#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	FILE *from;
	FILE *to;
} files;
void fail_unmap(int r);
void fail_close(int r);
void fail_unlink(int r);
void fail_fork(int p);
void checkFile(FILE *f);
void fail_sem(int s);
void fail_sem_unlink(int s);
#endif
