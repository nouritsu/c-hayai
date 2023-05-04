#include "erow.h"

#include "hayai_constants.h"

void row_update(erow* row) {
    int tabs = 0;
    for (int i = 0; i < row->size; i++) {  // Count tabs
        if (row->chars[i] == '\t') {
            tabs++;
        }
    }

    free(row->render);
    /* Tabs are 8 characters long, 1 character out of 8 is already counted for
       in row->size, hence tabs * 7*/
    row->render = malloc(row->size + tabs * (HAYAI_TAB_STOP - 1) + 1);

    int idx = 0;
    for (int i = 0; i < row->size; i++) {
        if (row->chars[i] == '\t') {
            row->render[idx++] = ' ';
            while (idx % HAYAI_TAB_STOP != 0) row->render[idx++] = ' ';
        } else {
            row->render[idx++] = row->chars[i];
        }
    }

    row->render[idx] = '\0';
    row->rsize = idx;
}

void row_insert_char(erow* row, int at, char c, int* dirty) {
    if (at < 0 || at > row->size) at = row->size;     // oob check
    row->chars = realloc(row->chars, row->size + 2);  // room for null byte
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    row_update(row);

    (*dirty)++;
}

void row_append_string(erow* row, char* s, size_t len, int* dirty) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    row_update(row);
    (*dirty)++;
}

void row_delete_char(erow* row, int at, int* dirty) {
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    row_update(row);
    (*dirty)++;
}

void row_free(erow* row) {
    free(row->render);
    free(row->chars);
}

int row_cx_to_rx(erow* row, int cx) {
    int rx = 0;
    for (int i = 0; i < cx; i++) {
        if (row->chars[i] == '\t') {
            rx += (HAYAI_TAB_STOP - 1) - (rx % HAYAI_TAB_STOP);
        }
        rx++;
    }
    return rx;
}

int row_rx_to_cx(erow* row, int rx) {
    int cur_rx = 0;

    int cx;
    for (cx = 0; cx < row->size; cx++) {
        if (row->chars[cx] == '\t') {
            cur_rx += (HAYAI_TAB_STOP - 1) - (cur_rx % HAYAI_TAB_STOP);
        }
        cur_rx++;

        if (cur_rx > rx) return cx;
    }
    return cx;
}