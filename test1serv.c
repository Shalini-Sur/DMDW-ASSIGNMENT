#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    float basic_salary, da, hra, gross_salary;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket to the defined address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Accepting a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Receiving basic salary from the client
    read(new_socket, buffer, 1024);
    basic_salary = atof(buffer);
    
    // Calculating DA, HRA, and Gross Salary
    da = 0.2 * basic_salary;
    hra = 0.1 * basic_salary;
    gross_salary = basic_salary + da + hra;

    // Preparing response
    snprintf(buffer, sizeof(buffer), "DA: %.2f, HRA: %.2f, Gross Salary: %.2f", da, hra, gross_salary);

    // Sending the response to the client
    send(new_socket, buffer, strlen(buffer), 0);

    // Closing the socket
    close(new_socket);
    close(server_fd);

    return 0;
}

