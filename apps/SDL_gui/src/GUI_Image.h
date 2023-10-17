//
// Created by Panutat Tejasen on 3/2/2016 AD.
//

#ifndef GUI_IMAGE_GUI_IMAGE_H
#define GUI_IMAGE_GUI_IMAGE_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Downloader.h"
#include <string>

class GUI_Image : public GUI_View {
public:
    GUI_Image(GUI_View *parent, SDL_Texture *tex, int x = 0, int y = 0, int width = 0, int height = 0, int align = GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
    virtual ~GUI_Image() override;

    void download(std::string url, int port, std::string path);
    GUI_Downloader *downloader;

    virtual void draw() override;
};

#endif //GUI_IMAGE_GUI_IMAGE_H
