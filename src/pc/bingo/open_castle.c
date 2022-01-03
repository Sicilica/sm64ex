#include "game/save_file.h"

void open_castle_lobby_doors(void) {
  save_file_set_flags(SAVE_FLAG_UNLOCKED_PSS_DOOR | SAVE_FLAG_UNLOCKED_WF_DOOR | SAVE_FLAG_UNLOCKED_JRB_DOOR | SAVE_FLAG_UNLOCKED_CCM_DOOR);
}

void open_castle_bowser_doors(void) {
  save_file_set_flags(SAVE_FLAG_UNLOCKED_BITDW_DOOR | SAVE_FLAG_UNLOCKED_BITFS_DOOR | SAVE_FLAG_DDD_MOVED_BACK | SAVE_FLAG_UNLOCKED_50_STAR_DOOR);
}

void open_castle_key_doors(void) {
  save_file_set_flags(SAVE_FLAG_UNLOCKED_BASEMENT_DOOR | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
  save_file_clear_flags(SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_HAVE_KEY_2);
}

void open_castle_for_bingo(void) {
  open_castle_lobby_doors();
  open_castle_bowser_doors();
  open_castle_key_doors();
}
