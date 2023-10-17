#include "FFmpegTestLayer.h"


using namespace sk_ui;

sk_sp<SkImage> img;

FFmpegTestLayer::FFmpegTestLayer()
    : fPause( false )
{   
    SKIAAPP()->setAppTitle( "FFmpegTest" );

    const char *video_fn = "resources/video/Start.m2v";

    // declare the AVFormatContext
    pFormatCtx = NULL; // [1]
    videoStream = -1;
    pCodec = NULL;
    pCodecCtx = NULL;
    pFrame = NULL;
    pFrameRGB = NULL;
    buffer = NULL;
    numBytes = 0;
    sws_ctx = NULL;
    pPacket = NULL;

    // now we can actually open the file:
    // the minimum information required to open a file is its URL, which is
    // passed to avformat_open_input(), as in the following code:
    int ret = avformat_open_input(&pFormatCtx, video_fn, NULL, NULL);    // [2]
    if (ret < 0)
    {
        // couldn't open file
        SkDebugf("Could not open file %s\n", video_fn);

        // exit with error
        return;
    }

    // The call to avformat_open_input(), only looks at the header, so next we
    // need to check out the stream information in the file.:
    // Retrieve stream information
    ret = avformat_find_stream_info(pFormatCtx, NULL);  //[3]
    if (ret < 0)
    {
        // couldn't find stream information
        printf("Could not find stream information %s\n", video_fn);

        // exit with error
        return;
    }

    // We introduce a handy debugging function to show us what's inside dumping
    // information about file onto standard error
    av_dump_format(pFormatCtx, 0, video_fn, 0);  // [4]
    
    // Now pFormatCtx->streams is just an array of pointers, of size
    // pFormatCtx->nb_streams, so let's walk through it until we find a video
    // stream.
    int i;

    // The stream's information about the codec is in what we call the
    // "codec context." This contains all the information about the codec that
    // the stream is using
    //AVCodecContext * pCodecCtxOrig = NULL;
    

    // Find the first video stream
    videoStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        // check the General type of the encoded data to match
	    // AVMEDIA_TYPE_VIDEO
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) // [5]
        {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1)
    {
        // didn't find a video stream
        SkDebugf( "Video Stream not found." );
        return;
    }

    // Get a pointer to the codec context for the video stream.
    // AVStream::codec deprecated
    // https://ffmpeg.org/pipermail/libav-user/2016-October/009801.html
    // pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;

    // But we still have to find the actual codec and open it:

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id); // [6]
    if (pCodec == NULL)
    {
        // codec not found
        SkDebugf("Unsupported codec!\n");

        // exit with error
        return;
    }    

    /**
     * Note that we must not use the AVCodecContext from the video stream
     * directly! So we have to use avcodec_copy_context() to copy the
     * context to a new location (after allocating memory for it, of
     * course).
     */

    // Copy context
    // avcodec_copy_context deprecation
    // http://ffmpeg.org/pipermail/libav-user/2017-September/010615.html
    //ret = avcodec_copy_context(pCodecCtx, pCodecCtxOrig);
    pCodecCtx = avcodec_alloc_context3(pCodec); // [7]
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
    if (ret != 0)
    {
        // error copying codec context
        SkDebugf("Could not copy codec context.\n");

        // exit with error
        return;
    }

    // Open codec
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);   // [8]
    if (ret < 0)
    {
        // Could not open codec
        SkDebugf("Could not open codec.\n");

        // exit with error
        return;
    }

    // Now we need a place to actually store the frame:
    // Allocate video frame
    pFrame = av_frame_alloc();  // [9]
    if (pFrame == NULL)
    {
        // Could not allocate frame
        SkDebugf("Could not allocate frame.\n");

        // exit with error
        return;
    }


    /**
     * Since we're planning to output PPM files, which are stored in 24-bit
     * RGB, we're going to have to convert our frame from its native format
     * to RGB. ffmpeg will do these conversions for us. For most projects
     * (including ours) we're going to want to convert our initial frame to
     * a specific format. Let's allocate a frame for the converted frame
     * now.
     */

    // Allocate an AVFrame structure
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL)
    {
        // Could not allocate frame
        SkDebugf("Could not allocate frame.\n");

        // exit with error
        return;
    }    


    // Even though we've allocated the frame, we still need a place to put
    // the raw data when we convert it. We use avpicture_get_size to get
    // the size we need, and allocate the space manually:


    // Determine required buffer size and allocate buffer
    // numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    // https://ffmpeg.org/pipermail/ffmpeg-devel/2016-January/187299.html
    // what is 'linesize alignment' meaning?:
    // https://stackoverflow.com/questions/35678041/what-is-linesize-alignment-meaning
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 32); // [10]
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));    // [11]

  /**
    * Now we use avpicture_fill() to associate the frame with our newly
    * allocated buffer. About the AVPicture cast: the AVPicture struct is
    * a subset of the AVFrame struct - the beginning of the AVFrame struct
    * is identical to the AVPicture struct.
    */
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    // Picture data structure - Deprecated: use AVFrame or imgutils functions
    // instead
    // https://www.ffmpeg.org/doxygen/3.0/structAVPicture.html#a40dfe654d0f619d05681aed6f99af21b
    // avpicture_fill( // [12]
    //     (AVPicture *)pFrameRGB,
    //     buffer,
    //     AV_PIX_FMT_RGB24,
    //     pCodecCtx->width,
    //     pCodecCtx->height
    // );
    av_image_fill_arrays( // [12]
        pFrameRGB->data,
        pFrameRGB->linesize,
        buffer,
        AV_PIX_FMT_RGB24,
        pCodecCtx->width,
        pCodecCtx->height,
        32
    );    

    // Finally! Now we're ready to read from the stream!

    /**
     * What we're going to do is read through the entire video stream by
     * reading in the packet, decoding it into our frame, and once our
     * frame is complete, we will convert and save it.
     */

    pPacket = av_packet_alloc();
    if (pPacket == NULL)
    {
        // couldn't allocate packet
        SkDebugf("Could not alloc packet,\n");

        // exit with error
        return;
    }  

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(   // [13]
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_0RGB32,   // sws_scale destination color scheme
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );      

    /**
     * The process, again, is simple: av_read_frame() reads in a packet and
     * stores it in the AVPacket struct. Note that we've only allocated the
     * packet structure - ffmpeg allocates the internal data for us, which
     * is pointed to by packet.data. This is freed by the av_free_packet()
     * later. avcodec_decode_video() converts the packet to a frame for us.
     * However, we might not have all the information we need for a frame
     * after decoding a packet, so avcodec_decode_video() sets
     * frameFinished for us when we have decoded enough packets the next
     * frame.
     * Finally, we use sws_scale() to convert from the native format
     * (pCodecCtx->pix_fmt) to RGB. Remember that you can cast an AVFrame
     * pointer to an AVPicture pointer. Finally, we pass the frame and
     * height and width information to our SaveFrame function.
     */

    i = 0;
  
}

FFmpegTestLayer::~FFmpegTestLayer() {
    /**
     * Cleanup.
     */

    sws_freeContext(sws_ctx);

    // Free the RGB image
    av_free(buffer);
    av_frame_free(&pFrameRGB);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_frame_free(&pFrame);
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
}

void FFmpegTestLayer::onResize( int w, int h, float s ) {
    width = w;
    height = h;
    scale = s;
}

void FFmpegTestLayer::onPaint( SkCanvas& canvas ) {
    static int fc = 0;

    ///
    //canvas.save(); 
    //canvas.scale( scale, scale );

    static bool bVideoEnd = false;

    if( !bVideoEnd ) {
        //SkDebugf( "Frame: %i\n", fc );
        fc++;
        if (av_read_frame(pFormatCtx, pPacket) >= 0)  // [14]
        {
            static int rc = 0;

            ///
            //SkDebugf( "Read: %i %i\n", fc, rc );
            rc++;

            // Is this a packet from the video stream?
            if (pPacket->stream_index == videoStream)
            {
                // Decode video frame
                // avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pPacket);
                // Deprecated: Use avcodec_send_packet() and avcodec_receive_frame().
                int ret = avcodec_send_packet(pCodecCtx, pPacket);    // [15]
                if (ret < 0)
                {
                    // could not send packet for decoding
                    SkDebugf("Error sending packet for decoding.\n");

                    // exit with eror
                    return;
                }

                while (ret >= 0)
                {
                    ret = avcodec_receive_frame(pCodecCtx, pFrame);   // [15]

                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        // EOF exit loop
                        break;
                    }
                    else if (ret < 0)
                    {
                        // could not decode packet
                        SkDebugf("Error while decoding.\n");

                        // exit with error
                        return;
                    }

                    // General N32 fallback.
                    const auto info = SkImageInfo::MakeN32(pFrame->width, pFrame->height,
                                                        SkAlphaType::kOpaque_SkAlphaType);

                    SkBitmap bm;
                    bm.allocPixels(info, info.minRowBytes());
                    SkDebugf( "Frame size: %i %i %i\n", pFrame->width, pFrame->height, info.minRowBytes());

                    constexpr auto fmt = SK_PMCOLOR_BYTE_ORDER(R,G,B,A) ? AV_PIX_FMT_RGBA : AV_PIX_FMT_BGRA;   

                    uint8_t*   dst[] = { (uint8_t*)bm.pixmap().writable_addr() };
                    int dst_stride[] = { SkToInt(bm.pixmap().rowBytes()) };

                    // Convert the image from its native format to RGB
                    sws_scale(
                        sws_ctx, 
                        (uint8_t const * const *)pFrame->data, 
                        pFrame->linesize, 
                        0, 
                        pCodecCtx->height, 
                        dst, 
                        dst_stride);

    
                    static int bi = 0;
                    //SkDebugf( "Bitmap Count: %i %i %i\n",fc, rc, bi );
                    bi++;

                    bm.setImmutable();
                    img = SkImage::MakeFromBitmap(bm);  

                    SkDebugf( "Img size: %i %i \n", img->width(), img->height() );
                                                      
                    // Save the frame to disk
                    {
                        // save the read AVFrame into ppm file
                        //saveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);

                        // print log information
                        /*
                        SkDebugf(
                            "Frame %c (%d) pts %d dts %d key_frame %d "
                            "[coded_picture_number %d, display_picture_number %d,"
                            " %dx%d]\n",
                            av_get_picture_type_char(pFrame->pict_type),
                            pCodecCtx->frame_number,
                            pFrameRGB->pts,
                            pFrameRGB->pkt_dts,
                            pFrameRGB->key_frame,
                            pFrameRGB->coded_picture_number,
                            pFrameRGB->display_picture_number,
                            pCodecCtx->width,
                            pCodecCtx->height
                        );
                        */
                    }
                }
            }
            canvas.clear( SK_ColorBLACK );
            if( img ) {
                SkPaint paint;
                //canvas.drawImage( img, 0, 0, );
                SkRect destRect = SkRect::MakeXYWH(0, 0, 1024, 600);
                canvas.drawImageRect( img, SkRect::MakeWH(img->width(), img->height()), 
                    destRect, SkSamplingOptions(SkFilterMode::kLinear), 
                    &paint, SkCanvas::kFast_SrcRectConstraint);

            }

            // Free the packet that was allocated by av_read_frame
            // [FFmpeg-cvslog] avpacket: Replace av_free_packet with
            // av_packet_unref
            // https://lists.ffmpeg.org/pipermail/ffmpeg-cvslog/2015-October/094920.html
            av_packet_unref(pPacket);
        } 
        else {
            bVideoEnd = true;
        }
    }
    

    //canvas.restore();
    
}

bool FFmpegTestLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false;
}

// Before use, check key available in function skui::Key get_key(unsigned short vk)
bool FFmpegTestLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey mod) {
    if ( key == skui::Key::k0 ) {
        if( state == skui::InputState::kUp ) {
            fPause = false;
        }
        else if( state == skui::InputState::kDown ) {
            fPause = true;
        }
        return true;
    }
    return false;
}

bool FFmpegTestLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool FFmpegTestLayer::onMouseWheel(float delta, skui::ModifierKey) {
    return false;
}

bool FFmpegTestLayer::onTouch(intptr_t owner, skui::InputState state, float x, float y) {
    return false;
}

