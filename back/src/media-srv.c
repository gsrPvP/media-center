#include "media-srv.h"

// memset, strings, etc...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>



#define SOCKET_PATH "/tmp/media-center.sock"

int init_server(int *server_fd) {
  *server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  
  if (*server_fd == -1) {
    perror("srv_socket failed");
    return 1;
  }
  
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SOCKET_PATH);

  unlink(SOCKET_PATH);

  if (bind(*server_fd,
            (struct sockaddr *)&addr,
            sizeof(addr)) == -1) {
      perror("bind error");
      return 1;
  }
  if (listen(*server_fd, 5) == -1) {
      perror("listen error");
      return 1;
  }

  return 0;
}

int send_all(int fd, const void *buffer, size_t size) {
    const char *ptr = buffer;
    size_t total = 0;

    while (total < size) {
        ssize_t n = send(fd, ptr + total, size - total, 0);

        if (n <= 0)
            return -1;

        total += (size_t)n;
    }

    return 0;
}

int recv_all(int *fd, void *buffer, size_t size){
  size_t total = 0;
  char *ptr = buffer;

  while(total < size){
    ssize_t n = recv(*fd, ptr + total, size - total, 0);
    if( n == 0){
      return -1;
    }
    if ( n < 0){
        return -1;
    }
    total += (size_t)n;
  }
  return 0;
}

int read_command(
    int *server_fd,
    int *client_fd,
    CommandHeader *header,
    char *data,
    size_t data_size
) {
    *client_fd = accept(*server_fd, NULL, NULL);

    if (*client_fd == -1) {
        perror("accept");
        return -1;
    }

    if (recv_all(client_fd, header, sizeof(*header)) != 0) {
        perror("recv header");
        close(*client_fd);
        return -1;
    }

    if (header->data_len > data_size - 1 ||
        header->data_len > MAX_COMMAND_DATA) {
        fprintf(stderr, "Payload muito grande\n");
        close(*client_fd);
        return -1;
    }

    if (header->data_len > 0) {
        if (recv_all(client_fd, data, header->data_len) != 0) {
            fprintf(stderr, "Erro ao receber payload\n");
            close(*client_fd);
            return -1;
        }
    }

    data[header->data_len] = '\0';

    return 0;
}
