#include "text.h"

// This all comes from charmap.txt
// Note: one problem with this routine is that kana with dakuten actually
// convert to two bytes each, and some macros (like copyright mark) convert multiple
// chars into one byte output. But we don't care for now.
u8* convert_raw_str_to_charmap(const char* raw) {
    u8* out = malloc(sizeof(u8) * strlen(raw) + 1);
    out[strlen(raw)] = 0xFF;

    char* i = raw;
    u8* j = out;
    while (*i != '\0') {
        char c = *i;
        if (c >= 'A' && c <= 'Z') {
            *j = (c - 'A') + 0x0A;
        } else if (c >= 'a' && c <= 'z') {
            *j = (c - 'a') + 0x24;
        } else if (c >= '0' && c <= '9') {
            *j = (c - '0') + 0x00;
        } else if (c == ' ') {
            *j = 0x9E;
        } else if (c == '\n') {
            *j = 0xFE;
        } else if (c == '(') {
            *j = 0xE1;
        } else if (c == ')') {
            *j = 0xE3;
        } else if (c == ')') {
            *j = 0xE3;
        } else if (c == '\'') {
            *j = 0x3E;
        } else if (c == '/') {
            *j = 0xD0;
        } else if (c == '-') {
            *j = 0x9F;
        } else if (c == '_') {
            *j = 0xF8;  // ellipsis
        } else if (c == '$') {
            *j = 0xF9;  // coin
        } else if (c == '@') {
            *j = 0xFA;  // filled star
        } else if (c == '*') {
            *j = 0xFB;  // multiply
        } else if (c == '#') {
            *j = 0xFD;  // unfilled star
        } else if (c == '&') {
            *j = 0xE5;
        } else if (c == '!') {
            *j = 0xF2;
        } else if (c == '?') {
            *j = 0xF4;
        } else {
            *j = 0xF4; // '?'
        }

        i++;
        j++;
    }

    return out;
}
