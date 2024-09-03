#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char file_name[256];

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Set address and port number for the server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(1);
    }

    printf("Connected to server...\n");

    // Receive list of files from server
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("List of files: %s\n", buffer);

    // Send file name to server
    printf("Enter file name to download: ");
    fgets(file_name, 256, stdin);
    file_name[strcspn(file_name, "\n")] = 0; // remove newline character
    send(client_fd, file_name, strlen(file_name), 0);

    // Receive file from server
    FILE *fp = fopen(file_name, "wb");
    if (fp == NULL) {
        printf("Failed to open file\n");
        exit(1);
    }
    while (1) {
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (strlen(buffer) == 0) break;
        fwrite(buffer, 1, strlen(buffer), fp);
    }
    fclose(fp);

    printf("File downloaded successfully\n");

    // Close client socket
    close(client_fd);

    return 0;
}
