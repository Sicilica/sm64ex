#include "save.h"

s32 save_file_get_total_cannon_count(s32 fileIndex) {
  s32 count = 0;
  for (s32 courseIndex = COURSE_MIN; courseIndex < COURSE_MAX - 1; courseIndex++) {
    if (save_file_get_cannon_flags(fileIndex, courseIndex - 1)) {
      count++;
    }
  }
  return count;
}

s32 save_file_get_total_coin_count(s32 fileIndex) {
  s32 count = 0;
  for (s32 courseIndex = COURSE_MIN; courseIndex < COURSE_BONUS_STAGES; courseIndex++) {
    count += save_file_get_course_coin_score(fileIndex, courseIndex - 1);
  }
  return count;
}

s32 bingo_get_star_count_from_set(const struct BingoStarRef* stars) {
  s32 count = 0;
  for (int i = 0; stars[i].course != COURSE_NONE; i++) {
    u32 flags = save_file_get_star_flags(gCurrSaveFileNum - 1, stars[i].course - 1);
    if ((flags & (1 << (stars[i].star - 1))) != 0) {
      count++;
    }
  }
  return count;
}
