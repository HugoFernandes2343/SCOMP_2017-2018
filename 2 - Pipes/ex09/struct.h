#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int customer_code;
	int product_code;
	int quantity;
} record;
void fail_fork(int p);
void fail_pipe(int p);
void incrementRecords(record* sales);
#endif
