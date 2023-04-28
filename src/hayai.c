/* INCLUDES */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* DEFINES / ENUMS */

// Converts ASCII character k into ASCII character equivalent to keypress CTRL+k
#define CTRL_KEY(k) ((k)&0x1f)

#define HAYAI_VERSION "0.0.1"

enum editor_key {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
};

/* DATA */

struct editor_config {
    int cx, cy;
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

int editor_read_key() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }

    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) {
            return '\x1b';
        }
        if (read(STDIN_FILENO, &seq[1], 1) != 1) {
            return '\x1b';
        }

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {  // Page Up & Down
                if (read(STDIN_FILENO, &seq[2], 1) != 1) {
                    return '\x1b';
                }
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1':
                            return HOME_KEY;
                        case '3':
                            return DEL_KEY;
                        case '4':
                            return END_KEY;
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                        case '7':
                            return HOME_KEY;
                        case '8':
                            return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {  // Arrow keys
                    case 'A':
                        return ARROW_UP;
                    case 'B':
                        return ARROW_DOWN;
                    case 'C':
                        return ARROW_RIGHT;
                    case 'D':
                        return ARROW_LEFT;
                    case 'F':
                        return END_KEY;
                    case 'H':
                        return HOME_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
            }
        }
        return '\x1b';
    } else {
        return c;
    }
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

    // Try to use ioctl to find terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // Try to move cursor 999 positions right and down
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
            return -1;
        }
        // Terminal size is wherever the cursor ends up
        return get_cursor_pos(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/* APPEND BUFFER */

struct abuf {
    char* b;
    int len;
};

#define ABUF_INIT \
    { NULL, 0 }

void ab_append(struct abuf* ab, const char* s, int len) {
    char* new = realloc(ab->b, ab->len + len);

    if (new == NULL) {
        return;
    }
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void ab_free(struct abuf* ab) { free(ab->b); }

/* OUTPUT FUNCTIONS */
void editor_draw_rows(struct abuf* ab) {
    for (int i = 0; i < E.screenrows; i++) {
        if (i == E.screenrows / 3) {
            char welcome[80];
            int welcome_len =
                snprintf(welcome, sizeof(welcome), "Hayai Editor -- version %s",
                         HAYAI_VERSION);
            if (welcome_len > E.screencols) {
                welcome_len = E.screencols;
            }

            // Padding
            int padding = (E.screencols - welcome_len) / 2;
            if (padding != 0) {  // Add ~ at start of line if padding required
                ab_append(ab, "~", 1);
                padding--;
            }
            while (padding--) {
                ab_append(ab, " ", 1);  // pad
            }

            ab_append(ab, welcome, welcome_len);
        } else {
            ab_append(ab, "~", 1);
        }

        ab_append(ab, "\x1b[K", 3);
        if (i < E.screenrows - 1) {
            ab_append(ab, "\r\n", 2);
        }
    }
}

void editor_refresh_screen() {
    struct abuf ab = ABUF_INIT;

    ab_append(&ab, "\x1b[?25l", 6);  // hide cursor before refreshing screen
    ab_append(&ab, "\x1b[H", 3);

    editor_draw_rows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1,
             E.cx + 1);  // add one to convert to terminal's 1 index positions
    ab_append(&ab, buf, strlen(buf));

    ab_append(&ab, "\x1b[?25h", 6);  // show cursor after refreshing screen

    write(STDOUT_FILENO, ab.b, ab.len);
    ab_free(&ab);
}

/* INPUT FUNCTIONS */

void editor_move_cursor(int key) {
    switch (key) {
        case ARROW_UP:
            if (E.cy > 0) E.cy--;
            break;
        case ARROW_LEFT:
            if (E.cx > 0) E.cx--;
            break;
        case ARROW_DOWN:
            if (E.cy < E.screenrows - 1) E.cy++;
            break;
        case ARROW_RIGHT:
            if (E.cx < E.screencols - 1) E.cx++;
            break;
    }
}

void editor_process_key() {
    int c = editor_read_key();
    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;

        case HOME_KEY:
            E.cx = 0;
            break;
        case END_KEY:
            E.cx = E.screencols - 1;
            break;

        case PAGE_UP:
        case PAGE_DOWN: {  // Scope to get rid of warning
            int times = E.screenrows;
            while (times--) {
                editor_move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
        } break;

        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editor_move_cursor(c);
            break;
    }
}

/* INIT */
void editor_init() {
    E.cx = 0;
    E.cy = 0;
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