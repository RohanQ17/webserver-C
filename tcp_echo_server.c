// Standard C headers
#include <stdio.h>      // For printf(), perror()
#include <stdlib.h>     // For exit()
#include <string.h>     // For memset()

// POSIX networking and I/O
#include <unistd.h>         // For read(), write(), close()
#include <sys/types.h>      // For general data types
#include <sys/socket.h>     // For socket(), bind(), listen(), accept()
#include <netinet/in.h>     // For sockaddr_in, htons(), INADDR_ANY
#include <arpa/inet.h>      // For inet_ntoa() (optional for printing IPs)

#define PORT 8080           // Server will listen on this port

int main() {
    int server_fd, client_fd;                    // File descriptors for server and client sockets
    struct sockaddr_in server_addr, client_addr; // Structures to store address info
    socklen_t client_len;                        // Length of the client address structure
    char buffer[1024];                           // Buffer to read and send data
    ssize_t bytes_read;                          // Number of bytes read from client

    // 1. Create a TCP socket
    // AF_INET: IPv4
    // SOCK_STREAM: TCP
    // 0: Let OS choose the protocol (will default to TCP for SOCK_STREAM)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set up server address struct
    memset(&server_addr, 0, sizeof(server_addr)); // Zero out the memory
    server_addr.sin_family = AF_INET;             // Address family: IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Accept connections from any interface (0.0.0.0)
    server_addr.sin_port = htons(PORT);           // Port number in network byte order

    // 3. Bind the socket to IP + port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Mark the socket as passive (ready to accept connections)
    // Second argument: backlog (max queue length for pending connections)
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 5. Accept a client connection (blocking call)
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n",
           inet_ntoa(client_addr.sin_addr),             // Convert IP to human-readable string
           ntohs(client_addr.sin_port));                // Convert port to host byte order

    // 6. Read data from the client
    bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("read failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 7. Echo the data back to the client
    write(client_fd, buffer, bytes_read);  // Send same data back

    // 8. Close sockets
    close(client_fd);
    close(server_fd);

    printf("Connection closed. Server exiting.\n");
    return 0;
}
