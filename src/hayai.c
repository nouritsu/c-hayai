/* INCLUDES */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* DEFINES */

// Converts ASCII character k into ASCII character equivalent to keypress CTRL+k
#define CTRL_KEY(k) ((k)&0x1f)

/* DATA */

struct editor_config {
    int screenrows, screencols;
    struct termios orig_termios;
};

/* GLOBALS */

struct editor_config E;

/* TERMINAL FUNCTIONS */

void die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
        die("tcsetattr");
    };
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) {
        die("tcgetattr");
    }
    atexit(disable_raw_mode);

    struct termios raw = E.orig_termios;  // Modify flags for raw mode
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        die("tcsetattr");
    }
}

char editor_read_key() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }
    return c;
}

int get_cursor_pos(int* rows, int* cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
        return -1;
    }

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) {
            break;
        }
        if (buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') {
        return -1;
    }
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) {
        return -1;
    }
    return 0;
}

int get_window_size(int* rows, int* cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
            return -1;
        }
        return get_cursor_pos(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/* OUTPUT FUNCTIONS */
void editor_draw_rows() {
    for (int i = 0; i < E.screenrows; i++) {
        write(STDOUT_FILENO, "~\r\n", 1);

        if (i < E.screenrows - 1) {
            write(STDOUT_FILENO, "\r\n", 2);
        }
    }
}

void editor_refresh_screen() {
    write(STDOUT_FILENO, '\x1b[2J', 4);
    write(STDOUT_FILENO, '\x1b[H', 3);

    editor_draw_rows();
    write(STDOUT_FILENO, '\x1b[H', 3);
}

/* INPUT FUNCTIONS */

void editor_process_key() {
    char c = editor_read_key();
    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

/* INIT */
void editor_init() {
    if (get_window_size(&E.screenrows, &E.screencols) == -1) {
        die("get_window_size");
    }
}

/* MAIN */

int main() {
    enable_raw_mode();
    editor_init();

    while (1) {  // Main Loop
        editor_refresh_screen();
        editor_process_key();
    }
    return 0;
}