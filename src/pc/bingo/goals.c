#include <stdbool.h>
#include <stdio.h>

#include "sm64.h"
#include "game/level_update.h"

#include "bingo.h"
#include "save.h"

// NOTE
// gCurrCourseNum gives the current level, or 0 for castle / title / etc
// before evaluating any goal or the player pos, we should try
// to check if we're at title screen or credits

bool bingo_have_N_total_stars(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
  return actual >= args.count;
}

bool bingo_open_N_cannons(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_cannon_count(gCurrSaveFileNum - 1);
  return actual >= args.count;
}

bool bingo_have_N_bonus_stars(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_BONUS_STAGES - 1, COURSE_MAX - 1);
  return actual >= args.count;
}

// bool bingo_have_at_least_N_lives(struct BingoGoalArgs args) {
//   s8 actual = gMarioState->numLives;
//   return actual >= args.count;
// }

bool bingo_have_no_lives(struct BingoGoalArgs args) {
  return gMarioState->numLives == 0;
}

bool bingo_lose_marios_cap(struct BingoGoalArgs args) {
  return (gMarioState->flags & MARIO_NORMAL_CAP) == 0;
}

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

bool bingo_have_N_coins_in_stage_X(struct BingoGoalArgs args) {
  s32 actual = save_file_get_course_coin_score(gCurrSaveFileNum - 1, args.course - 1);
  return actual >= args.count;
}

bool bingo_have_N_total_coins(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_coin_count(gCurrSaveFileNum - 1);
  return actual >= args.count;
}

bool bingo_have_N_coins_in_M_stages(struct BingoGoalArgs args) {
  s32 stageCount = 0;
  for (s32 courseIndex = COURSE_MIN; courseIndex < COURSE_BONUS_STAGES; courseIndex++) {
    s32 coinCount = save_file_get_course_coin_score(gCurrSaveFileNum - 1, courseIndex - 1);
    if (coinCount >= args.count) {
      stageCount++;
    }
  }
  return stageCount >= args.count2;
}

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
  // { .label = "WF\nReds", .fn = bingo_have_specific_star, .args = { .course = COURSE_WF, .star = 4 } },
  // { .label = "WF\n100$", .fn = bingo_have_specific_star, .args = { .course = COURSE_WF, .star = 7 } },
  // { .label = "PSS\n@*2", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_PSS, .count = 2 } },

  // All the stars that aren't in any set...
  // { .label = "JRB - Can The\nEel Come\nOut To Play?", .fn = bingo_have_specific_star, .args = { .course = COURSE_JRB, .star = 2 } },
  // { .label = "CCM - Li'l\nPenguin\nLost", .fn = bingo_have_specific_star, .args = { .course = COURSE_CCM, .star = 2 } },
  // { .label = "CCM -\nSnowman's Lost\nHis Head", .fn = bingo_have_specific_star, .args = { .course = COURSE_CCM, .star = 5 } },

  // Goal types I would want to generate:
  // - goal for every specific star in the game (these will generally be easy but will encourage going to more levels)
  //    (these can't really be generated)
  // - goal for N secret stars
  // - goal for N stars from X different courses
  // - goal for N 100 coin stars (and other categories)
  // - goal for 50 coins from N stages

  { .label = "Lose\nCap", .fn = bingo_lose_marios_cap },
  { .label = "0 Lives", .fn = bingo_have_no_lives },
  { .label = "Toad 2\n(@*25)", .fn = bingo_have_toad_star, .args = { .star = 2 } },
  { .label = "Toad 3\n(@*35)", .fn = bingo_have_toad_star, .args = { .star = 3 } },
  // { .label = "MIPS 2\n(@*50)", .fn = bingo_have_mips_star, .args = { .star = 2 } },

  { .label = "@*3\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 3 } },
  { .label = "@*3\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 3 } },
  { .label = "@*3\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 3 } },
  { .label = "@*3\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 3 } },
  { .label = "@*3\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 3 } },
  { .label = "@*3\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 3 } },
  { .label = "@*3\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 3 } },
  { .label = "@*3\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 3 } },
  { .label = "@*3\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 3 } },
  { .label = "@*3\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 3 } },
  { .label = "@*3\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 3 } },
  { .label = "@*3\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 3 } },
  { .label = "@*3\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 3 } },
  { .label = "@*3\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 3 } },
  { .label = "@*3\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 3 } },
  
  { .label = "@*4\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 4 } },
  { .label = "@*4\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 4 } },
  { .label = "@*4\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 4 } },
  { .label = "@*4\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 4 } },
  { .label = "@*4\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 4 } },
  { .label = "@*4\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 4 } },
  { .label = "@*4\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 4 } },
  { .label = "@*4\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 4 } },
  { .label = "@*4\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 4 } },
  { .label = "@*4\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 4 } },
  { .label = "@*4\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 4 } },
  { .label = "@*4\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 4 } },
  { .label = "@*4\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 4 } },
  { .label = "@*4\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 4 } },
  { .label = "@*4\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 4 } },
  
  { .label = "@*5\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 5 } },
  { .label = "@*5\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 5 } },
  { .label = "@*5\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 5 } },
  { .label = "@*5\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 5 } },
  { .label = "@*5\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 5 } },
  { .label = "@*5\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 5 } },
  { .label = "@*5\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 5 } },
  { .label = "@*5\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 5 } },
  { .label = "@*5\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 5 } },
  { .label = "@*5\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 5 } },
  { .label = "@*5\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 5 } },
  { .label = "@*5\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 5 } },
  { .label = "@*5\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 5 } },
  { .label = "@*5\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 5 } },
  { .label = "@*5\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 5 } },
  
  { .label = "@*6\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 6 } },
  { .label = "@*6\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 6 } },
  { .label = "@*6\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 6 } },
  { .label = "@*6\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 6 } },
  { .label = "@*6\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 6 } },
  { .label = "@*6\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 6 } },
  { .label = "@*6\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 6 } },
  { .label = "@*6\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 6 } },
  { .label = "@*6\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 6 } },
  { .label = "@*6\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 6 } },
  { .label = "@*6\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 6 } },
  { .label = "@*6\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 6 } },
  { .label = "@*6\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 6 } },
  { .label = "@*6\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 6 } },
  { .label = "@*6\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 6 } },
  
  { .label = "All @s\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 7 } },
  { .label = "All @s\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 7 } },
  { .label = "All @s\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 7 } },
  { .label = "All @s\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 7 } },
  { .label = "All @s\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 7 } },
  { .label = "All @s\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 7 } },
  { .label = "All @s\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 7 } },
  { .label = "All @s\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 7 } },
  { .label = "All @s\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 7 } },
  { .label = "All @s\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 7 } },
  { .label = "All @s\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 7 } },
  { .label = "All @s\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 7 } },
  { .label = "All @s\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 7 } },
  { .label = "All @s\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 7 } },
  { .label = "All @s\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 7 } },

  { .label = "3\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 3 } },
  { .label = "4\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 4 } },
  { .label = "5\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 5 } },
  { .label = "6\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 6 } },
  { .label = "7\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 7 } },
  { .label = "8\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 8 } },
  { .label = "9\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 9 } },
  { .label = "10\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 10 } },
  { .label = "All 11\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 11 } },

  { .label = "@*20\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 20 } },
  { .label = "@*25\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 25 } },
  { .label = "@*30\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 30 } },
  { .label = "@*35\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 35 } },

  { .label = "$*200\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 200 } },
  { .label = "$*250\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 250 } },
  { .label = "$*300\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 300 } },
  { .label = "$*350\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 350 } },
  { .label = "$*400\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 400 } },
  { .label = "$*450\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 450 } },
  { .label = "$*500\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 500 } },
  { .label = "$*550\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 550 } },
  { .label = "$*600\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 600 } },

  { .label = "@*4\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 4 } },
  { .label = "@*5\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 5 } },
  { .label = "@*6\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 6 } },
  { .label = "@*7\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 7 } },
  { .label = "@*8\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 8 } },
  { .label = "@*9\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 9 } },
  { .label = "@*10\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 10 } },
  { .label = "@*11\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 11 } },
  { .label = "@*12\nBonus", .fn = bingo_have_N_bonus_stars, .args = { .count = 12 } },

  { .label = "$*50\nin BOB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BOB, .count = 50 } },
  { .label = "$*50\nin WF", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WF, .count = 50 } },
  { .label = "$*50\nin JRB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_JRB, .count = 50 } },
  { .label = "$*50\nin CCM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_CCM, .count = 50 } },
  { .label = "$*50\nin BBH", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BBH, .count = 50 } },
  { .label = "$*50\nin HMC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_HMC, .count = 50 } },
  { .label = "$*50\nin LLL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_LLL, .count = 50 } },
  { .label = "$*50\nin SSL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SSL, .count = 50 } },
  { .label = "$*50\nin DDD", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_DDD, .count = 50 } },
  { .label = "$*50\nin SL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SL, .count = 50 } },
  { .label = "$*50\nin WDW", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WDW, .count = 50 } },
  { .label = "$*50\nin TTM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTM, .count = 50 } },
  { .label = "$*50\nin THI", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_THI, .count = 50 } },
  { .label = "$*50\nin TTC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTC, .count = 50 } },
  { .label = "$*50\nin RR", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_RR, .count = 50 } },

  { .label = "$*50\nany 1", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 1 } },
  { .label = "$*50\nany 2", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 2 } },
  { .label = "$*50\nany 3", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 3 } },
  { .label = "$*50\nany 4", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 4 } },
  { .label = "$*50\nany 5", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 5 } },
  { .label = "$*50\nany 6", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 6 } },

  { .label = "$*100\nany 1", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 1 } },
  { .label = "$*100\nany 2", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 2 } },
  { .label = "$*100\nany 3", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 3 } },
  { .label = "$*100\nany 4", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 4 } },

  { .label = "@*1\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 6 } },
  { .label = "@*1\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 7 } },
  { .label = "@*1\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 8 } },
  { .label = "@*1\nany 9", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 9 } },
  { .label = "@*1\nany 10", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 10 } },
  { .label = "@*1\nany 11", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 11 } },
  { .label = "@*1\nany 12", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 12 } },
  { .label = "@*1\neach", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 15 } },

  { .label = "@*2\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 4 } },
  { .label = "@*2\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 5 } },
  { .label = "@*2\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 6 } },
  { .label = "@*2\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 7 } },
  { .label = "@*2\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 8 } },
  { .label = "@*2\nany 9", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 9 } },
  { .label = "@*2\nany 10", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 10 } },
  { .label = "@*2\neach", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 15 } },

  { .label = "@*3\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 2 } },
  { .label = "@*3\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 3 } },
  { .label = "@*3\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 4 } },
  { .label = "@*3\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 5 } },
  { .label = "@*3\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 6 } },
  { .label = "@*3\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 7 } },
  // { .label = "@*3\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 8 } },
  
  { .label = "@*4\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 1 } },
  { .label = "@*4\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 2 } },
  { .label = "@*4\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 3 } },
  { .label = "@*4\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 4 } },
  { .label = "@*4\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 5 } },
  // { .label = "@*4\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 6 } },

  { .label = "@*5\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 1 } },
  { .label = "@*5\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 2 } },
  { .label = "@*5\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 3 } },
  { .label = "@*5\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 4 } },
  
  { .label = "All @s\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 1 } },
  { .label = "All @s\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 2 } },
  { .label = "All @s\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 3 } },
  
  // { .label = "Reds\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 5 } },
  // { .label = "Reds\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 6 } },
  // { .label = "Reds\nany 7", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 7 } },
  // { .label = "Reds\nany 8", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 8 } },
  // { .label = "Reds\nany 9", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 9 } },
  // { .label = "Reds\nany 10", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 10 } },
  // { .label = "Reds\nany 11", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 11 } },
  // { .label = "Reds\nany 12", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 12 } },
  // { .label = "Reds\nany 13", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 13 } },
  // { .label = "Reds\nany 14", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 14 } },
  // { .label = "Reds\nany 15", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 15 } },
  
  // { .label = "Box @s\nany 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 3 } },
  // { .label = "Box @s\nany 4", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 4 } },
  // { .label = "Box @s\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 5 } },
  // { .label = "Box @s\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 6 } },
  
  // { .label = "Open @s\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 5 } },
  // { .label = "Open @s\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 6 } },
  // { .label = "Open @s\nany 7", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 7 } },
  // { .label = "Open @s\nany 8", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 8 } },
  // { .label = "Open @s\nany 9", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 9 } },
  // { .label = "Open @s\nany 10", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 10 } },
  // { .label = "Open @s\nany 11", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 11 } },
  // { .label = "Open @s\nany 12", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 12 } },
  // { .label = "Open @s\nany 13", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 13 } },
  // { .label = "Open @s\nany 14", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 14 } },
  // { .label = "Open @s\nany 15", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 15 } },

  // END
  { .label = NULL }
};

void bingo_debug_text(char* buffer, int maxlen) {
  // snprintf(buffer, maxlen, "%d %d",
  //   // bool_to_str(goals[0].fn(goals[0].args)),
  //   // bool_to_str(goals[1].fn(goals[1].args))
  //   bingo_get_star_count_from_set(slideStars),
  //   bingo_get_star_count_from_set(boxStars)
  // );
  snprintf(buffer, maxlen, "hello %s", "world");
}

struct BingoGoal* get_possible_bingo_goals(void) {
  return goals;
}
