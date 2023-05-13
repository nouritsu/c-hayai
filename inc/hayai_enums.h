enum editor_key {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,  // Any value of of range of char
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
};

enum editor_highlight {
    HL_NORMAL = 0,
    HL_STRING,
    HL_COMMENT,
    HL_NUMBER,
    HL_MATCH,
    HL_KW1,
    HL_KW2,
};
