//
//  PES5DocumentView.hpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#ifndef PES5DocumentView_hpp
#define PES5DocumentView_hpp

#include "DeclarativeUI.h"
#include "PES5ModalDialog.hpp"
#include "pesEngine.h"

namespace sk_ui {

struct ActionDescription{
    std::string action = "undefined";
    float mx = 0;
    float my = 0;
    float tx = 0;
    float ty = 0;
    float dw = 0;
    float dh = 0;
    float sx = 0;
    float sy = 0;
    float degree = 0;
    float maxAbsDegree = 0;
};

struct CornerOrigin{
    std::string x = "left";
    std::string y = "top";
};

class Pes5DocumentView : public DecWidget {
//class Pes5DocumentView : public DecStackContainer {
public:
    Pes5DocumentView();
    virtual ~Pes5DocumentView() override;
    virtual std::string type() override { return "Pes5DocumentView"; }
    virtual void update() override;
    void setSelectedIndex(int index, bool multiselect = false);
    
    void drawGrid(SkCanvas& canvas, bool draw_grid);
    void drawRuler(SkCanvas& canvas, bool isDarkMode);
    virtual void init() override;
    virtual void js_callback( std::string message ) override;

    virtual bool onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey) override;
    
    std::vector<pesVec2f> corners;
    std::vector<std::vector<pesVec2f>> lines;
    std::vector<int> lines_flag;
    std::vector<pesVec2f> angles;
    std::vector<pesVec2f> angles_start;
    std::vector<pesVec2f> angles_end;
    std::vector<int> lines_id;

    bool bDrawGrid;

    bool isDarkMode = false;
    void setDarkMode(bool isDarkMode) {
        bool stateChange = this->isDarkMode != isDarkMode;
        if (stateChange) {
            if (isDarkMode) {
                this->bg_color(0xff080808);
            } else {
                this->bg_color(0xfff8f8f8);
            }
            this->isDarkMode = isDarkMode;
        }
    }

    int getLineByPoint( pesVec2f pt, int start_index, float max_distance=4, int except_id=-1 );
    int getNearestCorner( pesVec2f pt, float max_distance );
    int getNearestAngle( pesVec2f pt, float max_distance, int except_id=-1 );
    int getTargetCorner( pesVec2f pt1, pesVec2f pt2, float max_distance=24, float max_degree=12 );
    int getLineCut( pesVec2f pt1, pesVec2f pt2, float max_distance=12, float max_degree=10, int except_id=-1, int *ptIndex=NULL );

    void makeEvenlyPolyline( std::vector<pesVec2f> &source, std::vector<pesVec2f> &target, float dotInterval );
    void makeNomals( std::vector<pesVec2f> &source, std::vector<pesVec2f> &target );

protected:
    virtual void _draw_content(SkCanvas& canvas) override;
    virtual bool onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) override;
    void mousePressed(int ix, int iy, skui::ModifierKey modifiers);
    void mouseMoved(int ix, int iy);
    void mouseDragged(int ix, int iy);
    void mouseReleased(int ix, int iy);
    
private:
    pesRectangle selectedLocalBound;
    int selectedIndex;
    std::string selectedCorner;
    int pressedX, pressedY;
    int previousX, previousY;
    bool pressed;
    ActionDescription actionDesc;
    
    CornerOrigin getOriginFromCorner(std::string corner);
    std::string getActionFromCorner(std::string corner);
//    std::string checkCorners(int mx, int my, const pesRectangle & localBound);
    std::string checkCorners(int mx, int my);
    std::string getRotatedCornerCursor(std::string corner, float rotateDegree);

public:
    void setCursorByID(std::string id, std::string cursorStyle, bool force = false);
};

Pes5DocumentView & CreatePesDocumentView();
Pes5DocumentView * GetPesDocumentView();
Pes5TracingDialog * GetPesTracingDialog();
Pes5ShapeDialog * GetPesShapeDialog();
Pes5PatternFillDialog * GetPesPatternFillDialog();
Pes5MotifFillDialog * GetPesMotifFillDialog();
Pes5MotifStrokeDialog * GetPesMotifStrokeDialog();
Pes5SettingDialog * GetPesSettingDialog();
Pes5OutlineShapeDialog * GetPesOutlineShapeDialog();

}

#endif /* PES5DocumentView_hpp */
