#include "nouveau.h"

#include "pc/bingo/players.h"

#include <arpa/inet.h>
// #include <fcntl.h>
// #include <netdb.h>
#include "json/cJSON.h"

#include "receive.h"
#include "send.h"

bool attemptedConnection = FALSE;
int sock = -1;

void bingo_network_init(void) {
  init_bingo();
  gBingoPlayers[0].dialogName[0] = 0xFF;
}

void bingo_network_shutdown(void) {
  if (sock > 0) {
    close(sock);
    sock = -1;
  }
}

void bingo_network_update(void) {
  if (!attemptedConnection) {
    attemptedConnection = true;
    bingo_connect("127.0.0.1", 6000);
    if (sock > 0) {
      send_hello("Dummy");
    }
  }

  if (sock > 0) {
    receive_messages();
  } else {
    if (gBingoPlayers[0].dialogName[0] == 0xFF) {
      convert_chars_to_dialog("Player", gBingoPlayers[0].dialogName, BINGO_PLAYER_NAME_MAXLEN);
    }
    gBingoPlayers[0].connected = true;
    gBingoPlayers[0].color = bingoPlayerColors[0];
    gBingoPlayers[0].completion = get_current_player_bingo_completion();
    gBingoPlayers[0].location = get_current_player_location();
  }
}


void bingo_network_request_new_board(unsigned int seed) {
  char bingoConfig[32];
  bingo_config_to_string(gBingoConfig, bingoConfig, 32);
  send_board_request(bingoConfig);
}

// void resolve_addr(const char* hostname, const char* port, struct sockaddr_storage* outAddr) {
//   struct addrinfo hints = {};
//   hints.ai_family = AF_INET;
//   hints.ai_socktype = SOCK_DGRAM;

//   struct addrinfo* results = NULL;
//   if (getaddrinfo(hostname, port, &hints, &results) != 0) {
//     NOUVEAU_LOG("failed to resolve address");
//     return;
//   }
//   memcpy(outAddr, results->ai_addr, results->ai_addrlen);
//   freeaddrinfo(results);
// }

int ipv4_to_ipv6(const char *str, void *addr) {
  // TODO this is very dumb. surely it's possible to convert an ipv4->ipv6 address without building a string.
  char addrBuf[32];
  snprintf(addrBuf, 32, "::FFFF:%s", str);
  return inet_pton(AF_INET6, addrBuf, addr);
}

void bingo_connect(const char* ipv4, int port) {
  if (sock > 0) {
    bingo_network_shutdown();
  }
  reset_receive_buffers();
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].connected = false;
    gBingoPlayers[i].dialogName[0] = 0xFF;
  }

  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    NOUVEAU_ERR("create socket");
    return;
  }

  struct sockaddr_in6 server_addr = {};
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(port);
  ipv4_to_ipv6(ipv4, &server_addr.sin6_addr);
  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    NOUVEAU_ERR("connect");
    close(sock);
    sock = -1;
    return;
  }

  NOUVEAU_LOG("connected");
}

// void join_room(int32_t room_id) {
//   // TODO ???
//   for (int i = 1; i < BINGO_MAX_PLAYERS; i++) {
//     gBingoPlayers[i].connected = false;
//   }

//   if (sock > 0) {
//     close(sock);
//   }

//   // sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//   sock = socket(AF_INET, SOCK_DGRAM, 0);
//   if (sock < 0) {
//     NOUVEAU_LOG("failed to create socket");
//     return;
//   }

//   // TODO bind an address i guess?????????? (is this necessary?)
//   struct sockaddr_in listenAddr = {};
//   listenAddr.sin_family = AF_INET;
//   listenAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//   listenAddr.sin_port = htons(8000);
//   if (bind(sock, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) < 0) {
//     NOUVEAU_LOG("failed to bind (%d)", errno);
//   }
//   NOUVEAU_LOG("bound to port %d", listenAddr.sin_port);

//   // MEMO make it non-blocking
//   // MEMO can also use MSG_DONTWAIT flag
//   int flags = fcntl(sock, F_GETFL);
//   flags |= O_NONBLOCK;
//   fcntl(sock, F_SETFL, flags);

//   // char req[1024];
//   // snprintf(req, 1024, "{\"s\":\"sm64bingo\",\"r\":%d}\0", room_id);
//   char req[1024] = "hello world";


//   // // struct hostent* host = (struct hostent*) gethostbyname((char *) "127.0.0.1");
//   // struct hostent* host = (struct hostent*) gethostbyname((char *) "localhost");
//   // struct sockaddr_in server_addr;
//   // server_addr.sin_family = AF_INET;
//   // server_addr.sin_port = htons(6000);
//   // server_addr.sin_addr = *((struct in_addr*) host->h_addr);
//   // bzero(&(server_addr.sin_zero), 8);
//   // int sin_size = sizeof(struct sockaddr);
//   // if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {



//   // struct sockaddr_storage destAddr;
//   // resolve_addr("127.0.0.1", "6000", &destAddr);
//   // // MEMO i think this just makes us end up sending to the first resolved address
//   // if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&destAddr, sizeof(destAddr)) < 0) {




//   struct sockaddr_in notice_addr;
//   notice_addr.sin_family = AF_INET;
//   notice_addr.sin_port = htons(6000);
//   notice_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

//   if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&notice_addr, sizeof(notice_addr)) < 0) {
//     NOUVEAU_LOG("failed to send message (%d)", errno);
//   }

//   NOUVEAU_LOG("sent room request");
// }
