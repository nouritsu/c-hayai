#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "abuf.h"
#include "ekey.h"
#include "erow.h"
#include "hayai_constants.h"
#include "hayai_macros.h"

#ifndef EDITOR_H
#define EDITOR_H

struct editor_config {
    int cx, cy;
    int rx;
    int rowoff, coloff;
    int screenrows, screencols;
    int numrows;
    int dirty;  // file modified but not saved flag
    erow* row;
    char* filename;
    char statusmsg[80];
    time_t statusmsg_time;
    struct termios orig_termios;
};

void editor_init();
void die(const char* s);
void disable_raw_mode();
void enable_raw_mode();
int editor_read_key();
int get_cursor_pos(int* rows, int* cols);
int get_window_size(int* rows, int* cols);
void editor_insert_row(int at, char* s, size_t len);
void editor_del_row(int at);
void editor_insert_char(int c);
void editor_insert_new_line();
char* editor_row_to_string(int* buflen);
void editor_open(char* fname);
void editor_save();
void editor_scroll();
void editor_draw_rows(struct abuf* ab);
void editor_draw_statusbar(struct abuf* ab);
void editor_draw_msgbar(struct abuf* ab);
void editor_refresh_screen();
void editor_set_status(const char* fmt, ...);
char* editor_prompt(char* prompt);
void editor_move_cursor(int key);
void editor_process_key();
void editor_find();

#endif