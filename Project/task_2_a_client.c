#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>  // Required for threading

#define PORT 8080

// Function to continuously listen for server messages
void *read_server_messages(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[1024];

    while (1) {
        int bytes_read = read(sock, buffer, sizeof(buffer) - 1); // Read message from server
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  // Null terminate the message
            printf("Broadcast from server: %s\n", buffer);

            // After receiving a message, prompt the user to send a new message
            printf("Enter message to send: ");
        } else if (bytes_read == 0) {
            printf("Server disconnected.\n");
            close(sock);
            exit(0); // Exit the client program on server disconnect
        } else {
            perror("Read error");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char message[1024];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    // Start a separate thread to listen for incoming broadcast messages from the server
    pthread_t read_thread;
    if (pthread_create(&read_thread, NULL, read_server_messages, (void *)&sock) != 0) {
        perror("Failed to create read thread");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Main loop to send client messages to the server
    while (1) {
        // Wait for server broadcast and then prompt for user input
        printf("Enter message to send: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Remove newline character

        // Send message to server
        if (send(sock, message, strlen(message), 0) == -1) {
            perror("Send failed");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }

    // Clean up and close socket (although this is not reached due to exit in read thread)
    close(sock);
    return 0;
}

