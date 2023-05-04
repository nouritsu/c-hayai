#include "editor.h"

int main(int argc, char** argv) {
    enable_raw_mode();
    editor_init();
    if (argc >= 2) {
        editor_open(argv[1]);
    }

    editor_set_status("Ctrl-Q to Quit | Ctrl-S to Save");

    while (1) {  // Main Loop
        editor_refresh_screen();
        editor_process_key();
    }
    return 0;
}