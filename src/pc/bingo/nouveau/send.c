#include "send.h"

#include "json/cJSON.h"

#define MAX_SEND_SIZE 1024
char send_buf[MAX_SEND_SIZE];

#define KEEPALIVE_SECONDS 10
time_t last_keepalive;

void send_msg(uint16_t type, cJSON *msg) {
  if (!cJSON_PrintPreallocated(msg, send_buf, MAX_SEND_SIZE, 0)) {
    NOUVEAU_LOG("failed to print to json");
    return;
  }
  uint16_t size = strlen(send_buf);

  // NOUVEAU_LOG("sending message (t=%d, s=%d): %s", type, size, send_buf);

  char header[4];
  uint16_t ns = htons(type);
  memcpy(header, &ns, 2);
  ns = htons(size);
  memcpy(header+2, &ns, 2);

  last_keepalive = time(NULL);
  if (send(sock, header, 4, 0) < 0) {
    NOUVEAU_ERR("send header");
    return;
  }

  if (send(sock, send_buf, size, 0) < 0) {
    NOUVEAU_ERR("send body");
    return;
  }
}

void send_keepalive() {
  if (time(NULL) - last_keepalive < KEEPALIVE_SECONDS) {
    return;
  }

  // NOUVEAU_LOG("sending keepalive");
  cJSON *obj = cJSON_CreateObject();
  send_msg(TYPE_HELLO, obj);
  cJSON_Delete(obj);
}

void send_board_request(const char* config) {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddStringToObject(obj, "cfg", config);
  send_msg(TYPE_BOARD, obj);
  cJSON_Delete(obj);
}

void send_completion(uint32_t completion) {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddNumberToObject(obj, "com", completion);
  send_msg(TYPE_PLAYER_COMPLETION, obj);
  cJSON_Delete(obj);
}

void send_hello(const char* name, int preferred_color) {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddStringToObject(obj, "name", name);
  cJSON_AddNumberToObject(obj, "color", preferred_color);
  send_msg(TYPE_HELLO, obj);
  cJSON_Delete(obj);
}

void send_location(int16_t location) {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddNumberToObject(obj, "loc", location);
  send_msg(TYPE_PLAYER_LOCATION, obj);
  cJSON_Delete(obj);
}
