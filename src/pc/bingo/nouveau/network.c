#include "nouveau.h"

#include "pc/bingo/players.h"

#include <arpa/inet.h>
#include <netdb.h>
#include "json/cJSON.h"

#include "config.h"
#include "receive.h"
#include "send.h"

bool attemptedConnection = FALSE;
int sock = -1;

BingoCompletion last_sent_completion;
BingoPlayerLocation last_sent_location;
void send_state_if_outdated();

void bingo_network_init(void) {
  init_bingo();
  load_bingo_config();
  gBingoPlayers[0].dialogName[0] = 0xFF;
}

void bingo_network_shutdown(void) {
  if (sock > 0) {
    close(sock);
    sock = -1;
  }
  reset_receive_buffers();
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].connected = false;
    gBingoPlayers[i].dialogName[0] = 0xFF;
  }
  last_sent_completion = 0;
  last_sent_location = -1;
}

void bingo_network_update(void) {
  if (!attemptedConnection) {
    attemptedConnection = true;
    bingo_connect(host, port);
    if (sock > 0) {
      send_hello(username, preferred_color);
    }
  }

  if (sock > 0) {
    send_state_if_outdated();
    send_keepalive();
    receive_messages();
  } else {
    if (gBingoPlayers[0].dialogName[0] == 0xFF) {
      convert_chars_to_dialog("OFFLINE", gBingoPlayers[0].dialogName, BINGO_PLAYER_NAME_MAXLEN);
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

int ipv4_to_ipv6_slow(const char *str, struct in6_addr *addr) {
  char addrBuf[24];
  snprintf(addrBuf, 24, "::FFFF:%s", str);
  return inet_pton(AF_INET6, addrBuf, addr);
}

int ipv4_to_ipv6_fast(in_addr_t ipv4, struct in6_addr *addr) {
  memset(addr->__in6_u.__u6_addr8, 0x0, 16);
  addr->__in6_u.__u6_addr8[10] = 0xFF;
  addr->__in6_u.__u6_addr8[11] = 0xFF;
  addr->__in6_u.__u6_addr8[12] = (ipv4 >> 0) & 0xFF;
  addr->__in6_u.__u6_addr8[13] = (ipv4 >> 8) & 0xFF;
  addr->__in6_u.__u6_addr8[14] = (ipv4 >> 16) & 0xFF;
  addr->__in6_u.__u6_addr8[15] = (ipv4 >> 24) & 0xFF;
  return 1;
}

int resolve_addr(const char* host, int port, struct sockaddr_storage* outAddr) {
  // First, attempt ipv6
  struct sockaddr_in6 *ipv6 = outAddr;
  if (inet_pton(AF_INET6, host, &ipv6->sin6_addr) == 1) {
    NOUVEAU_LOG("resolved %s as ipv6", host);
    ipv6->sin6_family = AF_INET6;
    ipv6->sin6_port = htons(port);
    return 0;
  }

  // Next, attempt ipv4
  in_addr_t ipv4 = inet_addr(host);
  if (ipv4 != -1) {
    NOUVEAU_LOG("resolved %s as ipv4", host);
    ipv6->sin6_family = AF_INET6;
    ipv6->sin6_port = htons(port);
    if (ipv4_to_ipv6_fast(ipv4, &ipv6->sin6_addr) != 1) {
      NOUVEAU_LOG("failed to convert the ipv4 to ipv6");
      return -1;
    }
    return 0;
  }

  // Finally, use getaddrinfo
  char port_str[16];
  snprintf(port_str, 16, "%d", port);

  struct addrinfo hints = {};
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo* results = NULL;
  int res = getaddrinfo(host, port_str, &hints, &results);
  if (res != 0) {
    NOUVEAU_LOG("failed to resolve address");
    return res;
  }
  NOUVEAU_LOG("resolved address for %s with getaddrinfo", host);
  memcpy(outAddr, results->ai_addr, results->ai_addrlen);
  freeaddrinfo(results);

  return 0;
}

void bingo_connect(const char* host, int port) {
  bingo_network_shutdown();

  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    NOUVEAU_ERR("create socket");
    return;
  }

  struct sockaddr_storage server_addr;
  int res = resolve_addr(host, port, &server_addr);
  if (res != 0) {
    NOUVEAU_LOG("failed to resolve address (%d)", res);
    bingo_network_shutdown();
    return;
  }
  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    NOUVEAU_ERR("connect");
    bingo_network_shutdown();
    return;
  }

  NOUVEAU_LOG("connected");
}

void send_state_if_outdated() {
  BingoCompletion completion = get_current_player_bingo_completion();
  BingoPlayerLocation location = get_current_player_location();

  if (completion != last_sent_completion) {
    last_sent_completion = completion;
    send_completion(completion);
  }
  if (location != last_sent_location) {
    last_sent_location = location;
    send_location(location);
  }
}
