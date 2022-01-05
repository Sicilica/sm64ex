#ifndef BINGO_TEXT_H
#define BINGO_TEXT_H

#include "bingo.h"

u8* alloc_and_convert_chars_to_dialog(const char* raw);
void convert_chars_to_dialog(const char* raw, u8** out, int outlen)

#endif
