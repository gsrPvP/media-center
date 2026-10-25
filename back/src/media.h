#ifndef MEDIA_H
#define MEDIA_H


#include <gst/gst.h>
#include "protocol.h"
#include <stdint.h>
#include <stdbool.h>

GstElement *media_play(const char *path);
bool media_get_position(GstElement *pipeline, gint64 *seconds);
bool media_get_duration(GstElement *pipeline, gint64 *seconds);
bool media_seek_to(GstElement *pipeline, gint64 seconds);
bool media_seek_relative(GstElement *pipeline, gint64 offset_seconds);


#endif