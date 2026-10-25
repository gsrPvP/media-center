#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdint.h>

#define MAX_COMMAND_DATA 512

typedef enum{
    CMD_PLAY = 0,
    CMD_PAUSE,
    CMD_STOP,
    CMD_FWD,
    CMD_BWD,
    CMD_SEEK_TO,
    CMD_RESUME,
    CMD_GET_STATUS,
}command_types;

typedef struct{
    command_types type;
    unsigned int data_len;
}CommandHeader;

typedef struct {
    int64_t position;
    int64_t duration;
    int32_t state;
} MediaStatus;

#endif