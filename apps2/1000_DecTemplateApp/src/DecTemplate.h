#ifndef __DECTEMPLATE_LOADER_H__
#define __DECTEMPLATE_LOADER_H__

#include "DeclarativeUI.h"

class DecTemplate : public DecContainer {
public:    
    DecTemplate();
    virtual ~DecTemplate() override;

    virtual void init() override;
    virtual void update() override;

    DecText *pTextInfo;
};

DecTemplate &Template();

#endif //__DECTEMPLATE_LOADER_H__