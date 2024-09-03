#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    float basic_salary;

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Define the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connecting to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Taking input for basic salary
    printf("Enter Basic Salary: ");
    scanf("%f", &basic_salary);

    // Sending the basic salary to the server
    snprintf(buffer, sizeof(buffer), "%.2f", basic_salary);
    send(sock, buffer, strlen(buffer), 0);

    // Receiving the calculation results from the server
    read(sock, buffer, 1024);
    printf("Received: %s\n", buffer);

    // Closing the socket
    close(sock);

    return 0;
}
