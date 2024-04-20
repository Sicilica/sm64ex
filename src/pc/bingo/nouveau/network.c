#include "nouveau.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

#include <sys/socket.h>

void bingo_network_init(void) {
  init_bingo();

  gBingoPlayers[0].connected = true;
  gBingoPlayers[0].color = bingoPlayerColors[0];
  join_room(0);
  // convert_chars_to_dialog("Player", gBingoPlayers[0].dialogName, BINGO_PLAYER_NAME_MAXLEN);

  // // TMP DEBUG STUFF
  // for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
  //   gBingoPlayers[i].connected = true;
  //   gBingoPlayers[i].color = bingoPlayerColors[i];
  //   convert_chars_to_dialog("Player", gBingoPlayers[i].dialogName, BINGO_PLAYER_NAME_MAXLEN);
  //   gBingoPlayers[i].completion = (u32)i | 0x8000;
  // }
}

void bingo_network_shutdown(void) {
  // noop
}

void bingo_network_update(void) {
  // gBingoPlayers[0].completion = get_current_player_bingo_completion();
  // gBingoPlayers[0].location = get_current_player_location();
}


void bingo_network_request_new_board(unsigned int seed) {
  generate_bingo_board(seed, gBingoConfig);
}

void join_room(int32_t room_id) {
  for (int i = 1; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].connected = false;
  }

  int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sd < 0) {
    // TODO failed to create socket
  }
  // MEMO make it non-blocking
  int flags = fcntl(sd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sd, F_SETFL, flags);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(6000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  char req[1024] = "{\"s\":\"sm64bingo\"}";
  int server_struct_len = sizeof(server_addr);
  if (sendto(sd, req, strlen(req), 0, (struct sockaddr*)&server_addr, server_struct_len) < 0) {
    // TODO failed to send message
  }

  char res[1024];
  if (recvfrom(sd, res, sizeof(res), 0, (struct sockaddr*)&server_addr, &server_struct_len) < 0) {
    // TODO failed to receive message
  }

  close(sd);
}
