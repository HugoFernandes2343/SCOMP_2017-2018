#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int number;
	char name[50];
	char address[500];
} record;

typedef struct {
	record data[100];
} db;
void fail_unmap(int r);
void fail_close(int r);
void fail_unlink(int r);
void fail_sem(sem_t *s);
void consult (db *dt, sem_t *sem[]);
int insert (db *dt, sem_t *sem[], int indx);
void consultAll(db *dt, sem_t *sem[], int indx);
#endif
