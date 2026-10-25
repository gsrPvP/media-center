#include "media.h"


GstElement *media_play(const char *path) {
    gchar *uri = gst_filename_to_uri(path, NULL);
    
    if (uri == NULL) {
        fprintf(stderr, "Erro ao converter caminho para URI\n");
        return NULL;
    }
    
    GstElement *pipeline = gst_element_factory_make("playbin", NULL);
    
    if (pipeline == NULL) {
        fprintf(stderr, "Erro ao criar playbin\n");
        g_free(uri);
        return NULL;
    }
    
    g_object_set(pipeline, "uri", uri, NULL);
    g_free(uri);
    
    GstStateChangeReturn ret =
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    if (ret == GST_STATE_CHANGE_FAILURE) {
        fprintf(stderr, "Erro ao iniciar reprodução\n");
        gst_object_unref(pipeline);
        return NULL;
    }
    
    return pipeline;
}

bool media_get_position(GstElement *pipeline, gint64 *seconds)
{
    gint64 position;

    if (pipeline == NULL)
        return false;

    if (!gst_element_query_position(
            pipeline, GST_FORMAT_TIME, &position))
        return false;

    *seconds = position / GST_SECOND;
    return true;
}
bool media_get_duration(GstElement *pipeline, gint64 *seconds)
{
    gint64 duration;

    if (pipeline == NULL)
        return false;

    if (!gst_element_query_duration(
            pipeline, GST_FORMAT_TIME, &duration))
        return false;

    *seconds = duration / GST_SECOND;
    return true;
}

bool media_seek_to(GstElement *pipeline, gint64 seconds)
{
    gint64 duration;

    if (pipeline == NULL)
        return false;

    if (!gst_element_query_duration(
            pipeline, GST_FORMAT_TIME, &duration))
        return false;

    gint64 target = seconds * GST_SECOND;

    if (target > duration)
        target = duration;

    return gst_element_seek_simple(
        pipeline,
        GST_FORMAT_TIME,
        GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
        target
    );
}
bool media_seek_relative(GstElement *pipeline, gint64 offset_seconds) {
    gint64 position;
    gint64 duration;

    if (pipeline == NULL)
        return false;

    if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &position))
        return false;

    if (!gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration))
        return false;

    gint64 target = position + offset_seconds * GST_SECOND;

    if (target < 0)
        target = 0;

    if (target > duration)
        target = duration;

    return gst_element_seek_simple(
        pipeline,
        GST_FORMAT_TIME,
        GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
        target
    );
}