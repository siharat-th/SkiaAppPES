#ifndef __DEC_MESSAGEBOX_H__
#define __DEC_MESSAGEBOX_H__

#include "DecModal.h"

class DecMessageBox : public DecModal {
public:    
    DecMessageBox();
    virtual ~DecMessageBox() override;

    DecMessageBox &message( const char *msg );

    virtual std::string type() override { return "MessageBox"; }
    void onOK();
protected:
    DecText *_pTextMessage;
};

DecMessageBox &MessageBox();

#endif //__DEC_MESSAGEBOX_H__