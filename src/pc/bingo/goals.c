#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sm64.h"
#include "game/level_update.h"

#include "bingo.h"
#include "save.h"

#define TOTAL_CANNONS 11
#define TOTAL_CASTLE_STARS 15

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

bool bingo_have_N_castle_stars(struct BingoGoalArgs args) {
  s32 actual = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_BONUS_STAGES - 1, COURSE_MAX - 1);
  return actual >= args.count;
}

bool bingo_have_at_least_N_lives(struct BingoGoalArgs args) {
  s8 actual = gMarioState->numLives;
  return actual >= args.count;
}

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

typedef bool (*UseGoalGroupFn)(struct BingoConfig);

struct GoalGroup {
  UseGoalGroupFn fn;
  struct BingoGoal* goals;
};

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

s16 starset_size(struct BingoStarRef* starSet) {
  for (s16 size = 0; ; size++) {
    if (starSet[size].course == COURSE_NONE) {
      return size;
    }
  }
}

#define DEF_STARSET_METRIC(STARSET, LABEL) \
  bool goal_is_starset_ ## STARSET (struct BingoGoal* goal) { return goal->fn == bingo_have_N_stars_from_set && goal->args.starSet == STARSET; } \
  s16 starset_count_ ## STARSET (void) { return bingo_get_star_count_from_set(STARSET); } \
  s16 starset_max_ ## STARSET (void) { return starset_size(STARSET); } \
  struct BingoGoalMetric starset ## STARSET ## Metric = { .label = LABEL " @s", .includesGoal = goal_is_starset_ ## STARSET, .currentCount = starset_count_ ## STARSET, .calcMaximum = starset_max_ ## STARSET };

#define STARSET_METRIC(STARSET) &starset ## STARSET ## Metric

DEF_STARSET_METRIC(bossStars, "Boss")
DEF_STARSET_METRIC(boxStars, "Box")
DEF_STARSET_METRIC(freestandingStars, "Open")
DEF_STARSET_METRIC(redCoinStars, "Reds")
DEF_STARSET_METRIC(raceStars, "Race")
DEF_STARSET_METRIC(secretsStars, "Secrets")
DEF_STARSET_METRIC(slideStars, "Slide")

s16 total_cannons(void) {
  return TOTAL_CANNONS;
}

s16 cannons_count(void) {
  return save_file_get_total_cannon_count(gCurrSaveFileNum - 1);
}

bool goal_is_castle_stars(struct BingoGoal* goal) {
  return goal->fn == bingo_have_N_castle_stars;
}

s16 total_castle_stars(void) {
  return TOTAL_CASTLE_STARS;
}

s16 castle_stars_count(void) {
  return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_BONUS_STAGES - 1, COURSE_MAX - 1);
}

bool goal_is_cannons(struct BingoGoal* goal) {
  return goal->fn == bingo_open_N_cannons;
}

s16 total_coins(void) {
  return save_file_get_total_coin_count(gCurrSaveFileNum - 1);
}

bool goal_is_total_coins(struct BingoGoal* goal) {
  return goal->fn == bingo_have_N_total_coins;
}

struct BingoGoalMetric cannonsMetric = {
  .label = "Cannons",
  .includesGoal = goal_is_cannons,
  .currentCount = cannons_count,
  .calcMaximum = total_cannons,
};

struct BingoGoalMetric castleStarsMetric = {
  .label = "Castle @s",
  .includesGoal = goal_is_castle_stars,
  .currentCount = castle_stars_count,
  .calcMaximum = total_castle_stars,
};

struct BingoGoalMetric totalCoinsMetric = {
  .label = "Total Coins",
  .includesGoal = goal_is_total_coins,
  .currentCount = total_coins,
  .calcMaximum = NULL,
};

struct BingoGoalMetric* gBingoMetrics[] = {
  &cannonsMetric,
  &totalCoinsMetric,
  STARSET_METRIC(slideStars),
  STARSET_METRIC(secretsStars),
  STARSET_METRIC(redCoinStars),
  STARSET_METRIC(raceStars),
  STARSET_METRIC(freestandingStars),
  // NOTE Disabling castle stars because it doesn't quite fit on steam deck and
  // isn't really that useful anyway
  // &castleStarsMetric,
  STARSET_METRIC(boxStars),
  STARSET_METRIC(bossStars),
  NULL,
};

#define DEF_GOAL_GROUP(NAME, USE_FN, GOAL_LIST) \
  bool group_ ## NAME ## _use(struct BingoConfig config) USE_FN \
  struct BingoGoal group_ ## NAME ## _goals[] = { GOAL_LIST DEF_GOAL( .label = NULL ) };

#define DEF_GOAL(...) { __VA_ARGS__ },
#define GOAL_GROUP_REF(NAME) { .fn = group_ ## NAME ## _use, .goals = group_ ## NAME ## _goals },

DEF_GOAL_GROUP(BOSS_STARS, {
  return config.bossStars;
},
  DEF_GOAL( .label = "Bosses\nany 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = bossStars, .count = 3 } )
  DEF_GOAL( .label = "Bosses\nall 4", .fn = bingo_have_N_stars_from_set, .args = { .starSet = bossStars, .count = 4 } )
)

DEF_GOAL_GROUP(BOX_STARS, {
  return config.boxStars;
},
  DEF_GOAL( .label = "Box @s\nany 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 3 } )
  DEF_GOAL( .label = "Box @s\nany 4", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 4 } )
  DEF_GOAL( .label = "Box @s\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 5 } )
  DEF_GOAL( .label = "Box @s\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = boxStars, .count = 6 } )
)

DEF_GOAL_GROUP(CANNONS, {
  return true;
},
  DEF_GOAL( .label = "3\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 3 } )
  DEF_GOAL( .label = "4\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 4 } )
  DEF_GOAL( .label = "5\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 5 } )
  DEF_GOAL( .label = "6\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 6 } )
  DEF_GOAL( .label = "7\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 7 } )
  DEF_GOAL( .label = "8\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 8 } )
  DEF_GOAL( .label = "9\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 9 } )
  DEF_GOAL( .label = "10\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 10 } )
  DEF_GOAL( .label = "All 11\nCannons", .fn = bingo_open_N_cannons, .args = { .count = 11 } )
)

DEF_GOAL_GROUP(CAP_STAGE_STARS, {
  return config.capStageStars;
},
  DEF_GOAL( .label = "Wing\nCap", .fn = bingo_have_specific_star, .args = { .course = COURSE_TOTWC, .star = 1 })
  DEF_GOAL( .label = "Metal\nCap", .fn = bingo_have_specific_star, .args = { .course = COURSE_COTMC, .star = 1 } )
  DEF_GOAL( .label = "Vanish\nCap", .fn = bingo_have_specific_star, .args = { .course = COURSE_VCUTM, .star = 1 } )
)

DEF_GOAL_GROUP(CASTLE_STARS, {
  return true;
},
  DEF_GOAL( .label = "@*4\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 4 } )
  DEF_GOAL( .label = "@*5\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 5 } )
  DEF_GOAL( .label = "@*6\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 6 } )
  DEF_GOAL( .label = "@*7\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 7 } )
  DEF_GOAL( .label = "@*8\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 8 } )
  DEF_GOAL( .label = "@*9\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 9 } )
  DEF_GOAL( .label = "@*10\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 10 } )
  DEF_GOAL( .label = "@*11\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 11 } )
  DEF_GOAL( .label = "@*12\nCastle", .fn = bingo_have_N_castle_stars, .args = { .count = 12 } )
)

DEF_GOAL_GROUP(FREESTANDING_STARS, {
  return config.freestandingStars;
},
  DEF_GOAL( .label = "Open @s\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 5 } )
  DEF_GOAL( .label = "Open @s\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 6 } )
  DEF_GOAL( .label = "Open @s\nany 7", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 7 } )
  DEF_GOAL( .label = "Open @s\nany 8", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 8 } )
  DEF_GOAL( .label = "Open @s\nany 9", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 9 } )
  DEF_GOAL( .label = "Open @s\nany 10", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 10 } )
  DEF_GOAL( .label = "Open @s\nany 11", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 11 } )
  DEF_GOAL( .label = "Open @s\nany 12", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 12 } )
  DEF_GOAL( .label = "Open @s\nany 13", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 13 } )
  DEF_GOAL( .label = "Open @s\nany 14", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 14 } )
  DEF_GOAL( .label = "Open @s\nany 15", .fn = bingo_have_N_stars_from_set, .args = { .starSet = freestandingStars, .count = 15 } )
)

DEF_GOAL_GROUP(EXTRA_LIVES, {
  return config.extraLives;
},
  // DEF_GOAL( .label = "10\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 10 } )
  // DEF_GOAL( .label = "11\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 11 } )
  DEF_GOAL( .label = "12\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 12 } )
  // DEF_GOAL( .label = "13\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 13 } )
  DEF_GOAL( .label = "14\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 14 } )
  // DEF_GOAL( .label = "15\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 15 } )
  DEF_GOAL( .label = "16\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 16 } )
  // DEF_GOAL( .label = "17\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 17 } )
  DEF_GOAL( .label = "18\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 18 } )
  // DEF_GOAL( .label = "19\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 19 } )
  DEF_GOAL( .label = "20\nLives", .fn = bingo_have_at_least_N_lives, .args = { .count = 20 } )
)

DEF_GOAL_GROUP(MISC, {
  return true;
},
  DEF_GOAL( .label = "Lose\nCap", .fn = bingo_lose_marios_cap )
  DEF_GOAL( .label = "Zero\nLives", .fn = bingo_have_no_lives )
  DEF_GOAL( .label = "Toad 2\n(@*25)", .fn = bingo_have_toad_star, .args = { .star = 2 } )
  DEF_GOAL( .label = "Toad 3\n(@*35)", .fn = bingo_have_toad_star, .args = { .star = 3 } )
  // DEF_GOAL( .label = "MIPS 2\n(@*50)", .fn = bingo_have_mips_star, .args = { .star = 2 } )
)

DEF_GOAL_GROUP(PICK_STAGE_COINS, {
  return true;
},
  DEF_GOAL( .label = "$*50\nany 1", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 1 } )
  DEF_GOAL( .label = "$*50\nany 2", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 2 } )
  DEF_GOAL( .label = "$*50\nany 3", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 3 } )
  DEF_GOAL( .label = "$*50\nany 4", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 4 } )
  DEF_GOAL( .label = "$*50\nany 5", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 5 } )
  DEF_GOAL( .label = "$*50\nany 6", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 50, .count2 = 6 } )

  DEF_GOAL( .label = "$*100\nany 1", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 1 } )
  DEF_GOAL( .label = "$*100\nany 2", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 2 } )
  DEF_GOAL( .label = "$*100\nany 3", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 3 } )
  DEF_GOAL( .label = "$*100\nany 4", .fn = bingo_have_N_coins_in_M_stages, .args = { .count = 100, .count2 = 4 } )
)

DEF_GOAL_GROUP(PICK_STAGE_STARS, {
  return true;
},
  DEF_GOAL( .label = "@*1\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 6 } )
  DEF_GOAL( .label = "@*1\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 7 } )
  DEF_GOAL( .label = "@*1\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 8 } )
  DEF_GOAL( .label = "@*1\nany 9", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 9 } )
  DEF_GOAL( .label = "@*1\nany 10", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 10 } )
  DEF_GOAL( .label = "@*1\nany 11", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 11 } )
  DEF_GOAL( .label = "@*1\nany 12", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 12 } )
  DEF_GOAL( .label = "@*1\neach", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 1, .count2 = 15 } )

  DEF_GOAL( .label = "@*2\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 4 } )
  DEF_GOAL( .label = "@*2\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 5 } )
  DEF_GOAL( .label = "@*2\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 6 } )
  DEF_GOAL( .label = "@*2\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 7 } )
  DEF_GOAL( .label = "@*2\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 8 } )
  DEF_GOAL( .label = "@*2\nany 9", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 9 } )
  DEF_GOAL( .label = "@*2\nany 10", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 10 } )
  DEF_GOAL( .label = "@*2\neach", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 2, .count2 = 15 } )

  DEF_GOAL( .label = "@*3\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 2 } )
  DEF_GOAL( .label = "@*3\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 3 } )
  DEF_GOAL( .label = "@*3\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 4 } )
  DEF_GOAL( .label = "@*3\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 5 } )
  DEF_GOAL( .label = "@*3\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 6 } )
  DEF_GOAL( .label = "@*3\nany 7", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 7 } )
  // DEF_GOAL( .label = "@*3\nany 8", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 3, .count2 = 8 } )
  
  DEF_GOAL( .label = "@*4\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 1 } )
  DEF_GOAL( .label = "@*4\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 2 } )
  DEF_GOAL( .label = "@*4\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 3 } )
  DEF_GOAL( .label = "@*4\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 4 } )
  DEF_GOAL( .label = "@*4\nany 5", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 5 } )
  // DEF_GOAL( .label = "@*4\nany 6", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 4, .count2 = 6 } )

  DEF_GOAL( .label = "@*5\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 1 } )
  DEF_GOAL( .label = "@*5\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 2 } )
  DEF_GOAL( .label = "@*5\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 3 } )
  DEF_GOAL( .label = "@*5\nany 4", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 5, .count2 = 4 } )
  
  DEF_GOAL( .label = "All @s\nany 1", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 1 } )
  DEF_GOAL( .label = "All @s\nany 2", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 2 } )
  DEF_GOAL( .label = "All @s\nany 3", .fn = bingo_have_N_stars_in_M_stages, .args = { .count = 7, .count2 = 3 } )
)

DEF_GOAL_GROUP(RACE_STARS, {
  return config.raceStars;
},
  DEF_GOAL( .label = "Races\nany 2", .fn = bingo_have_N_stars_from_set, .args = { .starSet = raceStars, .count = 2 } )
  DEF_GOAL( .label = "Races\nall 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = raceStars, .count = 3 } )
)

DEF_GOAL_GROUP(RED_COIN_STARS, {
  return config.redCoinStars;
},
  DEF_GOAL( .label = "Reds\nany 5", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 5 } )
  DEF_GOAL( .label = "Reds\nany 6", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 6 } )
  DEF_GOAL( .label = "Reds\nany 7", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 7 } )
  DEF_GOAL( .label = "Reds\nany 8", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 8 } )
  DEF_GOAL( .label = "Reds\nany 9", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 9 } )
  DEF_GOAL( .label = "Reds\nany 10", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 10 } )
  DEF_GOAL( .label = "Reds\nany 11", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 11 } )
  DEF_GOAL( .label = "Reds\nany 12", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 12 } )
  DEF_GOAL( .label = "Reds\nany 13", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 13 } )
  DEF_GOAL( .label = "Reds\nany 14", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 14 } )
  DEF_GOAL( .label = "Reds\nany 15", .fn = bingo_have_N_stars_from_set, .args = { .starSet = redCoinStars, .count = 15 } )
)

DEF_GOAL_GROUP(SECRETS_STARS, {
  return config.secretsStars;
},
  DEF_GOAL( .label = "Secrets\nany 2", .fn = bingo_have_N_stars_from_set, .args = { .starSet = secretsStars, .count = 2 } )
  DEF_GOAL( .label = "Secrets\nany 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = secretsStars, .count = 3 } )
)

DEF_GOAL_GROUP(SPECIFIC_STAGE_COINS, {
  return true; 
},
  DEF_GOAL( .label = "$*50\nin BOB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BOB, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin WF", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WF, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin JRB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_JRB, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin CCM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_CCM, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin BBH", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BBH, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin HMC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_HMC, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin LLL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_LLL, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin SSL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SSL, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin DDD", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_DDD, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin SL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SL, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin WDW", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WDW, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin TTM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTM, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin THI", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_THI, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin TTC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTC, .count = 50 } )
  DEF_GOAL( .label = "$*50\nin RR", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_RR, .count = 50 } )

  DEF_GOAL( .label = "$*100\nin BOB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BOB, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin WF", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WF, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin JRB", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_JRB, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin CCM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_CCM, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin BBH", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_BBH, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin HMC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_HMC, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin LLL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_LLL, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin SSL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SSL, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin DDD", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_DDD, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin SL", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_SL, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin WDW", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_WDW, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin TTM", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTM, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin THI", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_THI, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin TTC", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_TTC, .count = 100 } )
  DEF_GOAL( .label = "$*100\nin RR", .fn = bingo_have_N_coins_in_stage_X, .args = { .course = COURSE_RR, .count = 100 } )
)

DEF_GOAL_GROUP(SPECIFIC_STAGE_STARS, {
  return true;
},
  DEF_GOAL( .label = "@*3\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 3 } )
  DEF_GOAL( .label = "@*3\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 3 } )
  
  DEF_GOAL( .label = "@*4\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 4 } )
  DEF_GOAL( .label = "@*4\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 4 } )
  
  DEF_GOAL( .label = "@*5\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 5 } )
  DEF_GOAL( .label = "@*5\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 5 } )
  
  DEF_GOAL( .label = "@*6\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 6 } )
  DEF_GOAL( .label = "@*6\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 6 } )
  
  DEF_GOAL( .label = "All @s\nin BOB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BOB, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin WF", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WF, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin JRB", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_JRB, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin CCM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_CCM, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin BBH", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_BBH, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin HMC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_HMC, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin LLL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_LLL, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin SSL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SSL, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin DDD", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_DDD, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin SL", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_SL, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin WDW", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_WDW, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin TTM", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTM, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin THI", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_THI, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin TTC", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_TTC, .count = 7 } )
  DEF_GOAL( .label = "All @s\nin RR", .fn = bingo_have_N_stars_in_stage_X, .args = { .course = COURSE_RR, .count = 7 } )
)

DEF_GOAL_GROUP(SLIDE_STARS, {
  return config.slideStars;
},
  DEF_GOAL( .label = "Slides\nany 3", .fn = bingo_have_N_stars_from_set, .args = { .starSet = slideStars, .count = 3 } )
  DEF_GOAL( .label = "Slides\nall 4", .fn = bingo_have_N_stars_from_set, .args = { .starSet = slideStars, .count = 4 } )
)

DEF_GOAL_GROUP(TOTAL_COINS, {
  return true;
},
  DEF_GOAL( .label = "$*200\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 200 } )
  DEF_GOAL( .label = "$*250\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 250 } )
  DEF_GOAL( .label = "$*300\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 300 } )
  DEF_GOAL( .label = "$*350\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 350 } )
  DEF_GOAL( .label = "$*400\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 400 } )
  DEF_GOAL( .label = "$*450\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 450 } )
  DEF_GOAL( .label = "$*500\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 500 } )
  DEF_GOAL( .label = "$*550\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 550 } )
  DEF_GOAL( .label = "$*600\nTotal", .fn = bingo_have_N_total_coins, .args = { .count = 600 } )
)

DEF_GOAL_GROUP(TOTAL_STARS, {
  return true;
},
  DEF_GOAL( .label = "@*20\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 20 } )
  DEF_GOAL( .label = "@*25\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 25 } )
  DEF_GOAL( .label = "@*30\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 30 } )
  DEF_GOAL( .label = "@*35\nTotal", .fn = bingo_have_N_total_stars, .args = { .count = 35 } )
)

struct GoalGroup goalGroups[] = {
  GOAL_GROUP_REF(BOSS_STARS)
  GOAL_GROUP_REF(BOX_STARS)
  GOAL_GROUP_REF(CANNONS)
  GOAL_GROUP_REF(CAP_STAGE_STARS)
  GOAL_GROUP_REF(CASTLE_STARS)
  GOAL_GROUP_REF(FREESTANDING_STARS)
  GOAL_GROUP_REF(EXTRA_LIVES)
  GOAL_GROUP_REF(MISC)
  GOAL_GROUP_REF(PICK_STAGE_COINS)
  GOAL_GROUP_REF(PICK_STAGE_STARS)
  GOAL_GROUP_REF(RACE_STARS)
  GOAL_GROUP_REF(RED_COIN_STARS)
  GOAL_GROUP_REF(SECRETS_STARS)
  GOAL_GROUP_REF(SPECIFIC_STAGE_COINS)
  GOAL_GROUP_REF(SPECIFIC_STAGE_STARS)
  GOAL_GROUP_REF(SLIDE_STARS)
  GOAL_GROUP_REF(TOTAL_COINS)
  GOAL_GROUP_REF(TOTAL_STARS)
  { .fn = NULL },
};

#define BINGO_FLAG_PLAYER_STATE (1 << 0)
#define BINGO_FLAG_TOTAL_STARS (1 << 1)
#define BINGO_FLAG_LIVES (1 << 2) // technically this is already covered by PLAYER_STATE
// #define BINGO_FLAG_PICK_STAGES (1 << 3)

// Stars 17-24 are worth 1 difficulty each. Stars 25-34 are worth 2 each. Stars 35+ are worth 3 each.
#define DIFFICULTY_FROM_TOTAL_STARS(x) (((x) > 16 ? (x) - 16 : 0) + ((x) > 25 ? (x) - 25 : 0) + ((x) > 35 ? (x) - 35 : 0))
#define DIFFICULTY_FROM_POOL_STARS(n, pool_size) 1
#define DIFFICULTY_FROM_UNIQUE_STAGES(x) 1
#define DIFFICULTY_FROM_TOTAL_COINS(x) 1

void calculate_difficulty_and_flags_for_goal(struct BingoGoal* goal) {
  // TODO
  goal->difficulty = 1;

  if (goal->fn == bingo_have_N_total_stars) {
    goal->difficulty = DIFFICULTY_FROM_TOTAL_STARS(goal->args.count);
    goal->flags = BINGO_FLAG_TOTAL_STARS;

  } else if (goal->fn == bingo_open_N_cannons) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_POOL_STARS(goal->args.count, TOTAL_CANNONS);

  } else if (goal->fn == bingo_have_N_castle_stars) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_POOL_STARS(goal->args.count, TOTAL_CASTLE_STARS);

  } else if (goal->fn == bingo_have_at_least_N_lives) {
    // TODO
    goal->difficulty = 1;
    goal->flags = BINGO_FLAG_PLAYER_STATE;
    goal->flags |= BINGO_FLAG_LIVES;

  } else if (goal->fn == bingo_have_no_lives) {
    // TODO
    goal->difficulty = 1;
    goal->flags = BINGO_FLAG_PLAYER_STATE;
    goal->flags |= BINGO_FLAG_LIVES;

  } else if (goal->fn == bingo_lose_marios_cap) {
    // TODO
    goal->difficulty = 1;
    goal->flags = BINGO_FLAG_PLAYER_STATE;

  } else if (goal->fn == bingo_have_specific_star) {
    // TODO
    goal->difficulty = 1;
    goal->course = goal->args.course;

  } else if (goal->fn == bingo_have_N_stars_from_set) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_POOL_STARS(1, 1);  // TODO count

  } else if (goal->fn == bingo_have_N_stars_in_stage_X) {
    // TODO
    goal->difficulty = 1; // TODO
    goal->course = goal->args.course;

  } else if (goal->fn == bingo_have_N_coins_in_stage_X) {
    // TODO
    goal->difficulty = 1; // TODO
    goal->course = goal->args.course;

  } else if (goal->fn == bingo_have_N_total_coins) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_TOTAL_COINS(goal->args.count);

  } else if (goal->fn == bingo_have_N_coins_in_M_stages) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_TOTAL_COINS(goal->args.count * goal->args.count2) + DIFFICULTY_FROM_UNIQUE_STAGES(goal->args.count2);
    // goal->flags = BINGO_FLAG_PICK_STAGES;

  } else if (goal->fn == bingo_have_N_stars_in_M_stages) {
    // TODO
    goal->difficulty = DIFFICULTY_FROM_TOTAL_STARS(goal->args.count * goal->args.count2) + DIFFICULTY_FROM_UNIQUE_STAGES(goal->args.count2);
    // goal->flags = BINGO_FLAG_PICK_STAGES;

  } else if (goal->fn == bingo_have_toad_star) {
    // 12/25/35
    goal->difficulty = 1 + DIFFICULTY_FROM_TOTAL_STARS(goal->args.star * 10 + 5 - (goal->args.star == 1 ? 3 : 0));
    goal->flags = BINGO_FLAG_TOTAL_STARS;

  } else if (goal->fn == bingo_have_mips_star) {  // unused
    // 15/50
    goal->difficulty = 2 + DIFFICULTY_FROM_TOTAL_STARS(goal->args.star * 35 - 20);
    goal->flags = BINGO_FLAG_TOTAL_STARS;

  } else {
    goal->difficulty = -1;
  }
}

bool can_goals_appear_in_a_row(struct BingoGoal* a, struct BingoGoal* b) {
  // Goals can't have the same function, since then they would be the exact same type
  if (a->fn == b->fn) {
    return false;
  }

  // If the goals are course-specific, they cannot be for the same course
  if (a->course != 0 && a->course == b->course) {
    return false;
  }

  // Goals can't have any other flags in common
  if (a->flags & b->flags) {
    return false;
  }

  return true;
}

struct BingoGoal** get_possible_bingo_goals(struct BingoConfig config) {
  int count = 0;
  for (struct GoalGroup* group = goalGroups; group->fn != NULL; group++) {
    if (group->fn(config) != 0) {
      for (struct BingoGoal* goal = group->goals; goal->label != NULL; goal++) {
        count++;
      }
    } else {
    }
  }

  struct BingoGoal** out = malloc((count + 1) * sizeof(struct BingoGoal*));
  int index = 0;
  for (struct GoalGroup* group = goalGroups; group->fn != NULL; group++) {
    if (group->fn(config) != 0) {
      for (struct BingoGoal* goal = group->goals; goal->label != NULL; goal++) {
        calculate_difficulty_and_flags_for_goal(goal);
        out[index++] = goal;
      }
    }
  }
  BINGO_LOG("finished choosing goals; i=%u, c=%u", index, count);
  out[count] = NULL;
  return out;
}
