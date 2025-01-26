#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Change "127.0.0.1" to the IP address of the server laptop
    char *server_ip = "192.168.43.209";  // Replace with the server laptop's IP address
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server\n");

    // Send a message to the server
    while (1) {
        printf("Enter message: ");
        fgets(buffer, 1024, stdin);

        // Send message to the server
        send(sock, buffer, strlen(buffer), 0);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Receive a message from the server
        int valread = read(sock, buffer, 1024);
        if (valread > 0) {
            printf("Server: %s\n", buffer);
        }
    }

    return 0;
}

