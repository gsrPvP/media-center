#include "protocol.h"

// memset, strings, etc...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/media-center.sock"

int send_all(int fd, const void *buffer, size_t size)
{
    printf("Bytes to send: %d", (int)size);
    size_t total = 0;
    const char *ptr = buffer;

    while (total < size) {
        ssize_t n = send(fd, ptr + total, size - total, 0);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (n == 0) {
            return -1;
        }

        total += (size_t)n;
    }
    printf("Total of bytes sent: %d", (int)total);
    return 0;
}


int cli_connect(int *client_fd) {
  struct sockaddr_un address;
  *client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (*client_fd == -1) {
    perror("socket error");
    return 1;
  }
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, SOCKET_PATH);

  if (connect(*client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("Erro ao conectar");
    close(*client_fd);
    return 1;
  }
  printf("\nclient connectado\n");
  return 0;
};
void cli_disconnect(int *client_fd) { close(*client_fd); };
int main(int argc, char *argv[])
{
    char input[32];
    char path[MAX_COMMAND_DATA] = "/home/jurek/corte.mp4";

    printf("Media CLI\n");
    printf("Comandos: play, pause, resume, stop, quit\n");

    while (1) {
        printf("\nmedia-cli> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        char *command = strtok(input, " \t\n");
        char *argument = strtok(NULL, " \t\n");

        if (command == NULL)
            continue;

        CommandHeader header = {0};
        const void *payload = NULL;

        if (strcmp(command, "quit") == 0) {
            break;
        }
        else if (strcmp(command, "play") == 0) {
            header.type = CMD_PLAY;

            // Caminho do vídeo
            char *path = "/home/jurek/corte.mp4";

            header.data_len = strlen(path) + 1;
            payload = path;
        }
        else if (strcmp(command, "seek") == 0) {
            if (argument == NULL) {
                printf("Uso: seek <segundos>\n");
                continue;
            }

            header.type = CMD_SEEK_TO;
            header.data_len = strlen(argument) + 1;
            payload = argument;
        }
        else if (strcmp(command, "fwd") == 0) {
            header.type = CMD_FWD;
        }
        else if (strcmp(command, "bwd") == 0) {
            header.type = CMD_BWD;
        }
        else if (strcmp(command, "pause") == 0) {
            header.type = CMD_PAUSE;
        }
        else if (strcmp(command, "resume") == 0) {
            header.type = CMD_RESUME;
        }
        else if (strcmp(command, "stop") == 0) {
            header.type = CMD_STOP;
        }
        else {
            printf("Comando desconhecido.\n");
            continue;
        }

        int client_fd;

        if (cli_connect(&client_fd) != 0)
            continue;

        int result = send_all(
            client_fd, &header, sizeof(header)
        );

        if (result == 0 && payload != NULL) {
            result = send_all(
                client_fd, payload, header.data_len
            );
        }

        if (result != 0)
            perror("Erro ao enviar comando");
        else
            printf("\nComando enviado!\n");

        close(client_fd);
    }

    printf("\nEncerrando cliente.\n");
    return 0;
}
