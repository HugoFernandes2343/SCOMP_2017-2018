#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int numbers[10];
	int current;
	int write_index;
} stru;
void fail_unmap(int r);
void fail_close(int r);
void fail_unlink(int r);
void fail_sem(sem_t *s);
void fail_sem_unlink(int s);
#endif
