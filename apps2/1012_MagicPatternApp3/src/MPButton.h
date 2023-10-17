#ifndef __MP_BUTTON_H__
#define __MP_BUTTON_H__

#include "DeclarativeUI.h"

class DecMPButton : public DecButton {
public:
    DecMPButton(const char *title, const int icon = 0, std::function<void(DecButton &button)> callback=nullptr);
    ~DecMPButton();
};

DecMPButton &MPButton(const char *title, const int icon = 0, std::function<void(DecButton &button)> callback=nullptr);

#endif //__MP_BUTTON_H__