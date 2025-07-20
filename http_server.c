#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>     // Added later for graceful shutdown
#include <sys/stat.h>   // Added later for file operations
#include <fcntl.h>      // Added later for file operations

#define PORT 8080
#define BUFFER_SIZE 1024

// Added later: Global flag to control server lifecycle
volatile sig_atomic_t keep_running = 1;

// Added later: Handler to catch Ctrl+C and shutdown gracefully
void handle_signal(int sig) {
    printf("\nReceived signal %d, shutting down gracefully...\n", sig);
    keep_running = 0;
}

// Added later: Function to serve actual files from filesystem
int serve_file(int client_fd, const char* filepath) {
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        char not_found[] = "HTTP/1.0 404 Not Found\r\n"
                          "Server: webserver-c\r\n"
                          "Content-type: text/html\r\n\r\n"
                          "<html><body><h1>404 Not Found</h1></body></html>\r\n";
        write(client_fd, not_found, strlen(not_found));
        return -1;
    }
    
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    
    char headers[512];
    snprintf(headers, sizeof(headers),
             "HTTP/1.0 200 OK\r\n"
             "Server: webserver-c\r\n"
             "Content-type: text/html\r\n"
             "Content-Length: %ld\r\n\r\n",
             file_stat.st_size);
    write(client_fd, headers, strlen(headers));
    
    char file_buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
        write(client_fd, file_buffer, bytes_read);
    }
    
    close(file_fd);
    return 0;
}

int main(){
    // Start with basic buffer for HTTP requests
    char buffer[BUFFER_SIZE];
    
    // Create a TCP socket (this is our foundation)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("webserver error (socket)");
        return 1;
    }
    printf("socket created successfully!\n");
    
    // Fix: Prevent "Address already in use" error when restarting server
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        return 1;
    }
    
    // Configure where the server will listen (any IP, port 8080)
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // Attach socket to the port
    if(bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0){
        perror("webserver (bind error)");
        close(sockfd);
        return 1;
    }
    printf("socket bound successfully to address\n");
    
    // Start listening for incoming connections
    if(listen(sockfd, SOMAXCONN) != 0){
        perror("webserver (listen error)");
        close(sockfd);
        return 1;
    }
    printf("socket is now listening for connections on port %d\n", PORT);
    
    // Added later: Set up graceful shutdown (Ctrl+C handling)
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Main server loop - this is where we handle requests
    while(keep_running) {
        printf("Waiting for connections...\n");
        
        // Accept a connection from a client
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr,
                              (socklen_t *)&host_addrlen);
        if(newsockfd < 0) {
            if (!keep_running) break;
            perror("webserver(accept)");
            continue;
        }
        
        // Improvement: Log which client connected to us
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &host_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Connection accepted from %s\n", client_ip);
        
        // Read the HTTP request from client
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if(valread < 0) {
            perror("value cannot be read (read error)");
            close(newsockfd);
            continue;
        }
        
        // Added later: Parse the HTTP request to understand what client wants
        char method[16] = {0};
        char path[256] = {0};
        char version[16] = {0};
        
        if (valread > 0) {
            sscanf(buffer, "%15s %255s %15s", method, path, version);
            printf("Request: %s %s %s\n", method, path, version);
            
            // Added later: Route different URLs to different responses
            if (strcmp(method, "GET") == 0) {
                if (strcmp(path, "/") == 0) {
                    // Send home page
                    char resp[] = "HTTP/1.0 200 OK\r\n"
                                 "Server: webserver-c\r\n"
                                 "Content-type: text/html\r\n\r\n"
                                 "<html><body>"
                                 "<h1>Welcome to C Web Server!</h1>"
                                 "<p>Server is running successfully.</p>"
                                 "<p><a href='/about'>About</a></p>"
                                 "</body></html>\r\n";
                    write(newsockfd, resp, strlen(resp));
                } else if (strcmp(path, "/about") == 0) {
                    // Send about page
                    char resp[] = "HTTP/1.0 200 OK\r\n"
                                 "Server: webserver-c\r\n"
                                 "Content-type: text/html\r\n\r\n"
                                 "<html><body>"
                                 "<h1>About</h1>"
                                 "<p>This is a simple C web server built with POSIX sockets.</p>"
                                 "<p><a href='/'>Home</a></p>"
                                 "</body></html>\r\n";
                    write(newsockfd, resp, strlen(resp));
                } else if (strcmp(path, "/file") == 0) {
                    // Added later: Serve actual files from disk
                    serve_file(newsockfd, "test.html");
                } else {
                    // Send 404 for unknown URLs
                    char resp[] = "HTTP/1.0 404 Not Found\r\n"
                                 "Server: webserver-c\r\n"
                                 "Content-type: text/html\r\n\r\n"
                                 "<html><body>"
                                 "<h1>404 - Page Not Found</h1>"
                                 "<p><a href='/'>Go Home</a></p>"
                                 "</body></html>\r\n";
                    write(newsockfd, resp, strlen(resp));
                }
            } else {
                // Added later: Handle POST, PUT, DELETE etc.
                char resp[] = "HTTP/1.0 405 Method Not Allowed\r\n"
                             "Server: webserver-c\r\n"
                             "Content-type: text/html\r\n\r\n"
                             "<html><body><h1>405 - Method Not Allowed</h1></body></html>\r\n";
                write(newsockfd, resp, strlen(resp));
            }
        }
        
        // Close connection with this client
        close(newsockfd);
    }
    
    // Clean shutdown
    printf("Shutting down server...\n");
    close(sockfd);
    return 0;
}