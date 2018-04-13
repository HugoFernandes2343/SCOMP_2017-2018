#ifndef STRUCT_H
#define STRUCT_H
typedef struct {
	int arr[10];
} array;
void fail_unmap(int r);
void fail_close(int r);
void fail_unlink(int r);
#endif
