#ifndef BINGO_H
#define BINGO_H

#include <stdbool.h>
#include <stdio.h>

#include <PR/ultratypes.h>
#include "game/save_file.h"

#define BINGO_VERSION "ALPHA 55"
#define BINGO_MAX_PLAYERS 8

#define BINGO_LOG(...)                                                                                 \
    printf("[BINGO/log] ");                                                                            \
    printf(__VA_ARGS__);                                                                               \
    printf("\n")
#define BINGO_MSG(...)                                                                                 \
    snprintf(bingoMsgBuffer, 256, __VA_ARGS__);                                                        \
    bingoMsgTimer = 30 * 5;

extern bool displayFullsizeBingoBoard;
extern bool bingoInitComplete;

extern char bingoMsgBuffer[];
extern int bingoMsgTimer;

void init_bingo(void);
void handle_bingo_input(void);
void render_bingo_board(void);

struct BingoConfig {
    bool bossStars;
    bool boxStars;
    bool capStageStars;
    bool extraLives;
    bool freestandingStars;
    bool raceStars;
    bool redCoinStars;
    bool secretsStars;
    bool slideStars;
};

struct BingoStarRef {
    enum CourseNum course;
    s32 star;
};

struct BingoGoalArgs {
    s32 count;
    s32 count2;
    enum CourseNum course;
    s32 star;
    const struct BingoStarRef *starSet;
};

typedef bool (*BingoGoalFn)(struct BingoGoalArgs);

struct BingoGoal {
    const char *label;
    BingoGoalFn fn;
    struct BingoGoalArgs args;

    // These 3 fields are used for building a valid board.
    enum CourseNum course;
    s32 difficulty;
    s32 flags;
};

bool can_goals_appear_in_a_row(struct BingoGoal *a, struct BingoGoal *b);
struct BingoGoal **get_possible_bingo_goals(struct BingoConfig config);

struct BingoGoalMetric {
    bool active;
    const char *label;
    u8 *mappedLabel;
    s16 labelWidth;
    bool (*includesGoal)(struct BingoGoal *);
    s16 (*currentCount)(void);
    s16 (*calcMaximum)(void);
    s16 *maximum;
};

struct LabelLine {
    u8 *dialog;
    s16 width;
};

struct BingoBoardCell {
    struct BingoGoal *goal;
    struct LabelLine labelLine1;
    struct LabelLine labelLine2;
};

extern struct BingoBoardCell gBingoBoard[];
extern unsigned int gBingoBoardSeed;
extern struct BingoConfig gBingoConfig;
extern struct BingoGoalMetric *gBingoMetrics[];

void clear_bingo_board(void);
void generate_bingo_board(unsigned int seed, struct BingoConfig config);

void bingo_request_new_board(void);

struct BingoConfig bingo_config_from_string(const char *s);
void bingo_config_to_string(struct BingoConfig config, char *buf, int maxlen);

void bingo_set_player_on_title(bool state);

#endif
