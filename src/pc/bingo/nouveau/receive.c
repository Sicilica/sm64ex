#include "receive.h"

#include "handlers.h"

#define MAX_RECEIVE_SIZE 4096

char recv_buf[MAX_RECEIVE_SIZE];
int leftover_bytes = 0;
uint16_t header[2];

void receive_messages() {
  while (TRUE) {
    int rlen = recv(sock, recv_buf + leftover_bytes, MAX_RECEIVE_SIZE - leftover_bytes, MSG_DONTWAIT);
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
      if (header[0] == 0x0) {
        // NOUVEAU_LOG("trying to read a header");
        if (leftover_bytes-offset < 2*sizeof(uint16_t)) {
          break;
        }
        memcpy(header, recv_buf+offset, 2*sizeof(uint16_t));
        offset += 2*sizeof(uint16_t);
        for (int i = 0; i < 2; i++) {
          header[i] = ntohs(header[i]);
        }
        
        // NOUVEAU_LOG("found header: t=%d s=%d", header[0], header[1]);
      }

      // NOUVEAU_LOG("trying to read a body (have %d, need %d)", leftover_bytes-offset, header[1]);
      if (leftover_bytes-offset < header[1]) {
        break;
      }
      cJSON *body = cJSON_ParseWithLength(recv_buf+offset, header[1]);
      offset += header[1];
      if (body == NULL) {
        NOUVEAU_LOG("failed to parse json!");
        bingo_network_shutdown();
        return;
      }

      switch (header[0]) {
      case TYPE_BOARD:
        handle_board(body);
        break;
      case TYPE_PLAYER_CONNECTED:
        handle_player_connected(body);
        break;
      case TYPE_PLAYER_DISCONNECTED:
        handle_player_disconnected(body);
        break;
      case TYPE_PLAYER_LOCATION:
        handle_player_location(body);
        break;
      case TYPE_PLAYER_COMPLETION:
        handle_player_completion(body);
        break;
      default:
        NOUVEAU_LOG("unhandled message type %d", header[0]);
      }
      cJSON_Delete(body);
      memset(header, 0x0, 2*sizeof(uint16_t));
    }
    leftover_bytes -= offset;
    NOUVEAU_LOG("done processing inbounds (%d leftover)", leftover_bytes);
    memcpy(recv_buf, recv_buf+offset, leftover_bytes);
  }
}

void reset_receive_buffers() {
  memset(header, 0x0, 2);
  leftover_bytes = 0;
}