#ifndef ABUF_H
#define ABUF_H

struct abuf {
    char* b;
    int len;
};

#define ABUF_INIT \
    { NULL, 0 }

void ab_append(struct abuf* ab, const char* s, int len);
void ab_free(struct abuf* ab);

#endif