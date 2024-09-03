#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 6000
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  char filename[BUFFER_SIZE];
  FILE *fp;

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  // Set server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(PORT);

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect failed");
    exit(1);
  }

  while (1) {
    printf("Enter filename: ");
    scanf("%s", filename);

    // Send filename to server
    send(sockfd, filename, strlen(filename), 0);

    // Receive file
    fp = fopen(filename, "wb");
    if (fp == NULL) {
      perror("fopen failed");
      exit(1);
    }
    while (1) {
      int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
      if (bytes_received == 0) {
        break;
      }
      if (bytes_received < 0) {
        perror("recv failed");
        exit(1);
      }
      fwrite(buffer, 1, bytes_received, fp);
    }
    fclose(fp);

    printf("Written file %s-dl successfully\n", filename);
  }

  // Close socket
  close(sockfd);

  return 0;
}
