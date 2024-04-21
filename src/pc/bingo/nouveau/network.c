#include "nouveau.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>

#include "json/cJSON.h"

#define NOUVEAU_LOG(...) printf("[BINGO/nouveau/log] ");printf(__VA_ARGS__);printf("\n")
#define NOUVEAU_ERR(...) printf("[BINGO/nouveau/err] failed to ");printf(__VA_ARGS__);printf(" (%d)", errno);printf("\n")

// struct sockaddr_in notice_addr;
// struct sockaddr_in6 server_addr;
int sock = -1;
#define INBOUND_BUF_SIZE 4096
char inbound_buffer[INBOUND_BUF_SIZE];
int leftover_bytes = 0;
uint16_t inbound_header[2];

// TODO rem this
void socket_test();
void socket_test_tcp();
void jank_test();
void connect_jank();
void send_jank(uint16_t type, cJSON *body);

void example_jank() {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddStringToObject(obj, "name", "Dummy");
  send_jank(1, obj);
  cJSON_Delete(obj);
}

void bingo_network_init(void) {
  NOUVEAU_LOG("build 21");
  init_bingo();

  gBingoPlayers[0].connected = true;
  gBingoPlayers[0].color = bingoPlayerColors[0];
  connect_jank();
  example_jank();
  // join_room(0);
  // jank_test();
  // socket_test();
  // socket_test_tcp();




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
  // socket_test();
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

    while (TRUE) {
      int rlen = recv(sock, inbound_buffer + leftover_bytes, INBOUND_BUF_SIZE - leftover_bytes, MSG_DONTWAIT);
      if (rlen < 0) {
        if (errno != 11) {
          NOUVEAU_ERR("recv");
          bingo_network_shutdown();
        }
        break;
      } else if (rlen == 0) {
        NOUVEAU_LOG("received 0 bytes; we probably timed out but aren't reporting an error...");
        bingo_network_shutdown();
        break;
      }
      NOUVEAU_LOG("received %d bytes", rlen);
      leftover_bytes += rlen;

      int offset = 0;
      while (leftover_bytes-offset > 0) {
        if (inbound_header[0] == 0x0) {
          // NOUVEAU_LOG("trying to read a header");
          if (leftover_bytes-offset < 2*sizeof(uint16_t)) {
            break;
          }
          memcpy(inbound_header, inbound_buffer+offset, 2*sizeof(uint16_t));
          offset += 2*sizeof(uint16_t);
          for (int i = 0; i < 2; i++) {
            inbound_header[i] = ntohs(inbound_header[i]);
          }
          
          // NOUVEAU_LOG("found header: t=%d s=%d", inbound_header[0], inbound_header[1]);
        }

        // NOUVEAU_LOG("trying to read a body (have %d, need %d)", leftover_bytes-offset, inbound_header[1]);
        if (leftover_bytes-offset < inbound_header[1]) {
          break;
        }
        cJSON *body = cJSON_ParseWithLength(inbound_buffer+offset, inbound_header[1]);
        offset += inbound_header[1];
        if (body == NULL) {
          NOUVEAU_LOG("failed to parse json!");
          bingo_network_shutdown();
          return;
        }

        switch (inbound_header[0]) {
        case 2: {
          cJSON *slot = cJSON_GetObjectItemCaseSensitive(body, "p");
          cJSON *name = cJSON_GetObjectItemCaseSensitive(body, "name");
          cJSON *color = cJSON_GetObjectItemCaseSensitive(body, "color");
          NOUVEAU_LOG("CONNECTED[%d]: %s", slot->valueint, name->valuestring);
          break;
        }
        case 3: {
          cJSON *slot = cJSON_GetObjectItemCaseSensitive(body, "p");
          NOUVEAU_LOG("DISCONNECTED[%d]", slot->valueint);
          break;
        }
        case 4: {
          cJSON *seed = cJSON_GetObjectItemCaseSensitive(body, "s");
          cJSON *config = cJSON_GetObjectItemCaseSensitive(body, "cfg");
          NOUVEAU_LOG("BOARD: cfg=%s s=%d", config->valuestring, seed->valueint);
          break;
        }
        case 5: {
          cJSON *slot = cJSON_GetObjectItemCaseSensitive(body, "p");
          cJSON *loc = cJSON_GetObjectItemCaseSensitive(body, "loc");
          NOUVEAU_LOG("LOCATION[%d]: %d", slot->valueint, loc->valueint);
          break;
        }
        case 6: {
          cJSON *slot = cJSON_GetObjectItemCaseSensitive(body, "p");
          cJSON *com = cJSON_GetObjectItemCaseSensitive(body, "com");
          NOUVEAU_LOG("COMPLETION[%d]: %d", slot->valueint, com->valueint);
          break;
        }
        default:
          NOUVEAU_LOG("unhandled message type %d", inbound_header[0]);
        }
        cJSON_Delete(body);
        memset(inbound_header, 0x0, 2*sizeof(uint16_t));
      }
      leftover_bytes -= offset;
      NOUVEAU_LOG("done processing inbounds (%d leftover)", leftover_bytes);
      memcpy(inbound_buffer, inbound_buffer+offset, leftover_bytes);
    }
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

int ipv4_to_ipv6(const char *str, void *addr) {
  // TODO this is very dumb. surely it's possible to convert an ipv4->ipv6 address without building a string.
  char addrBuf[32];
  snprintf(addrBuf, 32, "::FFFF:%s", str);
  return inet_pton(AF_INET6, addrBuf, addr);
}

void connect_jank() {
  if (sock > 0) {
    close(sock);
  }
  memset(inbound_header, 0x0, 2);
  leftover_bytes = 0;

  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    NOUVEAU_ERR("create socket");
    return;
  }

  struct sockaddr_in6 server_addr = {};
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(6000);
  ipv4_to_ipv6("127.0.0.1", &server_addr.sin6_addr);
  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    NOUVEAU_ERR("connect");
    close(sock);
    sock = -1;
    return;
  }
}

void send_jank(uint16_t type, cJSON *msg) {
  char buf[1024];
  if (!cJSON_PrintPreallocated(msg, buf, 1024, 0)) {
    NOUVEAU_ERR("json print");
    return;
  }
  uint16_t size = strlen(buf);

  NOUVEAU_LOG("sending message (t=%d, s=%d): %s", type, size, buf);

  char header[4];
  uint16_t ns = htons(type);
  memcpy(header, &ns, 2);
  ns = htons(size);
  memcpy(header+2, &ns, 2);

  if (send(sock, header, 4, 0) < 0) {
    NOUVEAU_ERR("send header");
    return;
  }

  if (send(sock, buf, size, 0) < 0) {
    NOUVEAU_ERR("send body");
    return;
  }
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
  listenAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  listenAddr.sin_port = htons(8000);
  if (bind(sock, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) < 0) {
    NOUVEAU_LOG("failed to bind (%d)", errno);
  }
  NOUVEAU_LOG("bound to port %d", listenAddr.sin_port);

  // MEMO make it non-blocking
  // MEMO can also use MSG_DONTWAIT flag
  int flags = fcntl(sock, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sock, F_SETFL, flags);

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




  struct sockaddr_in notice_addr;
  notice_addr.sin_family = AF_INET;
  notice_addr.sin_port = htons(6000);
  notice_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (sendto(sock, req, strlen(req), 0, (struct sockaddr*)&notice_addr, sizeof(notice_addr)) < 0) {
    NOUVEAU_LOG("failed to send message (%d)", errno);
  }

  NOUVEAU_LOG("sent room request");
}
