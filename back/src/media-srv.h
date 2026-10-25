#ifndef MEDIA_SRV_H
#define MEDIA_SRV_H

#include "protocol.h"

int read_command(int *server_fd, int *client_fd, CommandHeader *header, char *data, size_t data_size);
int init_server(int *server_fd);
int send_all(int fd, const void *buffer, size_t size);
#endif
