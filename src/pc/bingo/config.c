#include "bingo.h"

// TODO actually save this in configfile.h / configfile.c
struct BingoConfig gBingoConfig = {
    .bossStars = TRUE,
    .boxStars = TRUE,
    .capStageStars = TRUE,
    .extraLives = TRUE,
    .freestandingStars = TRUE,
    .raceStars = TRUE,
    .redCoinStars = TRUE,
    .secretsStars = TRUE,
    .slideStars = TRUE,
};

struct BingoConfig bingo_config_from_string(const char *s) {
    struct BingoConfig config = { 0 };
    u32 flags = strtol(s, NULL, 16);
    if (flags & (1 << 0)) {
        config.bossStars = TRUE;
    }
    if (flags & (1 << 1)) {
        config.boxStars = TRUE;
    }
    if (flags & (1 << 2)) {
        config.capStageStars = TRUE;
    }
    if (flags & (1 << 3)) {
        config.freestandingStars = TRUE;
    }
    if (flags & (1 << 4)) {
        config.raceStars = TRUE;
    }
    if (flags & (1 << 5)) {
        config.redCoinStars = TRUE;
    }
    if (flags & (1 << 6)) {
        config.secretsStars = TRUE;
    }
    if (flags & (1 << 7)) {
        config.slideStars = TRUE;
    }
    if (flags & (1 << 8)) {
        config.extraLives = TRUE;
    }
    return config;
}

void bingo_config_to_string(struct BingoConfig config, char *buf, int maxlen) {
    u32 flags;
    if (config.bossStars) {
        flags |= 1 << 0;
    }
    if (config.boxStars) {
        flags |= 1 << 1;
    }
    if (config.capStageStars) {
        flags |= 1 << 2;
    }
    if (config.freestandingStars) {
        flags |= 1 << 3;
    }
    if (config.raceStars) {
        flags |= 1 << 4;
    }
    if (config.redCoinStars) {
        flags |= 1 << 5;
    }
    if (config.secretsStars) {
        flags |= 1 << 6;
    }
    if (config.slideStars) {
        flags |= 1 << 7;
    }
    snprintf(buf, maxlen, "%x", flags);
}
