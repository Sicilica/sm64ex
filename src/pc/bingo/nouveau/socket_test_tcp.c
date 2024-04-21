#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOG(...) printf("[LOG] ");printf(__VA_ARGS__);printf("\n")
#define ERR(...) printf("[LOG/err] ");printf(__VA_ARGS__);printf(" (%d)\n", errno)

void socket_test_tcp();

// int main(int argc, char** argv) {
//   socket_test_tcp();
//   return 0;
// }

void socket_test_tcp() {
  int sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    ERR("failed to create socket");
    return;
  }

  struct sockaddr_in6 send_addr = {};
  send_addr.sin6_family = AF_INET6;
  send_addr.sin6_port = htons(6000);
  // send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // TODO this is very dumb. surely it's possible to convert an ipv4->ipv6 address without building a string.
  char addrBuf[32];
  snprintf(addrBuf, 32, "::FFFF:%s", "127.0.0.1");
  inet_pton(AF_INET6, addrBuf, &send_addr.sin6_addr);
  if (connect(sock, (struct sockaddr*)&send_addr, sizeof(send_addr)) < 0) {
    ERR("failed to connect");
    close(sock);
    return;
  }

  int32_t rid = 0;
  char req[1024];
  snprintf(req, 1024, "{\"s\":\"example\",\"r\":%d}", rid);

  if (write(sock, req, strlen(req)) < 0) {
    ERR("failed to send message");
    close(sock);
    return;
  }
  LOG("request sent");

  char res[1024];
  // struct sockaddr_in recv_addr;
  // socklen_t recv_addr_structlen = sizeof(recv_addr);
  struct sockaddr_in6 recv_addr = send_addr;
  if (read(sock, res, sizeof(res)) < 0) {
    if (errno == 11) {
      LOG("no response yet (check again later)");
    } else {
      ERR("failed to receive message");
    }
    close(sock);
    return;
  }
  if (inet_ntop(AF_INET6, &recv_addr.sin6_addr, addrBuf, 32) < 0) {
    ERR("failed to parse recv_addr");
  }
  LOG("got response from %s on port %d:", addrBuf, ntohs(recv_addr.sin6_port));
  LOG("%s", res);

  close(sock);
  LOG("done");
}
