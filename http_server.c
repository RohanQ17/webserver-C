#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(){
    // create a new socket
    int sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("webserver error (socket)");
    }
    else printf("socket created successfully!");

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen)!=0){
        perror("webserver (bind error)");
        return 1;
    }
    printf("socket binded successfully to address");
    //We’ve created a socket and bounded it to a local address, now we need to make sure that the socket is listening for incoming connection.
    //The symbolic constant SOMAXCONN in <sys/socket.h> is defined by our system (128 in the case of Linux), and we can use it to set the backlog argument (man sys_socket.h).
    if(listen(sockfd, SOMAXCONN) !=0){
        perror("webserver (listen error)");
    }
    printf("socket is now listening for connections");
}