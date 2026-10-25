
#include "media.h"
#include "media-srv.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <gst/gst.h>

void free_media(GstElement pipeline, GstBus *bus, GstMessage *msg) {
  /* Free resources */
  gst_message_unref(msg);
  gst_object_unref(bus);
  gst_element_set_state(&pipeline, GST_STATE_NULL);
  gst_object_unref(&pipeline);
}

int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);
  int client_fd;
  int server_fd;

  GstElement *pipeline = NULL;
  GstState state;
  GstState pending;
  CommandHeader header;
  char data[MAX_COMMAND_DATA +1] = {0};

  gint64 position;
  gint64 duration;

  init_server(&server_fd);
  while (1) {
    if (media_get_position(pipeline, &position)){
      printf("Posição: %lld s\n", (long long)position);
    }
    if (media_get_duration(pipeline, &duration)){
      printf("Duração: %lld s\n", (long long)duration);
    }
    
    if(read_command(&server_fd, &client_fd, &header, data, sizeof(data)) != 0){
      printf("Erro ao executar o read_command");
      continue;
    };
    printf("\nCommando Recebido!\n");
    printf("\nComando: %d", header.type);
    printf("\nPath: %s\n", data);
    switch (header.type) {
    case CMD_PLAY:
        if (data[0] == '\0') {
            fprintf(stderr, "Caminho do filme vazio\n");
            break;
        }

        if (pipeline != NULL) {
            gst_element_set_state(pipeline, GST_STATE_NULL);
            gst_object_unref(pipeline);
            pipeline = NULL;
        }

        pipeline = media_play(data);

        if (pipeline != NULL) {
            printf("Pipeline iniciada!\n");
        }
        break;

    case CMD_PAUSE:
        if (pipeline != NULL) {
            gst_element_set_state(pipeline, GST_STATE_PAUSED);
            printf("Reprodução pausada\n");
        }
        break;

    case CMD_RESUME:
        if (pipeline != NULL) {
            gst_element_set_state(pipeline, GST_STATE_PLAYING);
            printf("Reprodução retomada\n");
        }
        break;

    case CMD_STOP:
        if (pipeline != NULL) {
            gst_element_set_state(pipeline, GST_STATE_NULL);
            gst_object_unref(pipeline);
            pipeline = NULL;
            printf("Reprodução parada\n");
        }
        break;
    
    case CMD_FWD:
        if(pipeline != NULL){
            media_seek_relative(pipeline, 10);
            printf("Avançando 10 segundos\n");
        }
        break;
    case CMD_BWD:
        if(pipeline != NULL){
            media_seek_relative(pipeline, -10);
            printf("Retrocendendo 10 segundos\n");
        }
        break;
    case CMD_SEEK_TO:
    if (pipeline != NULL && data[0] != '\0') {
        char *end;
        long long seconds = strtoll(data, &end, 10);

        if (*end != '\0' || seconds < 0) {
            fprintf(stderr, "Posição inválida\n");
            break;
        }

        if (media_seek_to(pipeline, seconds)) {
            printf("Posição alterada para %lld segundos\n", seconds);
        } else {
            fprintf(stderr, "Falha ao alterar posição\n");
        }
    }
      break;
    case CMD_GET_STATUS: {
      gint64 position = 0;
      gint64 duration = 0;
      const char *state_name = "stopped";

      if (pipeline != NULL) {
          GstState current;
          GstState pending;

          gst_element_get_state(pipeline, &current, &pending, 0);

          if (current == GST_STATE_PLAYING)
              state_name = "playing";
          else if (current == GST_STATE_PAUSED)
              state_name = "paused";

          media_get_position(pipeline, &position);
          media_get_duration(pipeline, &duration);
      }

      char response[256];

      int len = snprintf(
          response,
          sizeof(response),
          "{\"position\":%lld,\"duration\":%lld,\"state\":\"%s\"}\n",
          (long long)position,
          (long long)duration,
          state_name
      );

      if (len > 0 && (size_t)len < sizeof(response))
          send_all(client_fd, response, (size_t)len);

      break;
  }
    
    default:
        fprintf(stderr, "Comando desconhecido: %d\n", header.type);
        break;
    }
  }
  if (pipeline != NULL) {
      gst_element_set_state(pipeline, GST_STATE_NULL);
      gst_object_unref(pipeline);
  }
  return 0;
}
