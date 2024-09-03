#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char file_list[1024];

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Set address and port number for the server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server listening on port 8080...\n");

    while (1) {
        // Accept incoming connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("Connection accepted from client IP address %s and port %d...\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Send list of files in the current directory
        system("ls > file_list.txt");
        FILE *fp = fopen("file_list.txt", "r");
        fread(file_list, 1, 1024, fp);
        fclose(fp);
        send(client_fd, file_list, strlen(file_list), 0);

        // Receive file name from client
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        printf("Client requested file: %s\n", buffer);

        // Send file to client
        FILE *file = fopen(buffer, "rb");
        if (file == NULL) {
            send(client_fd, "File not found", 13, 0);
        } else {
            while (1) {
                fread(buffer, 1, BUFFER_SIZE, file);
                int bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
                if (bytes_sent < 0) {
                    perror("send failed");
                    break;
                }
                if (feof(file)) break;
            }
            fclose(file);
            printf("File sent successfully\n");
        }

        // Close client connection
        close(client_fd);
    }

    return 0;
}
