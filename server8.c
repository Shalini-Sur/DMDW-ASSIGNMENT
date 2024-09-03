#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6000
#define BUFFER_SIZE 1024

int main() {
  int sockfd, clientfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len;
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
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind socket to address
  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    exit(1);
  }

  // Listen for connections
  if (listen(sockfd, 3) < 0) {
    perror("listen failed");
    exit(1);
  }

  printf("Listening on port %d\n", PORT);

  while (1) {
    client_addr_len = sizeof(client_addr);
    // Accept a connection
    clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (clientfd < 0) {
      perror("accept failed");
      exit(1);
    }

    printf("Waiting for connection from client...\n");
    printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive filename from client
    recv(clientfd, filename, BUFFER_SIZE, 0);
    printf("Trying to open the file: %s\n", filename);

    // Open file for reading
    fp = fopen(filename, "rb");
    if (fp == NULL) {
      perror("fopen failed");
      send(clientfd, "ERROR", 5, 0);
      close(clientfd);
      continue;
    }

    // Send file to client
    while (!feof(fp)) {
      int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);
      if (bytes_read > 0) {
        send(clientfd, buffer, bytes_read, 0);
      }
    }

    fclose(fp);
    printf("Done sending file %s\n", filename);

    // Close connection
    close(clientfd);
  }

  // Close socket
  close(sockfd);

  return 0;
}
