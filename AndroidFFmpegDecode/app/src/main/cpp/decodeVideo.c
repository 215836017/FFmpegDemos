
#include <jni.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#include "logUtil.h"

//void (*callback)(void*, int, const char*, va_list)
void ffmpegLog(void *ptr, int level, char *content, va_list v1) {
    LOGD("ffmpegLog() -- ffmpeg log return");
}

JNIEXPORT jint JNICALL
Java_com_cakes_androidffmpegdecode_MainActivity_startDecode(JNIEnv *env, jobject thiz,
                                                            jstring src_path, jstring dest_path) {

    AVFormatContext *pFormatCtx;
    AVIOContext *pIOContext;
    int i, videoIndex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *outBuffer;
    AVPacket *packet;
    int ySize;
    int ret;
    int got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frameCount;
    clock_t timeStart, timeFinish;
    double timeDuration = 0.0;

    char inputStr[500] = {0};
    char outputStr[500] = {0};
    char info[1000] = {0};
    char errorBuf[500] = {0};

    sprintf(inputStr, "srcPath = %s", (*env)->GetStringUTFChars(env, src_path, NULL));
    sprintf(outputStr, "destPath = %s", (*env)->GetStringUTFChars(env, dest_path, NULL));

    // ffmpeg log
    av_log_set_callback(ffmpegLog);

    av_register_all();
    avcodec_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    if (NULL == pFormatCtx) {
        LOGE("NULL == pFormatCtx.\n");
        return -1;
    }

    pIOContext = av_mallocz(sizeof(AVIOContext));
    if (NULL == pIOContext) {
        LOGE("NULL == pIOContext.\n");
        return -1;
    }
    /*
     * 不写的话报错： No such file or directory
     * 写的话报错：   Invalid data found when processing input  --- 初步分析是库编译的有问题
     */
    pFormatCtx->pb = pIOContext;

    // avformat_open_input() --  @return 0 on success, a negative AVERROR on failure.
    ret = avformat_open_input(&pFormatCtx, inputStr, NULL, NULL);
    LOGD("ret of avformat_open_input() = %d\n", ret);
    if (ret != 0) {
        av_strerror(ret, errorBuf, 1024);
        LOGE("Couldn't open input stream: %s\n", errorBuf);
        return -1;
    }

    // avformat_find_stream_info() --  @return >=0 if OK, AVERROR_xxx on error
    if (avformat_find_stream_info(&pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }

    videoIndex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (NULL == pCodec) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }

    // avcodec_open2() -- @return zero on success, a negative value on error
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    outBuffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
                                     pCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    // SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
    //                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
    //                                  int flags, SwsFilter *srcFilter,
    //                                  SwsFilter *dstFilter, const double *param);
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    sprintf(info, "[Input     ]%s\n", inputStr);
    sprintf(info, "%s[Output    ]%s\n", info, outputStr);
    sprintf(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);

    fp_yuv = fopen(outputStr, "wb+");
    if (NULL == fp_yuv) {
        printf("Cannot open output file.\n");
        return -1;
    }

    frameCount = 0;
    timeStart = clock();
    // av_read_frame() -- @return 0 if OK, < 0 on error or end of file
    while (av_read_frame(pCodecCtx, packet) >= 0) {
        if (packet->stream_index == videoIndex) {
            // avcodec_decode_video2() -- @return On error a negative value is returned, otherwise the number of bytes
            ret = avcodec_decode_video2(pCodecCtx, pFrame, got_picture, packet);
            if (ret < 0) {
                LOGE("Decode Error.\n");
                return -1;
            }

            if (got_picture) {
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                ySize = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, ySize, fp_yuv);             // Y
                fwrite(pFrameYUV->data[1], 1, ySize / 4, fp_yuv);   // U
                fwrite(pFrameYUV->data[2], 1, ySize / 4, fp_yuv);   // V

                // output info
                char picTypeStr[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        sprintf(picTypeStr, "I");
                        break;
                    case AV_PICTURE_TYPE_B:
                        sprintf(picTypeStr, "B");
                        break;
                    case AV_PICTURE_TYPE_P:
                        sprintf(picTypeStr, "P");
                        break;

                    default:
                        sprintf(picTypeStr, "Other");
                        break;
                }
                LOGI("Frame Index: %5d. Type:%s", frameCount, picTypeStr);
                frameCount++;
            }
        }

        av_free_packet(packet);
    }

    //flush decoder
    //FIX: Flush Frames remained in Codec
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0) {
            break;
        }
        if (!got_picture) {
            break;
        }

        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                  0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
        ySize = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, ySize, fp_yuv);             // Y
        fwrite(pFrameYUV->data[1], 1, ySize / 4, fp_yuv);   // U
        fwrite(pFrameYUV->data[2], 1, ySize / 4, fp_yuv);   // V

        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                break;
            case AV_PICTURE_TYPE_P:
                sprintf(pictype_str, "P");
                break;
            case AV_PICTURE_TYPE_B:
                sprintf(pictype_str, "B");
                break;
            default:
                sprintf(pictype_str, "Other");
                break;
        }
        LOGI("Frame Index: %5d. Type:%s", frameCount, pictype_str);
    }

    timeFinish = clock();
    timeDuration = (double) (timeFinish - timeStart);
    sprintf(info, "%s[Time      ]%fms\n", info, timeDuration);
    sprintf(info, "%s[Count     ]%d\n", info, frameCount);

    sws_freeContext(img_convert_ctx);
    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(pFormatCtx);

    return 0;
}