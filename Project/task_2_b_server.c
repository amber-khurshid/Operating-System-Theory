#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int server_fd, new_socket, addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
    address.sin_port = htons(PORT);  // The port we will listen on

    // Bind socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        return -1;
    }

    printf("Client connected\n");

    // Handle communication with the client
    while (1) {
        // Read message from client
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            printf("Client disconnected or error occurred\n");
            break;
        }

        printf("Client: %s\n", buffer);

        // Send a response back to the client
        send(new_socket, "Message received", strlen("Message received"), 0);
    }

    // Close the connection
    close(new_socket);
    close(server_fd);
    return 0;
}

