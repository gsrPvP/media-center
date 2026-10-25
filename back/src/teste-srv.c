#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_PATH "/tmp/gstmedia.sock"
#define CMD_SIZE 1024


int main(){
    int server_fd;
    int client_fd;
    char buffer[1024];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;

    if(server_fd == -1){
        perror("socket");
        return 1;
    }

    //zera o valor de addr
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);
    if(bind(server_fd, (struct sockaddr*)&addr,sizeof(addr)) == -1){
        perror("bind");
        return 1;
    };
    listen(server_fd, 5);

    while(1){
        
        client_fd = accept(server_fd, NULL,NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) -1,0);
        if(n > 0){
            buffer[n] = '\0';
            printf("%s\n",buffer);
            send(client_fd, "OK",2,0);
        }
        close(client_fd);
    }
    
    unlink(SOCKET_PATH);
    close(server_fd);
    return 0;
}
