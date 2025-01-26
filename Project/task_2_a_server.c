#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h> // For multithreading

#define PORT 8080
#define MAX_CLIENTS 4

int client_socket[MAX_CLIENTS]; // Store client sockets
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex for shared resources
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Condition variable for synchronizing server prompt

// Flag to indicate when to prompt the server to send a message
int ready_to_prompt = 0; // 0 = not ready, 1 = ready

// Function to handle communication with a client
void *client_handler(void *client_sock) {
    int sock = *(int *)client_sock;
    char buffer[1024];
    int bytes_read;

    while (1) {
        bytes_read = read(sock, buffer, sizeof(buffer));
        if (bytes_read == 0) {
            printf("Client disconnected\n");
            close(sock);

            // Remove the client socket from the list
            pthread_mutex_lock(&lock);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == sock) {
                    client_socket[i] = 0; // Mark the client slot as free
                    break;
                }
            }
            pthread_mutex_unlock(&lock);

            pthread_exit(NULL); // Exit thread when the client disconnects
        }

        buffer[bytes_read] = '\0';
        printf("Message from client: %s\n", buffer);

        // After receiving the message, signal the server to prompt for broadcast message
        pthread_mutex_lock(&lock);
        ready_to_prompt = 1;
        pthread_cond_signal(&cond); // Signal the server to prompt
        pthread_mutex_unlock(&lock);
    }
}

// Function for the server to send messages to all clients
void *server_sender(void *arg) {
    char buffer[1024];

    while (1) {
        // Wait until it's time to prompt the server to send a message to clients
        pthread_mutex_lock(&lock);
        while (ready_to_prompt == 0) {
            pthread_cond_wait(&cond, &lock); // Wait for the signal
        }
        ready_to_prompt = 0; // Reset flag, so server waits for the next message
        pthread_mutex_unlock(&lock);

        // Prompt the server to send a message to all clients
        printf("Enter a message to send to all clients: ");
        fgets(buffer, sizeof(buffer), stdin); // Read input from the server console
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        // Broadcast message to all clients
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socket[i] != 0) { // Only send to connected clients
                send(client_socket[i], buffer, strlen(buffer), 0);
            }
        }
        pthread_mutex_unlock(&lock);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize client sockets to 0 (no clients connected)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Start a thread for the server to send messages to clients
    pthread_t sender_thread;
    pthread_create(&sender_thread, NULL, server_sender, NULL);

    while (1) {
        // Accept new connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("New client connected\n");

        // Store the client socket in the array
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socket[i] == 0) {
                client_socket[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&lock);

        // Create a thread for the new client
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, client_handler, (void *)&new_socket);
        pthread_detach(client_thread); // Detach thread so that it cleans up resources automatically
    }

    return 0;
}

