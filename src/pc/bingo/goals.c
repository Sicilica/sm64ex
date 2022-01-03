#include <stdbool.h>
#include <stdio.h>

#include "sm64.h"
#include "game/level_update.h"

#include "bingo.h"

// confirmed
s32 save_file_get_total_cannon_count(s32 fileIndex) {
  s32 count = 0;
  for (s32 courseIndex = COURSE_MIN; courseIndex < COURSE_MAX - 1; courseIndex++) {
    if (save_file_get_cannon_flags(fileIndex, courseIndex - 1)) {
      count++;
    }
  }
  return count;
}

// confirmed
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

// NOTE
// gCurrCourseNum gives the current level, or 0 for castle / title / etc
// before evaluating any goal or the player pos, we should try
// to check if we're at title screen or credits

// semi-confirmed
bool bingo_have_N_total_stars(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
  return actual >= args.count;
}

// semi-confirmed
bool bingo_open_N_cannons(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_cannon_count(gCurrSaveFileNum - 1);
  return actual >= args.count;
}

// semi-confirmed
bool bingo_have_N_bonus_stars(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_BONUS_STAGES - 1, COURSE_MAX - 1);
  return actual >= args.count;
}

// confirmed
bool bingo_have_at_least_N_lives(struct BingoGoalArgs args) {
  s8 actual = gMarioState->numLives;
  return actual >= args.count;
}

bool bingo_have_no_lives(struct BingoGoalArgs args) {
  return gMarioState->numLives == 0;
}

// confirmed
bool bingo_lose_marios_cap(struct BingoGoalArgs args) {
  return (gMarioState->flags & MARIO_NORMAL_CAP) == 0;
}

// confirmed (need to test star 7?)
bool bingo_have_specific_star(struct BingoGoalArgs args) {
  u32 starFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, args.course - 1);
  return (starFlags & (1 << (args.star - 1))) != 0;
}

bool bingo_have_N_stars_from_set(struct BingoGoalArgs args) {
  s32 actual = bingo_get_star_count_from_set(args.starSet);
  return actual >= args.count;
}

bool bingo_have_N_stars_in_stage_X(struct BingoGoalArgs args) {
  s32 actual = save_file_get_course_star_count(gCurrSaveFileNum - 1, args.course - 1);
  return actual >= args.count;
}

// semi-confirmed
// bool bingo_have_N_coins_in_stage_X(struct BingoGoalArgs args) {
//   s32 actual = save_file_get_course_coin_score(gCurrSaveFileNum - 1, args.course - 1);
//   return actual >= args.count;
// }

bool bingo_have_N_stars_in_M_stages(struct BingoGoalArgs args) {
  s32 stageCount = 0;
  for (s32 courseIndex = COURSE_MIN; courseIndex < COURSE_BONUS_STAGES; courseIndex++) {
    s32 starCount = save_file_get_course_star_count(gCurrSaveFileNum - 1, courseIndex - 1);
    if (starCount >= args.count) {
      stageCount++;
    }
  }
  return stageCount >= args.count2;
}

bool bingo_have_toad_star(struct BingoGoalArgs args) {
  s32 saveFlags = save_file_get_flags();
  return (saveFlags & (1 << (23 + args.star))) != 0;
}

bool bingo_have_mips_star(struct BingoGoalArgs args) {
  u32 starFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, -1);
  return (starFlags & (1 << (2 + args.star))) != 0;
}

const char* bool_to_str(bool b) {
  if (b) {
    return "T";
  }
  return "F";
}

struct BingoStarRef redCoinStars[] = {
  { .course = COURSE_BOB, .star = 4 },
  { .course = COURSE_WF, .star = 4 },
  { .course = COURSE_JRB, .star = 4 },
  { .course = COURSE_CCM, .star = 4 },
  { .course = COURSE_BBH, .star = 4 },
  { .course = COURSE_HMC, .star = 2 },
  { .course = COURSE_LLL, .star = 3 },
  { .course = COURSE_SSL, .star = 5 },
  { .course = COURSE_DDD, .star = 3 },
  { .course = COURSE_SL, .star = 5 },
  { .course = COURSE_WDW, .star = 5 },
  { .course = COURSE_TTM, .star = 3 },
  { .course = COURSE_THI, .star = 5 },
  { .course = COURSE_TTC, .star = 6 },
  { .course = COURSE_RR, .star = 3 },
  { .course = COURSE_BITDW, .star = 1 },
  { .course = COURSE_BITFS, .star = 1 },
  { .course = COURSE_BITS, .star = 1 },
  { .course = COURSE_COTMC, .star = 1 },
  { .course = COURSE_TOTWC, .star = 1 },
  { .course = COURSE_VCUTM, .star = 1 },
  { .course = COURSE_WMOTR, .star = 1 },
  { .course = COURSE_SA, .star = 1 },
  { .course = COURSE_NONE }
};

struct BingoStarRef boxStars[] = {
  { .course = COURSE_BOB, .star = 3 },
  { .course = COURSE_JRB, .star = 1 },
  { .course = COURSE_JRB, .star = 5 },
  { .course = COURSE_SL, .star = 4 },
  { .course = COURSE_WDW, .star = 1 },
  { .course = COURSE_WDW, .star = 2 },
  { .course = COURSE_THI, .star = 2 },
  { .course = COURSE_RR, .star = 6 },
  { .course = COURSE_PSS, .star = 1 },
  { .course = COURSE_NONE }
};

struct BingoStarRef freestandingStars[] = {
  { .course = COURSE_BOB, .star = 6 },
  { .course = COURSE_WF, .star = 2 },
  { .course = COURSE_WF, .star = 3 },
  { .course = COURSE_WF, .star = 5 },
  { .course = COURSE_WF, .star = 6 },
  { .course = COURSE_JRB, .star = 6 },
  { .course = COURSE_CCM, .star = 6 },
  { .course = COURSE_BBH, .star = 3 },
  { .course = COURSE_HMC, .star = 1 },
  { .course = COURSE_HMC, .star = 3 },
  { .course = COURSE_HMC, .star = 4 },
  { .course = COURSE_HMC, .star = 5 },
  { .course = COURSE_HMC, .star = 6 },
  { .course = COURSE_LLL, .star = 4 },
  { .course = COURSE_LLL, .star = 5 },
  { .course = COURSE_LLL, .star = 6 },
  { .course = COURSE_SSL, .star = 2 },
  { .course = COURSE_SSL, .star = 3 },
  { .course = COURSE_DDD, .star = 1 },
  { .course = COURSE_DDD, .star = 6 },
  { .course = COURSE_SL, .star = 1 },
  { .course = COURSE_SL, .star = 3 },
  { .course = COURSE_SL, .star = 6 },
  { .course = COURSE_WDW, .star = 4 },
  { .course = COURSE_WDW, .star = 6 },
  { .course = COURSE_TTM, .star = 1 },
  { .course = COURSE_TTM, .star = 4 },
  { .course = COURSE_TTM, .star = 5 },
  { .course = COURSE_TTM, .star = 6 },
  { .course = COURSE_TTC, .star = 1 },
  { .course = COURSE_TTC, .star = 2 },
  { .course = COURSE_TTC, .star = 3 },
  { .course = COURSE_TTC, .star = 4 },
  { .course = COURSE_TTC, .star = 5 },
  { .course = COURSE_RR, .star = 1 },
  { .course = COURSE_RR, .star = 2 },
  { .course = COURSE_RR, .star = 4 },
  { .course = COURSE_RR, .star = 5 },
  { .course = COURSE_NONE }
};

struct BingoStarRef secretsStars[] = {
  { .course = COURSE_BOB, .star = 5 },
  { .course = COURSE_SSL, .star = 6 },
  { .course = COURSE_WDW, .star = 3 },
  { .course = COURSE_THI, .star = 4 },
  { .course = COURSE_NONE }
};

struct BingoStarRef slideStars[] = {
  { .course = COURSE_CCM, .star = 1 },
  { .course = COURSE_TTM, .star = 4 },
  { .course = COURSE_PSS, .star = 1 },
  { .course = COURSE_PSS, .star = 2 },
  { .course = COURSE_NONE }
};

struct BingoStarRef raceStars[] = {
  { .course = COURSE_BOB, .star = 2 },
  { .course = COURSE_CCM, .star = 3 },
  { .course = COURSE_THI, .star = 3 },
  { .course = COURSE_NONE }
};

struct BingoStarRef bossStars[] = {
  { .course = COURSE_BOB, .star = 1 },
  { .course = COURSE_WF, .star = 1 },
  // not counting BBH stars cuz you fight big boo 3 times
  // not counting big bully because you fight twice
  { .course = COURSE_SSL, .star = 4 },
  { .course = COURSE_THI, .star = 6 },
  { .course = COURSE_NONE }
};

struct BingoStarRef highestStars[] = {
  // TODO
  // { .course = COURSE_BOB, .star = 0 },
  // { .course = COURSE_WF, .star = 0 },
  // { .course = COURSE_JRB, .star = 0 },
  // { .course = COURSE_CCM, .star = 0 },
  // { .course = COURSE_BBH, .star = 0 },
  // { .course = COURSE_HMC, .star = 0 },
  // { .course = COURSE_LLL, .star = 0 },
  // { .course = COURSE_SSL, .star = 0 },
  // { .course = COURSE_DDD, .star = 0 },
  // { .course = COURSE_SL, .star = 0 },
  // { .course = COURSE_WDW, .star = 0 },
  // { .course = COURSE_TTM, .star = 0 },
  // { .course = COURSE_THI, .star = 0 },
  // { .course = COURSE_TTC, .star = 0 },
  // { .course = COURSE_RR, .star = 0 },
  { .course = COURSE_NONE }
};

struct BingoGoal goals[] = {
  // BingoSync
  // { .label = "Red Coin Star in WF", .fn = bingo_have_specific_star, .args = { .course = COURSE_WF, .star = 4 } },
  // { .label = "100 Coin Star in WF", .fn = bingo_have_specific_star, .args = { .course = COURSE_WF, .star = 7 } },
  { .label = "Peach's Slide x 2", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_PSS, .count = 2 } },

  // All the stars that aren't in any set...
  { .label = "JRB - Can The Eel Come Out To Play?", .fn = bingo_have_specific_star, .args = { .course = COURSE_JRB, .star = 2 } },
  { .label = "CCM - Li'l Penguin Lost", .fn = bingo_have_specific_star, .args = { .course = COURSE_CCM, .star = 2 } },
  { .label = "CCM - Snowman's Lost His Head", .fn = bingo_have_specific_star, .args = { .course = COURSE_CCM, .star = 5 } },

  // Goal types I would want to generate:
  // - goal for every specific star in the game (these will generally be easy but will encourage going to more levels)
  //    (these can't really be generated)
  // - goal for N stars from every course in the game
  // - goal for N secret stars
  // - goal for N stars from X different courses
  // - goal for N 100 coin stars (and other categories)
  // - goal for N total coins
  // - goal for 50 coins from N stages
  // - goal for N total stars
  // - goal for N cannons

  { .label = "lose your cap", .fn = bingo_lose_marios_cap },
  { .label = "have 0 extra lives", .fn = bingo_have_no_lives },
  { .label = "top floor toad star (req 35)", .fn = bingo_have_toad_star, .args = { .star = 3 } },
  { .label = "mips 2 (req 50)", .fn = bingo_have_mips_star, .args = { .star = 2 } },
  { .label = NULL }
};

void bingo_debug_text(char* buffer) {
  sprintf(buffer, "%d %d",
    // bool_to_str(goals[0].fn(goals[0].args)),
    // bool_to_str(goals[1].fn(goals[1].args))
    bingo_get_star_count_from_set(slideStars),
    bingo_get_star_count_from_set(boxStars)
  );
}

struct BingoGoal* get_possible_bingo_goals(void) {
  return goals;
}
