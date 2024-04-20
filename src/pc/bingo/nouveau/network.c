#include "nouveau.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>

#define NOUVEAU_LOG(...) printf("[BINGO/nouveau/log] ");printf(__VA_ARGS__);printf("\n")

struct sockaddr_in notice_addr;
int sock = -1;

void bingo_network_init(void) {
  NOUVEAU_LOG("build 20");
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
  if (sock > 0) {
    close(sock);
    sock = -1;
  }
}

void bingo_network_update(void) {
  // gBingoPlayers[0].completion = get_current_player_bingo_completion();
  // gBingoPlayers[0].location = get_current_player_location();

  if (sock > 0) {
    // char res[1024];
    // int server_struct_len = sizeof(notice_addr);
    // if (recvfrom(sock, res, sizeof(res), 0, (struct sockaddr*)&notice_addr, &server_struct_len) < 0) {
    //   if (errno == 11) {
    //     // NOUVEAU_LOG("no response yet (check again later)");
    //   } else {
    //     NOUVEAU_LOG("failed to receive message (%d)", errno);
    //   }
    // } else {
    //   NOUVEAU_LOG("got response");
    // }
  }
}


void bingo_network_request_new_board(unsigned int seed) {
  generate_bingo_board(seed, gBingoConfig);
}

void resolve_addr(const char* hostname, const char* port, struct sockaddr_storage* outAddr) {
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo* results = NULL;
  if (getaddrinfo(hostname, port, &hints, &results) != 0) {
    NOUVEAU_LOG("failed to resolve address");
    return;
  }
  memcpy(outAddr, results->ai_addr, results->ai_addrlen);
  freeaddrinfo(results);
}

void join_room(int32_t room_id) {
  // TODO ???
  for (int i = 1; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].connected = false;
  }

  if (sock > 0) {
    close(sock);
  }

  // sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    NOUVEAU_LOG("failed to create socket");
    return;
  }

  // TODO bind an address i guess?????????? (is this necessary?)
  struct sockaddr_in listenAddr = {};
  listenAddr.sin_family = AF_INET;
  if (bind(sock, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) < 0) {
    NOUVEAU_LOG("failed to bind (%d)", errno);
  }

  // // MEMO make it non-blocking
  // int flags = fcntl(sock, F_GETFL);
  // flags |= O_NONBLOCK;
  // fcntl(sock, F_SETFL, flags);

  // char req[1024];
  // snprintf(req, 1024, "{\"s\":\"sm64bingo\",\"r\":%d}\0", room_id);
  char req[1024] = "hello world";


  // // struct hostent* host = (struct hostent*) gethostbyname((char *) "127.0.0.1");
  // struct hostent* host = (struct hostent*) gethostbyname((char *) "localhost");
  // struct sockaddr_in server_addr;
  // server_addr.sin_family = AF_INET;
  // server_addr.sin_port = htons(6000);
  // server_addr.sin_addr = *((struct in_addr*) host->h_addr);
  // bzero(&(server_addr.sin_zero), 8);
  // int sin_size = sizeof(struct sockaddr);
  // if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {



  // struct sockaddr_storage destAddr;
  // resolve_addr("127.0.0.1", "6000", &destAddr);
  // // MEMO i think this just makes us end up sending to the first resolved address
  // if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&destAddr, sizeof(destAddr)) < 0) {




  notice_addr.sin_family = AF_INET;
  notice_addr.sin_port = htons(6000);
  notice_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&notice_addr, sizeof(notice_addr)) < 0) {
    NOUVEAU_LOG("failed to send message (%d)", errno);
  }

  NOUVEAU_LOG("sent room request");
}
