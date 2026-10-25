#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/gstmedia.sock"

int main(int argc, char *argv[]){
    int client_fd;
    char buffer[1024];

    struct sockaddr_un address;

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(client_fd == -1){
        perror("socket");
        return 1;
    }

    memset(&address,0 ,sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if(connect(client_fd,(struct sockaddr *)&address,sizeof(address)) == -1){
        perror("connect");
        close(client_fd);
        return 1;
    }
    char *msg = "PAUSE";
    send(client_fd, msg,strlen(msg),0);

    ssize_t n = recv(client_fd,buffer,sizeof(buffer) -1,0);

    if(n > 0){
        buffer[n] = '\0';
        printf("Server Responder: %s\n",buffer);
    }
    
    close(client_fd);
    return 0;
}