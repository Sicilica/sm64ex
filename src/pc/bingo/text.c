#include "text.h"

u8* alloc_and_convert_chars_to_dialog(const char* raw) {
    u8* out = malloc(sizeof(u8) * strlen(raw) + 1);
    convert_chars_to_dialog(raw, &out, strlen(raw) + 1);
    return out;
}

void convert_chars_to_dialog(const char* raw, u8** out, int outlen) {
    int i;
    for (i = 0; i < outlen - 1 && *raw != '\0'; raw++) {
        if (*raw >= 'A' && *raw <= 'Z') {
            out[i++] = (*raw - 'A') + 0x0A;
        } else if (*raw >= 'a' && *raw <= 'z') {
            out[i++] = (*raw - 'a') + 0x24;
        } else if (*raw >= '0' && *raw <= '9') {
            out[i++] = (*raw - '0') + 0x00;
        } else {
            switch (*raw) {
            case '\'': out[i++] = 0x3E; break;
            case '-': out[i++] = 0x9F; break;
            case '/': out[i++] = 0xD0; break;
            case '(': out[i++] = 0xE1; break;
            case ')': out[i++] = 0xE3; break;
            case '&': out[i++] = 0xE5; break;
            case '!': out[i++] = 0xF2; break;
            case '?': out[i++] = 0xF4; break;
            case '_': out[i++] = 0xF8; /*ellipsis*/ break;
            case '$': out[i++] = 0xF9; /*coin*/ break;
            case '@': out[i++] = 0xFA; /*filled star*/ break;
            case '*': out[i++] = 0xFB; /*multiply*/ break;
            case '#': out[i++] = 0xFD; /*unfilled star*/ break;
            case '\n': out[i++] = 0xFE; break;
            default:
                out[i++] = 0xF4; /*question mark*/
            }
        }
    }
    out[i] = 0xFF;
}
