#ifndef __DEC_QRCODE_H__
#define __DEC_QRCODE_H__

#include "DecWidget.h"
#include "qrencode.h"

class DecQRCode : public DecWidget {
public:    
    DecQRCode(const char *text);
    virtual ~DecQRCode() override;

    virtual std::string type() override { return "QRCode"; }

    virtual DecWidget& text( const char *t) override;

  
    virtual void _draw_content(SkCanvas &canvas) override;

protected:

    int casesensitive;
    int eightbit;
    int version;
    int micro;

    float qrmargins;
    float qrborderwidth;
    uint32_t qrbordercolor;

    QRecLevel level = QR_ECLEVEL_L;
    QRencodeMode hint = QR_MODE_8;  

    QRcode *qr;
};

DecQRCode &QR( const char *text );

#endif //__DEC_QRCODE_H__