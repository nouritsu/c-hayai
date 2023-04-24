/* INCLUDES */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* DEFINES */
#define CTRL_KEY(k) ((k)&0x1f)

/* GLOBALS */

struct termios orig_termios;

/* TERMINAL FUNCTIONS */

void die(const char* s) {
    perror(s);
    exit(1);
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    };
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr");
    }
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;  // Modify flags for raw mode
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

/* INPUT FUNCTIONS */

void editor_process_key() {
    char c = editor_read_key();
    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
        default:
            if (iscntrl(c)) {
                printf("%d\r\n", c);
            } else {
                printf("%d ('%c')\r\n", c, c);
            }
    }
}

/* MAIN */

int main() {
    enable_raw_mode();

    while (1) {  // Main Loop
        editor_process_key();
    }
    return 0;
}