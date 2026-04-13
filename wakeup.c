#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  int status, sockfd, numbytes;
  struct sockaddr_in broadcast_addr;
  int broadcast = 1;

  //1 synch stream plus 16 repeats of the MAC address = 17. MAC address and synch are each 6 bytes.
  int n = 17 * 6;
  uint8_t* bytes = calloc(n, 1);
  uint8_t mac[] = {0x50, 0xeb, 0xf6, 0x80, 0x8b, 0x66};

  if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1){
    perror("setsockopt (SO_BROADCAST)");
    exit(1);
  }
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_port = htons(9);
  inet_pton(AF_INET, "192.168.0.255", &broadcast_addr.sin_addr);

  memset(broadcast_addr.sin_zero, '\0', sizeof broadcast_addr.sin_zero);

  memset(bytes, 0xFF, 6); //FF stream

  for (int i = 6; i < n; i += 6){
    memcpy(bytes + i, mac, 6);
  }

  numbytes = sendto(sockfd, bytes, n, 0,
      (struct sockaddr *)&broadcast_addr, sizeof broadcast_addr);

  if (numbytes == -1) {
    perror("sendto");
    exit(1);
  }

  printf("sent %d bytes to %s\n", numbytes, inet_ntoa(broadcast_addr.sin_addr));

  free(bytes);
  close(sockfd);

  return 0;
}
