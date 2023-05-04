#include <stdlib.h>
#include <string.h>

#ifndef EROW_H
#define EROW_H
typedef struct erow {
    int size, rsize;
    char *chars, *render;
} erow;

void row_update(erow* row);
void row_insert_char(erow* row, int at, char c, int* dirty);
void row_append_string(erow* row, char* s, size_t len, int* dirty);
void row_delete_char(erow* row, int at, int* dirty);
void row_free(erow* row);
int row_cs_to_rx(erow* row, int cx);

#endif