#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    float basic, da, hra, gross;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Read the basic salary from the client
    read(new_socket, buffer, 1024);
    basic = atof(buffer);

    // Calculate DA, HRA, and Gross Salary
    da = 0.10 * basic;  // DA is 10% of basic salary
    hra = 0.05 * basic; // HRA is 5% of basic salary
    gross = basic + da + hra;

    // Send the results back to the client
    snprintf(buffer, sizeof(buffer), "Gross Salary: %.2f, DA: %.2f, HRA: %.2f", gross, da, hra);
    send(new_socket, buffer, strlen(buffer), 0);

    printf("Calculations sent to client.\n");

    // Close the socket
    close(new_socket);
    close(server_fd);

    return 0;
}
