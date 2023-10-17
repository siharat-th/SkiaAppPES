//
// Created by Panutat Tejasen on 3/2/2016 AD.
//

#include "GUI_Image.h"
#include "SDL_image.h"

GUI_Image::GUI_Image(GUI_View *p, SDL_Texture *tex, int x, int y, int w, int h, int align) : GUI_View(p, "", x, y, w, h, cClear) {
    setIcon(tex);

    if (w == 0) {
        tw_area.w = icon_area.w;
    }
    
    if (h == 0) {
        tw_area.h = icon_area.h;
    }
    
    setBorder(0);
    downloader = NULL;
}

GUI_Image::~GUI_Image() {
}

void GUI_Image::download(std::string url, int port, std::string path) {
    downloader = new GUI_Downloader();
    SDL_Log("Download: %s", url.c_str());
    downloader->downloadFile(url.c_str(), port, path.c_str(), [this](GUI_Downloader *dl) {
        if (dl->http_count) {
            SDL_RWops *rw = SDL_RWFromMem(dl->http_data, dl->http_count);
            SDL_Surface *srf = IMG_Load_RW(rw, 1);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(GUI_renderer, srf);
            SDL_FreeSurface(srf);
            this->setIcon(tex);
        }
    });
}

void GUI_Image::draw() {
    if (!isVisible())
        return;

    predraw();

    if (textureIcon) {
        if (iconColor.a != 0) {
            SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
        }

        float scale_x = (float)tw_area.w / (float)icon_area.w;
        float scale_y = (float)tw_area.h / (float)icon_area.h;
        float scale = (scale_x < scale_y) ? scale_x : scale_y;

        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = (int)(icon_area.w * scale);
        rect.h = (int)(icon_area.h * scale);
        SDL_RenderCopyEx(GUI_renderer, textureIcon, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
    }

    if (display_cmd)
        display_cmd(this);

    if (_border > 0) {
        GUI_Rect r = GUI_Rect(_border, _border, tw_area.w - (2 * _border), tw_area.h - (2 * _border));

        if (corner) {
            GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, corner * GUI_scale, borderColor);
        } else {
            GUI_DrawRect(0, 0, tw_area.w, tw_area.h, borderColor);
        }

        SDL_IntersectRect(&r, &clip_area, &clip_area);
        SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
                                                         clip_area.y,
                                                         clip_area.w,
                                                         clip_area.h));
    }
}
