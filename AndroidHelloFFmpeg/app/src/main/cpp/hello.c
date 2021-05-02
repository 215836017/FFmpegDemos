#include <jni.h>
#include <stdio.h>
#include <string.h>

#include <include/libavformat/avformat.h>
#include <include/libavcodec/avcodec.h>
#include <include/libavutil/avutil.h>
#include <include/libavfilter/avfilter.h>
#include "logUtil.h"

struct URLProtocol;

const long size = 4000000;

JNIEXPORT jstring
JNICALL
Java_com_cakes_androidhelloffmpeg_MainActivity_getConfig(JNIEnv *env, jobject thiz) {
    char config[size] = {0};
    sprintf(config, "%s", avcodec_configuration());

    return (*env)->NewStringUTF(env, config);
}

JNIEXPORT jstring
JNICALL
Java_com_cakes_androidhelloffmpeg_MainActivity_getProtocol(JNIEnv *env, jobject thiz) {

    char info[size] = {0};

    av_register_all();

    struct URLProtocol *pup = NULL;
    //Input
    struct URLProtocol **p_temp = &pup;

    avio_enum_protocols((void **) p_temp, 0);
    while ((*p_temp) != NULL) {
        sprintf(info, "%s[In ][%10s]\n", info, avio_enum_protocols((void **) p_temp, 0));
    }
    pup = NULL;

    // output
    avio_enum_protocols((void **) p_temp, 1);
    while ((*p_temp) != NULL) {
        sprintf(info, "%s[Out][%10s]\n", info, avio_enum_protocols((void **) p_temp, 1));
    }

    LOGI("MainActivity_getProtocol() -- %s", info);

    return (*env)->NewStringUTF(env, info);
}

JNIEXPORT jstring
JNICALL
Java_com_cakes_androidhelloffmpeg_MainActivity_getFormat(JNIEnv *env, jobject thiz) {

    char info[size] = {0};

    av_register_all();
    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    // input
    while (if_temp != NULL) {
        sprintf(info, "%s[In ][%10s]\n", info, if_temp->name);
        if_temp = if_temp->next;
    }

    // output
    while (of_temp != NULL) {
        sprintf(info, "%s[Out][%10s]\n", info, of_temp->name);
        of_temp = of_temp->next;
    }

    LOGI("MainActivity_getFormat -- %s", info);

    return (*env)->NewStringUTF(env, info);
}

JNIEXPORT jstring
JNICALL
Java_com_cakes_androidhelloffmpeg_MainActivity_getCodec(JNIEnv *env, jobject thiz) {

    char info[size] = {0};
    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%s[Decode]", info);
        } else {
            sprintf(info, "%s[Encode]", info);
        }

        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s[Video]", info);
                break;

            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s[Audio]", info);
                break;

            default:
                sprintf(info, "%s[Other]", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", c_temp->name);
        c_temp = c_temp->next;
    }
    LOGI("MainActivity_getCodec -- %s", info);
    return (*env)->NewStringUTF(env, info);
}

JNIEXPORT jstring
JNICALL
Java_com_cakes_androidhelloffmpeg_MainActivity_getFilter(JNIEnv *env, jobject thiz) {
    char info[size] = {0};

    avfilter_register_all();
    AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
    while (f_temp != NULL) {
        sprintf(info, "%s[%10s]\n", info, f_temp->name);
        f_temp = f_temp->next;
    }

    LOGD("MainActivity_getFilter -- %s", info);

    return (*env)->NewStringUTF(env, info);
}