//
// Created by Jimmy on 2/10/2016 AD.
//
#include <stdio.h>
#include "SDL_gui.h"
#include "GUI_View.h"
#include <SDL_ttf.h>

template<class T> T* re_alloc(T* arr, int& len) { // arr only used for type inference
    T* new_arr = new T[len * 2];
    memset(new_arr + len, 0, sizeof(T)*len);

    for (int i = 0; i < len; ++i)
        new_arr[i] = arr[i];

    delete[] arr;
    len *= 2;

    return new_arr;
}

extern int GUI_scale;
extern float GUI_mouseScale;

GUI_View * GUI_mouseCapturedWindow = NULL;
GUI_View * GUI_focusWindow = NULL;

GUI_View *GUI_View::createView( GUI_View *parent, const char *title, int x, int y, int width, int height )
{
    GUI_View *view = new GUI_View( parent, title, x, y, width, height );
    return view;
}

GUI_View *GUI_View::create(GUI_View *parent, const char *title, int x, int y, int width, int height) {
    GUI_View *v = new GUI_View(parent, title, x, y, width, height, cWhite);
    v->setAlignment(GUI_ALIGN_LEFT | GUI_ALIGN_TOP);
    v->setBorder(0);
    v->dragable = false;
    return v;
}


GUI_View::GUI_View(GUI_View *p, const char *t, int x, int y, int w, int h, SDL_Color col,
                   void(*d_cmd)(GUI_View *), bool(*e_cmd)(GUI_View *v, SDL_Event *ev)) :
    parent(p),
    children(0),
    topleft(x * GUI_scale, y * GUI_scale),
    tw_area(x * GUI_scale, y * GUI_scale, w == -1 ? w : w * GUI_scale, h == -1 ? h : h * GUI_scale),
    clip_area(0, 0, 0, 0),
    bgcol(col),
    display_cmd(d_cmd),
    handle_event_cmd(e_cmd),
    lst_child(-1),
    end_child(5),
    _hidden(false),
    _disable(false),
    dragable(true),
    click_through(false),
    dragging(false),
    lastMousePoint(0, 0),
    titleTexture(NULL),
    title_area(0, 0, 0, 0),
    titleColor(cBlack),
    iconColor(cClear),
    corner(0),
    textureIcon(NULL),
    icon_area(0, 0, 0, 0),
    icon_gap(5),
    layout(GUI_LAYOUT_ABSOLUTE),
    align(GUI_ALIGN_LEFT | GUI_ALIGN_TOP),
    padding{0,0,0,0},
    margin{0,0,0,0},
    bClose(false),
    tag(0),
    isFocus(false),
    clicktotop(false),
    _uiready(false),
    onClose(NULL),
    borderColor(cBlack),
    __handleInputEvent(true),
    title_align(GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER),
    __shared_ptr_textureIcon(false),
    min_width(0),
    min_height(0),

    // Tooh - text selection
    textSelectionScrollIndex(0),
    textSelectionStartIndex(0),
    textSelectionEndIndex(0)
{
    ox = x;
    oy = y;
    ow = w;
    oh = h;

    setBorder(1);

    if (t) {
        title = string(t);
    }

    if (parent)
        parent->add_child(this);

    if (ow == -1) {
        if (parent) {
            tw_area.w = parent->tw_area.w;
        } else {
            tw_area.w = GUI_windowWidth;
        }
    }

    updateSubLayout();
}

GUI_View::~GUI_View() {
	if (parent)
		parent->remove_child(this);

	if (onClose)
		onClose(this);

	remove_all_children();

    if (titleTexture) {
        SDL_DestroyTexture(titleTexture);
    }

    if (textureIcon) {
        if (__shared_ptr_textureIcon == false) {
            SDL_DestroyTexture(textureIcon);
            textureIcon = NULL;
        } else {
            // pom - shared_ptr do not destroy
            textureIcon = NULL;
        }
    }


}

void GUI_View::setEventHandler(bool(*event_cmd)(GUI_View *v, SDL_Event *ev)) {
    handle_event_cmd = event_cmd;
}

void GUI_View::close() {
    bClose = true;
}

void GUI_View::add_child(GUI_View *child) {
    child->parent = this;

    if (!children)
        children = new GUI_View*[end_child];
    else if (lst_child == end_child - 1)
        children = re_alloc(children, end_child);

    children[++lst_child] = child;
    updateLayout();
}

void GUI_View::remove_child(GUI_View *child) {
    for (int i = lst_child; i >= 0; --i) {
        if (children[i] == child) {
            for (; i < lst_child; ++i)
                children[i] = children[i + 1];

            children[lst_child--] = NULL;
            updateLayout();
            return;
        }
    }

    GUI_Log("*** ERROR: remove_child: child %p not found\n", child);
}

void GUI_View::remove_all_children() {
    if (children) {
        if (lst_child > -1) {
            int count = lst_child + 1;

            while (count--) {
                delete children[count];
                children[count] = NULL;
            }

            lst_child = count;
        }

        delete[] children;
        children = NULL;
    }
}

void GUI_View::predraw() {
    if (isVisible() == false)
        return;

    if (parent) {
        GUI_Rect parent_clip = GUI_Rect(parent->clip_area);
        parent_clip.x -= topleft.x;
        parent_clip.y -= topleft.y;
//        SDL_IntersectRect(GUI_MakeRect(0, 0, tw_area.w, tw_area.h), &parent_clip, &clip_area);
        
        clip_area = GUI_Rect(0, 0, tw_area.w, tw_area.h).getIntersection(parent_clip);

        // Bug of SDL
        if (clip_area.w < 0)
            clip_area.w = 0;
        if (clip_area.h < 0)
            clip_area.h = 0;
    } else {
        clip_area = GUI_Rect(0, 0, tw_area.w, tw_area.h);
    }

//#ifdef __EMSCRIPTENx__
//    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(tw_area.x, GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h, tw_area.w, tw_area.h));
//#else
//    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(tw_area.x, tw_area.y, tw_area.w, tw_area.h));
//#endif
//
//#ifdef __EMSCRIPTENx__
//    float magic_y = GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h;
//
//    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(tw_area.x + clip_area.x,
//                                                     0 - magic_y + clip_area.y,
//                                                     clip_area.w,
//                                                     clip_area.h));
//#else
//    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
//                                                     clip_area.y,
//                                                     clip_area.w,
//                                                     clip_area.h));
//#endif
//
//    clear();
    
    GUI_SetViewport(GUI_Rect(tw_area.x, tw_area.y, tw_area.w, tw_area.h));
    GUI_SetClipRect(clip_area);
    clear();
}

void GUI_View::clear(GUI_Rect *rect) {
    if (!rect)
        rect = GUI_MakeRect(0, 0, tw_area.w, tw_area.h);

    if (bgcol.a != 0) {
        if (corner != 0) {
            GUI_FillRoundRect(rect->x, rect->y, rect->w, rect->h, corner * GUI_scale, bgcol);
        } else {
            GUI_FillRect(rect->x, rect->y, rect->w, rect->h, bgcol);
        }
    }
}

//extern sk_sp<SkImage> exImage;

void GUI_View::draw() {
    if (!isVisible())
        return;

    predraw();

#if __USE_SKIA__
    if(imageIcon){
        auto bounds = imageIcon->bounds();
        auto canvas = GUI_canvas;
        SkPaint p;
        p.setStyle(SkPaint::kFill_Style);
        p.setColor(sk_color(iconColor));
        canvas->drawImage(imageIcon.get(), icon_area.x, icon_area.y);
    }
#else
    if (textureIcon) {
        if (iconColor.a != 0) {
            SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
        }

        SDL_RenderCopy(GUI_renderer, textureIcon, NULL, &icon_area);
    }
#endif

    if (display_cmd)
        display_cmd(this);

    if (_border > 0) {
        GUI_Rect r = GUI_Rect(_border, _border, tw_area.w - (2 * _border), tw_area.h - (2 * _border));

        if (corner) {
            GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, corner * GUI_scale, borderColor);
        } else {
            GUI_DrawRect(0, 0, tw_area.w, tw_area.h, borderColor);
        }

//        SDL_IntersectRect(&r, &clip_area, &clip_area);
//
//#ifdef __EMSCRIPTENx__
//        float magic_y = GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h;
//
//        SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(tw_area.x + clip_area.x,
//                                                         0 - magic_y + clip_area.y,
//                                                         clip_area.w,
//                                                         clip_area.h));
//#else
//        SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
//                                                         clip_area.y,
//                                                         clip_area.w,
//                                                         clip_area.h));
//#endif
        
        clip_area = r.getIntersection(clip_area);
        GUI_SetClipRect(GUI_Rect(clip_area.x, clip_area.y, clip_area.w, clip_area.h));
    }

    
#if __USE_SKIA__
    if(imageTitle && titleColor.a){
        auto bounds = imageTitle->bounds();
        if (textSelectionIsSelected()) {
            int startIndex = 0;
            int endIndex = 0;
            textSelectionGetSelectedIndex(&startIndex, &endIndex);

            if (startIndex < textSelectionScrollIndex) {
                startIndex = textSelectionScrollIndex;
            }

            GUI_Rect rect = title_area;
            std::string str = title.substr(startIndex, endIndex - startIndex);
            const char * text = str.c_str();
            SkRect bounds;
            GUI_skfonts[textSelectionFontSize].measureText(text, strlen(text), SkTextEncoding::kUTF8, &bounds);
            rect.w = bounds.width();

            if (startIndex > textSelectionScrollIndex) {
                const char * text = (title.substr(textSelectionScrollIndex, startIndex - textSelectionScrollIndex)).c_str();
                SkRect bounds;
                GUI_skfonts[textSelectionFontSize].measureText(text, strlen(text), SkTextEncoding::kUTF8, &bounds);
                rect.x += bounds.width();
            }
            GUI_FillRect(rect.x, rect.y, rect.w, rect.h, cTextSelected);
        }
        
        auto canvas = GUI_canvas;
        SkPaint p;
        p.setStyle(SkPaint::kFill_Style);
        if(!isEnable()){
            p.setColor(sk_color(cGrey));
        }
        else{
            p.setColor(SkColorSetARGB(titleColor.a, titleColor.r, titleColor.g, titleColor.b));
        }
        canvas->drawImage(imageTitle, title_area.x, title_area.y);
//        SkSamplingOptions smp;
//        canvas->drawImageRect(imageTitle, title_area.toSkRect(), smp);
        
//        SkRect src = SkRect::MakeXYWH(0,
//                                      0,
//                                      imageTitle->width(),
//                                      imageTitle->height());
//        SkRect dest = SkRect::MakeXYWH(title_area.x,
//                                       title_area.y,
//                                       title_area.w,
//                                       title_area.h);
//        SkSamplingOptions sampling;
//        SkCanvas::SrcRectConstraint constraint = SkCanvas::kFast_SrcRectConstraint;
//        GUI_canvas->drawImageRect(imageTitle, src, dest, sampling, &p, constraint);
//        GUI_canvas->drawImage(exImage, title_area.x, title_area.y);
        
      
        
//        canvas->drawImageRect(imageTitle, imageTitle->bounds(), dest, &p);

//        GUI_canvas->drawString(title.c_str(), title_area.x, title_area.y, GUI_skfonts[1], p);
        
//        auto data = imageTitle->encodeToData(SkEncodedImageFormat::kPNG, 50);
//        jsBuffer buff((const char*)data->bytes(), data->size());
//        jsBufferToFile("/Users/pom/Desktop/test/" + guiToString(SDL_GetTicks()) + ".png", buff);
    }
#else
    if (titleTexture && titleColor.a) {
        // Tooh - text selection
        if (textSelectionIsSelected()) {
            int startIndex = 0;
            int endIndex = 0;
            textSelectionGetSelectedIndex(&startIndex, &endIndex);

            if (startIndex < textSelectionScrollIndex) {
                startIndex = textSelectionScrollIndex;
            }

            GUI_Rect rect = title_area;
            std::string str = title.substr(startIndex, endIndex - startIndex);
            int w = 0;
            int h = 0;
            TTF_SizeUTF8(GUI_fonts[textSelectionFontSize], title.substr(startIndex, endIndex - startIndex).c_str(), &w, &h);
            rect.w = w;

            if (startIndex > textSelectionScrollIndex) {
                TTF_SizeUTF8(GUI_fonts[textSelectionFontSize], title.substr(textSelectionScrollIndex, startIndex - textSelectionScrollIndex).c_str(), &w, &h);
                rect.x += w;
            }

            GUI_FillRect(rect.x, rect.y, rect.w, rect.h, cTextSelected);
        }

        // pom
        if (!isEnable()) {
            SDL_Color c = cGrey;
            SDL_SetTextureColorMod(titleTexture, c.r, c.g, c.b);
        } else {
            SDL_SetTextureColorMod(titleTexture, titleColor.r, titleColor.g, titleColor.b);
        }

        SDL_RenderCopy(GUI_renderer, titleTexture, NULL, &title_area);
    }
#endif
}

void GUI_View::postdraw() {
    if (isVisible() == false)
        return;
    
    if (parent) {
        GUI_Rect parent_clip = GUI_Rect(parent->clip_area);
        parent_clip.x -= topleft.x;
        parent_clip.y -= topleft.y;
//        SDL_IntersectRect(GUI_MakeRect(0, 0, tw_area.w, tw_area.h), &parent_clip, &clip_area);
        clip_area = GUI_Rect(0, 0, tw_area.w, tw_area.h).getIntersection(parent_clip);
        
        // Bug of SDL
        if (clip_area.w < 0)
            clip_area.w = 0;
        if (clip_area.h < 0)
            clip_area.h = 0;
    } else {
        clip_area = GUI_Rect(0, 0, tw_area.w, tw_area.h);
    }
    
//#ifdef __EMSCRIPTENx__
//    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(tw_area.x, GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h, tw_area.w, tw_area.h));
//#else
//    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(tw_area.x, tw_area.y, tw_area.w, tw_area.h));
//#endif
//
//#ifdef __EMSCRIPTENx__
//    float magic_y = GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h;
//
//    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(tw_area.x + clip_area.x,
//                                                     0 - magic_y + clip_area.y,
//                                                     clip_area.w,
//                                                     clip_area.h));
//#else
//    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
//                                                     clip_area.y,
//                                                     clip_area.w,
//                                                     clip_area.h));
//#endif
    
    GUI_SetViewport(GUI_Rect(tw_area.x, tw_area.y, tw_area.w, tw_area.h));
    GUI_SetClipRect(GUI_Rect(clip_area.x, clip_area.y, clip_area.w, clip_area.h));
}

void GUI_View::setTitle(std::string t, int fontSize) {
    title = std::string(t);
#if __USE_SKIA__
    createTitleImage(fontSize);
#else
    createTitleTexture(fontSize);
#endif
    updateSubLayout();
}

void GUI_View::toTop() {
    if (parent) {
        parent->remove_child(this);
        parent->add_child(this);
    }
}

bool GUI_View::handleUserEvents(SDL_Event *ev) {
    return false;
}

bool GUI_View::handleEvents(SDL_Event *ev) {
    // PAINT - handle from back to front
    if (ev->type == GUI_EventPaint) {
        if (!isVisible())
            return false;

        this->draw();
#if __USE_SKIA__
        GUI_canvas->save();
#endif
        _uiready = true;

        if (handle_event_cmd) {
            if (handle_event_cmd(this, ev))
                return true;
        }

        for (int i = 0; i <= lst_child; i++) {
            GUI_View *wb = children[i];

            if (wb) {
                if (wb->bClose) {
                    if (GUI_IsModal(wb))
                        GUI_SetModal(NULL);

                    if ((GUI_View *)GUI_mouseCapturedWindow) {
                        GUI_View *p = GUI_mouseCapturedWindow;

                        while (p) {
                            if (p == wb) {
                                GUI_mouseCapturedWindow = NULL;
                                break;
                            }

                            p = p->parent;
                        }
                    }

                    delete(wb);
                    wb = NULL;
                    break;
                }

                if (wb->handleEvents(ev))
                    return true;
            }
        }
        
#if __USE_SKIA__
        GUI_canvas->save();
#endif
        postdraw();
        return false;
    } else if (ev->type >= GUI_EventUser) {
        if (handleUserEvents(ev)) {
            return true;
        }
    }

    // pom
    if (__handleInputEvent == false)
        return true;

    // Other event handle from front to back
    switch (ev->type) {
        case SDL_WINDOWEVENT:
            if (ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED &&
                this == GUI_TopView) {
                //GUI_Log("Resize: %i, %i", ev->window.data1, ev->window.data2);
                int w = ev->window.data1;
                int h = ev->window.data2;
#ifdef __ANDROID__
                w = GUI_windowWidth;
                h = GUI_windowHeight;
#endif
                this->resize(w - margin[1] - margin[3], h - margin[0] - margin[2]);
                this->updateSubLayout();
                this->updateLayout();
            }
            break;

#ifdef _RPi
        case SDL_FINGERDOWN:
        {
            SDL_TouchFingerEvent e = ev->tfinger;
            int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
            int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
            GUI_Log("Touch down at: %i %i\n", x, y);

            for (int i = lst_child; i >= 0; --i) {      // last child first
                GUI_View *wb = children[i]->hitTest(x, y, false);

                if (wb) {
                    if (wb->clicktotop)
                        wb->toTop();

                    if (!wb->handleEvents(ev)) {
                        if (wb->dragable) {
                            wb->dragging = true;
                            wb->lastMousePoint.set(x, y);
                            GUI_Log("%s Start dragging by Touch\n", wb->title.c_str());
                            GUI_mouseCapturedWindow = wb;
                            return true;
                        }
                    } else
                        return true;
                }
            }

            if (handle_event_cmd) {
                handle_event_cmd(this, ev);
            }

            return false;
            break;
        }

        case SDL_FINGERUP:
        {
            SDL_TouchFingerEvent e = ev->tfinger;
            int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
            int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
            GUI_Log("Touch up: %i %i\n", x, y);

            if (dragging) {
                dragging = false;
                GUI_mouseCapturedWindow = NULL;
                GUI_Log("%s End draging\n", title.c_str());
            } else {
                for (int i = lst_child; i >= 0; --i) {      // last child first
                    GUI_View *wb = children[i]->hitTest(x, y, false);
                    if (wb) {
                        if (wb->handleEvents(ev))
                            return true;
                    }
                }

                // pom
                if (handle_event_cmd) {
                    handle_event_cmd(this, ev);
                }
            }

            return false;
            break;
        }

        case SDL_FINGERMOTION:
        {
            SDL_TouchFingerEvent e = ev->tfinger;
            int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
            int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);

            if (dragging) {
                if (parent) {
                    if (!parent->hitTest(x, y, false)) {
                        return false;
                    }
                }

                int dx = x - lastMousePoint.x;
                int dy = y - lastMousePoint.y;
                lastMousePoint.set(x, y);
                move(dx, dy);
                return true;
            } else {
                for (int i = lst_child; i >= 0; --i) {      // last child first
                    GUI_View *wb = children[i]->hitTest(x, y, false);

                    if (wb) {
                        if (wb->handleEvents(ev))
                            return true;
                    }
                }

                if (handle_event_cmd) {
                    handle_event_cmd(this, ev);
                }
            }

            return false;
            break;
        }
#endif

        case SDL_MOUSEBUTTONDOWN:
        {
            if (isVisible() == false )
                return false;
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);

            for (int i = lst_child; i >= 0; --i) { // last child first
                GUI_View *wb = children[i]->hitTest(x, y, false);

                if (wb) {
                    if (wb->clicktotop)
                        wb->toTop();

                    if (!wb->handleEvents(ev)) {
                        if (wb->dragable) {
                            wb->dragging = true;
                            wb->lastMousePoint.set(x, y);
                            GUI_Log("%s Start draging\n", wb->title.c_str());
                            GUI_mouseCapturedWindow = wb;
                            return true;
                        }
                    } else
                        return true;
                }
            }

            if (handle_event_cmd) {
                handle_event_cmd(this, ev);
            }

            return false;
        }

        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);

            if (dragging) {
                dragging = false;
                GUI_mouseCapturedWindow = NULL;
                GUI_Log("%s End draging\n", title.c_str());
            } else {
                for (int i = lst_child; i >= 0; --i) { // last child first
                    GUI_View *wb = children[i]->hitTest(x, y, false);

                    if (wb) {
                        if (wb->handleEvents(ev))
                            return true;
                    }
                }

                // pom
                if (handle_event_cmd) {
                    handle_event_cmd(this, ev);
                }
            }

            return false;
        }

        case SDL_MOUSEMOTION:
        {
            SDL_MouseMotionEvent e = ev->motion;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);

            if (dragging) {
                if (parent) {
                    if (!parent->hitTest(x, y, false)) {
                        return false;
                    }
                }

                int dx = x - lastMousePoint.x;
                int dy = y - lastMousePoint.y;
                lastMousePoint.set(x, y);
                move(dx, dy);
                return true;
            } else {
                for (int i = lst_child; i >= 0; --i) { // last child first
                    GUI_View *wb = children[i]->hitTest(x, y, false);

                    if (wb) {
                        if (wb->handleEvents(ev))
                            return true;
                    }
                }

                if (handle_event_cmd) {
                    handle_event_cmd(this, ev);
                }
            }

            return false;
        }

        default:
            break;
    }

    if (handle_event_cmd) {
        if (handle_event_cmd(this, ev))
            return true;
    }

    for (int i = lst_child; i >= 0; i--) { // last child first
        GUI_View *wb = children[i];

        if (wb) {
            if (wb->isVisible() == false && ev->type == GUI_EventPaint)
                continue;

            if (wb->handleEvents(ev))
                return true;
        }
    }

    return false;
}

GUI_View *GUI_View::hitTest(int x, int y, bool bRecursive) {
    if (!isVisible())
        return 0;

    if (!isUIReady())
        return 0;

    if (click_through)
        return 0;

    SDL_Point pt;
    pt.x = x;
    pt.y = y;

    if (SDL_PointInRect(&pt, &tw_area)) {
        if (bRecursive) {
            for (int i = lst_child; i >= 0; --i) { // last child first
                GUI_View *wb = children[i]->hitTest(x, y);

                if (wb)
                    return wb;
            }
        }

        return this;
    }

    return 0;
}

void GUI_View::move_twarea(int dx, int dy) {
    tw_area.x += dx;
    tw_area.y += dy;

    for (int i = 0; i <= lst_child; ++i)
        children[i]->move_twarea(dx, dy);
}

void GUI_View::move(int dx, int dy) {
    topleft.x += dx; topleft.y += dy;

    tw_area.x += dx;
    tw_area.y += dy;

    for (int i = 0; i <= lst_child; ++i)
        children[i]->move_twarea(dx, dy);
}

SDL_Point GUI_View::getPosition() {
    SDL_Point pt;
    pt.x = tw_area.x / GUI_scale;
    pt.y = tw_area.y / GUI_scale;
    return pt;
}
SDL_Rect GUI_View::getRect() {
    SDL_Rect rect;
    rect.x = tw_area.x / GUI_scale;
    rect.y = tw_area.y / GUI_scale;
    rect.w = tw_area.w / GUI_scale;
    rect.h = tw_area.h / GUI_scale;
    
    return rect;
}


#if __USE_SKIA__
void GUI_View::createTitleImage(int fontSize){
    if (title.length() != 0) {
        textSelectionFontSize = fontSize;
        std::string str = title.substr(textSelectionScrollIndex);
        imageTitle =  GUI_createTextImage(str.c_str(), fontSize);
        auto bounds = imageTitle->bounds();
        if(bounds.width() && bounds.height()){
            title_area.w = imageTitle->width();
            title_area.h = imageTitle->height();
            title_area.x = (tw_area.w - imageTitle->width()) / 2;
            title_area.y = (tw_area.h - imageTitle->height()) / 2;
        }
        else {
            titleTexture = NULL;
            title_area.w = 0;
            title_area.h = 0;
            title_area.x = (tw_area.w) / 2;
            title_area.y = (tw_area.h) / 2;
        }
    }
}
void GUI_View::setIcon(sk_sp<SkImage> image){
    imageIcon = image;
    icon_area.x = 0;
    icon_area.y = 0;
    icon_area.w = imageIcon->width();
    icon_area.h = imageIcon->height();

    updateSubLayout();
}
#endif


void GUI_View::createTitleTexture(int fontSize) {
    if (titleTexture)
        SDL_DestroyTexture(titleTexture);

    if (title.length() != 0) {
        // Tooh - text selection
        textSelectionFontSize = fontSize;
        std::string str = title.substr(textSelectionScrollIndex);

        SDL_Surface* titleSurface = TTF_RenderUTF8_Blended(GUI_fonts[fontSize], str.c_str(), cWhite);

        if (titleSurface) {
            title_area.w = titleSurface->w;
            title_area.h = titleSurface->h;
            title_area.x = (tw_area.w - titleSurface->w) / 2;
            title_area.y = (tw_area.h - titleSurface->h) / 2;

            titleTexture = SDL_CreateTextureFromSurface(GUI_renderer, titleSurface);
        }

        if (titleTexture == NULL) {
            GUI_Log("Create Title Texture failed.\n");
        }

        SDL_FreeSurface(titleSurface);
    } else {
        titleTexture = NULL;
        title_area.w = 0;
        title_area.h = 0;
        title_area.x = (tw_area.w) / 2;
        title_area.y = (tw_area.h) / 2;
    }
}

void GUI_View::setIcon(SDL_Texture *tex, bool shared) {
    if (!tex) {
        if (textureIcon) {
            if (__shared_ptr_textureIcon == false) {
                SDL_DestroyTexture(textureIcon);
                textureIcon = NULL;
            } else {
                // pom - shared_ptr do not destroy
                textureIcon = NULL;
            }

            updateSubLayout(); // Tooh - fix layout bug when textureIcon is destroyed
        }

        return;
    }

    __shared_ptr_textureIcon = shared;

    int iW, iH;
    SDL_QueryTexture(tex, NULL, NULL, &iW, &iH);

    // pom
    if (textureIcon && !__shared_ptr_textureIcon)
        SDL_DestroyTexture(textureIcon);

    textureIcon = tex;
    icon_area.x = 0;
    icon_area.y = 0;
    icon_area.w = iW;
    icon_area.h = iH;

    updateSubLayout();
}

void GUI_View::centerToParent() {
    if (parent) {
        int x = (parent->tw_area.w - tw_area.w) / 2;
        int y = (parent->tw_area.h - tw_area.h) / 2;

        move(x - topleft.x, y - topleft.y);
    }
}

void GUI_View::setPosition(int x, int y) {
    int diffx = (x * GUI_scale) - topleft.x;
    int diffy = (y * GUI_scale) - topleft.y;

    move(diffx, diffy);
}

void GUI_View::setPadding(int p0, int p1, int p2, int p3) {
    padding[0] = p0;
    padding[1] = p1;
    padding[2] = p2;
    padding[3] = p3;
    updateLayout();
}

void GUI_View::setMargin(int p0, int p1, int p2, int p3) {
    margin[0] = p0;
    margin[1] = p1;
    margin[2] = p2;
    margin[3] = p3;

    if (parent)
        parent->updateLayout();
    else {
        if (this == GUI_TopView) {
            if (ox == 0)
                tw_area.x = margin[3] * GUI_scale;

            if (oy == 0)
                tw_area.y = margin[0] * GUI_scale;

            if (ow == -1)
                tw_area.w = (GUI_windowWidth - margin[1] - margin[3]) * GUI_scale;

            if (oh == -1)
                tw_area.h = (GUI_windowHeight - margin[0] - margin[2]) * GUI_scale;
        }
    }
}

void GUI_View::setLayout(int l) {
    layout = l;
    updateLayout();
}

void GUI_View::setAlignment(int a) {
    align = a;

    if (parent) {
        parent->updateLayout();
    }
}

void GUI_View::updateLayout() {
    if (layout == GUI_LAYOUT_ABSOLUTE) {
#ifdef DEBUG_LAYOUT
        GUI_Log("   Absolute\n");
#endif
        bool sz_changed = false;

        for (int i = 0; i <= lst_child; i++) {
            GUI_View *c = children[i];

            if (c->isVisible() == false)
                continue;

            if (c->align & GUI_ALIGN_RIGHT) {
                c->topleft.x = tw_area.w - c->tw_area.w - (c->margin[1] + padding[1]) * GUI_scale;
                c->tw_area.x = tw_area.x + c->topleft.x;
            } else if (c->align & GUI_ALIGN_CENTER) {
                c->topleft.x = (tw_area.w - c->tw_area.w) / 2;
                c->tw_area.x = tw_area.x + c->topleft.x;
            } else {
                if ((c->align & GUI_ALIGN_ABSOLUTE) == 0) { // Left Align || ow == 0
                    if (c->ox == 0) {
                        c->topleft.x = (c->margin[3] + padding[3]) * GUI_scale;
                    } else {
                        c->topleft.x = c->ox * GUI_scale;
                    }
                }

                c->tw_area.x = tw_area.x + c->topleft.x;

                if (c->ow == -1) {
                    c->tw_area.w = tw_area.w - c->topleft.x - (c->margin[1] + padding[1]) * GUI_scale;
                    c->updateSubLayout();
                }
            }

            if (ow == 0) {
                int wwx = (c->tw_area.w + (padding[1] + padding[3] + c->margin[1] + c->margin[3])*GUI_scale);

                if (wwx > tw_area.w) {
                    tw_area.w = wwx;
                    sz_changed = true;
                }
            }

            if (c->align & GUI_ALIGN_BOTTOM) {
                c->topleft.y = tw_area.h - c->tw_area.h - (c->margin[2] + padding[2]) * GUI_scale;
                c->tw_area.y = tw_area.y + c->topleft.y;
            } else if (c->align & GUI_ALIGN_VCENTER) {
                c->topleft.y = (tw_area.h - c->tw_area.h) / 2;
                c->tw_area.y = tw_area.y + c->topleft.y;
            } else {
                if ((c->align & GUI_ALIGN_ABSOLUTE) == 0) { // Top Align || oh == 0
                    if (c->oy == 0) {
                        c->topleft.y = (c->margin[0] + padding[0]) * GUI_scale;
                    } else {
                        c->topleft.y = c->oy * GUI_scale;
                    }
                }

                c->tw_area.y = tw_area.y + c->topleft.y;

                if (c->oh == -1) {
                    c->tw_area.h = tw_area.h - c->topleft.y - (c->margin[2] + padding[2]) * GUI_scale;
                    c->updateSubLayout();
                }
            }

            if (oh == 0) {
                int hhx = (c->tw_area.h + (padding[0] + padding[2] + c->margin[0] + c->margin[2])*GUI_scale);

                if (hhx > tw_area.h) {
                    tw_area.h = hhx;
                    sz_changed = true;
                }
            }

            if (ow == 0 || oh == 0) {
                if (sz_changed) {
                    int oww = ow;
                    int ohh = oh;
                    ow = tw_area.w;
                    oh = tw_area.h;
                    updateSubLayout();

                    if (align & (GUI_ALIGN_CENTER | GUI_ALIGN_RIGHT | GUI_ALIGN_BOTTOM | GUI_ALIGN_VCENTER)) {
                        if (parent) {
#ifdef DEBUG_LAYOUT
                            GUI_Log(">> Force parent update due to zero size\n");
#endif
                            parent->updateLayout();
                        }
                    }

                    ow = oww;
                    oh = ohh;
                }
            }

            c->updateLayout();
        }
    } else if (layout == GUI_LAYOUT_VERTICAL) {
#ifdef DEBUG_LAYOUT
        GUI_Log("   Vertical\n");
#endif
        int old_height = tw_area.h;
        int y = 0;

        y += padding[0] * GUI_scale;

        for (int i = 0; i <= lst_child; i++) {
            GUI_View *c = children[i];
            if( !c )
                continue;

            if (c->isVisible() == false)
                continue;

            if (c->align & GUI_ALIGN_ABSOLUTE) {
                continue;
            }

            if (c->align & GUI_ALIGN_VCENTER) {
                c->topleft.y = (tw_area.h - c->tw_area.h) / 2;
                c->tw_area.y = tw_area.y + c->topleft.y;

                for (int ii = i - 1; ii >= 0; ii--) {
                    GUI_View *cc = children[ii];

                    if (cc->isVisible() == false)
                        continue;

                    if (cc->align & GUI_ALIGN_VCENTER) {
                        cc->topleft.y -= c->tw_area.h / 2;
                        cc->tw_area.y = tw_area.y + cc->topleft.y;
                        c->topleft.y += cc->tw_area.h / 2;
                        c->tw_area.y = tw_area.y + c->topleft.y;
                    }
                }
            } else if (c->align & GUI_ALIGN_BOTTOM) {
                c->topleft.y = tw_area.h - c->tw_area.h - (padding[2] + c->margin[2]) * GUI_scale;
                c->tw_area.y = tw_area.y + c->topleft.y;

                for (int ii = i - 1; ii >= 0; ii--) {
                    GUI_View *cc = children[ii];

                    if (cc->isVisible() == false)
                        continue;

                    if (cc->align & GUI_ALIGN_BOTTOM) {
                        cc->move(0, 0 - (c->tw_area.h + (c->margin[0] + c->margin[2]) * GUI_scale));
                    } else if (cc->align & GUI_ALIGN_CENTER) {
                    } else {
                        if (cc->oh == -1) {
                            cc->tw_area.h -= c->tw_area.h + (c->margin[0] + c->margin[2]) * GUI_scale;;
                            cc->oh = -2;
                            cc->updateLayout();
                            cc->updateSubLayout();
                            cc->oh = -1;
                            break;
                        }
                    }
                }
            } else {
                c->topleft.y = y + c->margin[0] * GUI_scale;
                c->tw_area.y = tw_area.y + c->topleft.y;

                if (c->oh == -1) {
                    c->tw_area.h = tw_area.h - c->topleft.y - (c->margin[2] + padding[2])*GUI_scale;
                    c->updateSubLayout();
                }

                y += c->tw_area.h + c->margin[0] * GUI_scale + c->margin[2] * GUI_scale;

                if (oh == 0) {
                    tw_area.h = y;
                }
            }

            if (ow == 0) {
                int hhx = (c->tw_area.w + (padding[1] + padding[3] + c->margin[1] + c->margin[3])*GUI_scale);

                if (hhx > tw_area.w) {
                    tw_area.w = hhx;

                    if (parent) {
                        ow = tw_area.w;
                        parent->updateLayout();
                        ow = 0;
                    }
                }
            }

            if (c->align & GUI_ALIGN_RIGHT) {
                c->topleft.x = tw_area.w - c->tw_area.w - (c->margin[1] + padding[1])*GUI_scale;
                c->tw_area.x = tw_area.x + c->topleft.x;
            } else if (c->align & GUI_ALIGN_CENTER) {
                c->topleft.x = (tw_area.w - c->tw_area.w) / 2;
                c->tw_area.x = tw_area.x + c->topleft.x;
            } else {
                c->topleft.x = (padding[3] + c->margin[3]) * GUI_scale;
                c->tw_area.x = tw_area.x + c->topleft.x;

                if (c->ow == -1) {
                    c->tw_area.w = tw_area.w - (padding[3] + c->margin[3] + padding[1] + c->margin[1]) * GUI_scale;
                    int oww = c->ow;
                    int ohh = c->oh;
                    c->ow = c->tw_area.w;
                    c->oh = c->tw_area.h;
                    c->updateSubLayout();
                    c->ow = oww;
                    c->oh = ohh;
                }
            }

            c->updateLayout();
        }

        if (oh == 0) {
            tw_area.h += padding[2] * GUI_scale;

            if (tw_area.h != old_height) {
                int oww = ow;
                int ohh = oh;
                ow = tw_area.w;
                oh = tw_area.h;

                if (align & (GUI_ALIGN_CENTER | GUI_ALIGN_RIGHT | GUI_ALIGN_BOTTOM | GUI_ALIGN_VCENTER)) {
                    if (parent) {
                        parent->updateLayout();
                    }
                }

                ow = oww;
                oh = ohh;
            }
        }
    } else if (layout == GUI_LAYOUT_HORIZONTAL) {
        int x = 0;
        x += padding[3] * GUI_scale;

        for (int i = 0; i <= lst_child; i++) {
            GUI_View *c = children[i];

            if (c->isVisible() == false)
                continue;
            if (c->ow == -1) {
                c->tw_area.w = tw_area.w - (padding[1] + padding[3] + c->margin[1] + c->margin[3]) * GUI_scale;
                c->updateSubLayout();
            }

            if (c->align & GUI_ALIGN_ABSOLUTE) {
                continue;
            }

            if (c->align & GUI_ALIGN_CENTER) {
                c->topleft.x = (tw_area.w - c->tw_area.w) / 2;
                c->tw_area.x = tw_area.x + c->topleft.x;

                for (int ii = i - 1; ii >= 0; ii--) {
                    GUI_View *cc = children[ii];

                    if (cc->isVisible() == false)
                        continue;

                    if (cc->align & GUI_ALIGN_CENTER) {
                        cc->topleft.x -= c->tw_area.w / 2;
                        cc->tw_area.x = tw_area.x + cc->topleft.x;
                        c->topleft.x += cc->tw_area.w / 2;
                        c->tw_area.x = tw_area.x + c->topleft.x;
                        cc->updateLayout();
                    }
                }
            } else if (c->align & GUI_ALIGN_RIGHT) {
                c->topleft.x = (tw_area.w - c->tw_area.w) - ((c->margin[1] + padding[3]) * GUI_scale);
                c->tw_area.x = tw_area.x + c->topleft.x;

                for (int ii = i - 1; ii >= 0; ii--) {
                    GUI_View *cc = children[ii];

                    if (cc->isVisible() == false)
                        continue;

                    if (cc->align & GUI_ALIGN_RIGHT) {
                        cc->topleft.x -= c->tw_area.w + (c->margin[1] + c->margin[3]) * GUI_scale;
                        cc->tw_area.x = tw_area.x + cc->topleft.x;
                        int oww = cc->ow;
                        int ohh = cc->oh;
                        cc->ow = cc->tw_area.w;
                        cc->oh = cc->tw_area.h;
                        cc->updateSubLayout();
                        cc->ow = oww;
                        cc->oh = ohh;
                        cc->updateLayout();
                    } else if (cc->align & GUI_ALIGN_CENTER) {
                    } else {
                        if (cc->ow == -1) {
                            cc->tw_area.w -= c->tw_area.w + (c->margin[1] + c->margin[3]) * GUI_scale;
                            cc->ow = -2;
                            cc->updateSubLayout();
                            cc->updateLayout();
                            cc->ow = -1;
                            break;
                        }
                    }
                }
            } else {
                c->topleft.x = x + c->margin[3] * GUI_scale;
                c->tw_area.x = tw_area.x + c->topleft.x;

                if (c->ow == -1) {
                    c->tw_area.w = tw_area.w - c->topleft.x - (padding[1] + c->margin[1])* GUI_scale;
                }

                x += c->tw_area.w + c->margin[1] * GUI_scale + c->margin[3] * GUI_scale;

                if (ow == 0) {
                    tw_area.w = x;
                }
            }

            if (oh == 0) {
                int hhx = (c->tw_area.h + (padding[0] + padding[2] + c->margin[0] + c->margin[2])*GUI_scale);

                if (hhx > tw_area.h) {
                    tw_area.h = hhx;

                    if (parent) {
                        oh = tw_area.h;
                        parent->updateLayout();
                        oh = 0;
                    }
                }
            }

            if (c->align & GUI_ALIGN_BOTTOM) {
                c->topleft.y = tw_area.h - c->tw_area.h - (padding[2] + c->margin[2])*GUI_scale;
                c->tw_area.y = tw_area.y + c->topleft.y;
            } else if (c->align & GUI_ALIGN_VCENTER) {
                c->topleft.y = (tw_area.h - c->tw_area.h) / 2;
                c->tw_area.y = tw_area.y + c->topleft.y;
            } else {
                c->topleft.y = (padding[0] + c->margin[0]) * GUI_scale;
                c->tw_area.y = tw_area.y + c->topleft.y;

                if (c->oh == -1) {
                    c->tw_area.h = tw_area.h - (padding[0] + c->margin[0] + padding[2] + c->margin[2]) * GUI_scale;
                }
            }

            c->updateLayout();
        }

        if (ow == 0) {
            tw_area.w += padding[1] * GUI_scale;
        }
    }
}

void GUI_View::updateSubLayout() {
    if (title_align & GUI_ALIGN_CENTER)
        title_area.x = (tw_area.w - title_area.w) / 2;

    if (title_align & GUI_ALIGN_VCENTER)
        title_area.y = (tw_area.h - title_area.h) / 2;
}

// pom
GUI_Rect GUI_View::getAbsoluteArea() { // local area(absolute non-scale)
    GUI_Rect area(topleft.x, topleft.y, tw_area.w / GUI_scale, tw_area.h / GUI_scale);
    return area;
}

void GUI_View::pushState() {
    ___hidden = _hidden;
    __disable = _disable;
    __uiready = _uiready;
}

void GUI_View::popState() {
    _hidden = ___hidden;
    _disable = __disable;
    _uiready = __uiready;
}

void GUI_View::disableInputEvent() {
    __handleInputEvent = false;
}

void GUI_View::enableInputEvent() {
    __handleInputEvent = true;
}

// Tooh - text selection
bool GUI_View::textSelectionIsSelected() {
    return (isActive() && textSelectionStartIndex != textSelectionEndIndex);
}

void GUI_View::textSelectionGetSelectedIndex(int* startIndex, int* endIndex) {
    *startIndex = textSelectionStartIndex;
    *endIndex = textSelectionEndIndex;

    if (*endIndex < *startIndex) {
        *startIndex = textSelectionEndIndex;
        *endIndex = textSelectionStartIndex;
    }
}

void GUI_View::textSelectionCancel() {
    textSelectionScrollIndex = 0;
    textSelectionStartIndex = 0;
    textSelectionEndIndex = textSelectionStartIndex;
}
