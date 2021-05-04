#include <jni.h>
#include <libavutil/log.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>

#include "logUtil.h"

//void (*callback)(void*, int, const char*, va_list)
void ffmpegLog(void *ptr, int level, char *content, va_list v1) {
    LOGD("ffmpegLog() -- ffmpeg log return");
}

JNIEXPORT jint
JNICALL
Java_com_cakes_androidffmpegpush_MainActivity_pushVideo(JNIEnv *env, jobject thiz, jstring src_path,
                                                        jstring dest_url) {

    av_log_set_callback(ffmpegLog);

    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket packet;

    int ret, i;
    char inputStr[500] = {0};
    char outputStr[500] = {0};
    char info[1000] = {0};
    char errorBuf[1000] = {0};

    sprintf(inputStr, "%s", (*env)->GetStringUTFChars(env, src_path, NULL));
    sprintf(outputStr, "%s", (*env)->GetStringUTFChars(env, dest_url, NULL));

    av_register_all();
    // network
    avformat_network_init();

    ret = avformat_open_input(&ifmt_ctx, inputStr, 0, 0);
    if (ret < 0) {
        av_strerror(ret, errorBuf, 1024);
        LOGE("Couldn't open input file: %s", errorBuf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        av_strerror(ret, errorBuf, 1024);
        LOGE("Failed to retrieve input stream information: %s", errorBuf);
        goto end;
    }

    int videoIndex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }

    //Output
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", outputStr); // RTMP
//    avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", outputStr); // UDP
    if (!ofmt_ctx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *inStream = ifmt_ctx->streams[i];
        AVStream *outStream = avformat_new_stream(ofmt_ctx, inStream->codec->codec);
        if (!outStream) {
            LOGE("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        ret = avcodec_copy_context(outStream->codec, inStream->codec);
        if (ret < 0) {
            av_strerror(ret, errorBuf, 1024);
            LOGE("Failed to copy context from input to output stream codec context: %s", errorBuf);
            goto end;
        }

        outStream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    //Open output URL
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, outputStr, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_strerror(ret, errorBuf, 1024);
            LOGE("Could not open output URL: %s", errorBuf);
            goto end;
        }
    }

    // write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_strerror(ret, errorBuf, 1024);
        LOGE("Error occurred when opening output URL: %s", errorBuf);
        goto end;
    }

    int frameIndex = 0;
    int64_t startTime = av_gettime();
    while (1) {
        AVStream *inStream, *outStream;
        ret = av_read_frame(ifmt_ctx, &packet);
        if (ret < 0) {
            av_strerror(ret, errorBuf, 1024);
            LOGE("av_read_frame() error: %s", errorBuf);
            break;
        }

        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (packet.pts == AV_NOPTS_VALUE) {
            // write PTS
            AVRational timeBase1 = ifmt_ctx->streams[videoIndex]->time_base;
            // duration between 2 frames(us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoIndex]->r_frame_rate);
            // parameters
            packet.pts = (double) (frameIndex * calc_duration) /
                         (double) (av_q2d(timeBase1) * AV_TIME_BASE);
            packet.dts = packet.pts;
            packet.duration = (double) calc_duration / (double) (av_q2d(timeBase1) * AV_TIME_BASE);
        }

        //Important:Delay
        if (packet.stream_index == videoIndex) {
            AVRational timeBase = ifmt_ctx->streams[videoIndex]->time_base;
            AVRational timeBaseQ = {1, AV_TIME_BASE};
            int64_t ptsTime = av_rescale_q(packet.dts, timeBase, timeBaseQ);
            int64_t nowTime = av_gettime() - startTime;
            int64_t interval = ptsTime - nowTime;
            if (interval > 0) {
                av_usleep(interval);
            }
        }
        inStream = ifmt_ctx->streams[packet.stream_index];
        outStream = ofmt_ctx->streams[packet.stream_index];
        /* copy packet */
        //Convert PTS/DTS
        packet.pts = av_rescale_q_rnd(packet.pts, inStream->time_base, outStream->time_base,
                                      AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.dts = av_rescale_q_rnd(packet.dts, inStream->time_base, outStream->time_base,
                                      AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.duration = av_rescale_q(packet.duration, inStream->time_base, outStream->time_base);
        packet.pos = -1;

        if (packet.stream_index == videoIndex) {
            LOGE("Send %8d video frames to output URL\n", frameIndex);
            frameIndex++;
        }

        ret = av_interleaved_write_frame(ofmt_ctx, &packet);
        if (ret < 0) {
            av_strerror(ret, errorBuf, 1024);
            LOGE("Error muxing packet: %s", errorBuf);
            break;
        }

        av_free_packet(&packet);
    }

    // write file trailer
    av_write_trailer(ofmt_ctx);

    end:
    avformat_close_input(&ifmt_ctx);
    // close output
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)) {
        avio_close(ofmt_ctx->pb);
    }
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        av_strerror(ret, errorBuf, 1024);
        LOGE("Error occurred: %s", errorBuf);
        return -1;
    }

    return 0;
}