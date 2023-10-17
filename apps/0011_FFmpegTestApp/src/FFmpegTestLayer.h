#ifndef __FFMPEG_TEST_LAYER_H__
#define __FFMPEG_TEST_LAYER_H__

#include "sk_ui.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

class FFmpegTestLayer : public sk_ui::SkiaAppLayer {
public:
    FFmpegTestLayer( );
    virtual ~FFmpegTestLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
    AVFormatContext * pFormatCtx;
    int videoStream;
    const AVCodec * pCodec;
    AVCodecContext * pCodecCtx;
    AVFrame * pFrame;
    AVFrame * pFrameRGB;
    uint8_t * buffer;
    int numBytes;  
    struct SwsContext * sws_ctx;
    AVPacket * pPacket;


    bool        fPause;
    //SkVideoDecoder fDecoder;

    int         width;
    int         height;
    float       scale;
};



#endif // __FFMPEG_TEST_LAYER_H__
