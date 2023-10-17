//
//  PesSatinColumn.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 27/10/2565 BE.
//

#ifndef PesSatinColumn_hpp
#define PesSatinColumn_hpp

#include <stdio.h>
#include "DeclarativeUI.h"
#include "pesEngine.h"
#include "PESUndoRedoCommand.hpp"

class PesSatinColumn{
public:
    PesSatinColumn();
    
    void reset();
    void addCornerPoint(float x, float y);
    void addCurvePoint(float x, float y);
    void removePreviousPoint();
    void draw(SkCanvas & canvas);
    void drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot);
    
    pesCubicSuperPath csp[2];
    
private:
    SkRect makeRectFromCenter(float x, float y, float w, float h);
};

class PesLines{
public:
    PesLines();
    
    void reset();
    void addCornerPoint(float x, float y);
    void addCurvePoint(float x, float y);
    void removePreviousPoint();
    void draw(SkCanvas & canvas);
    void drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot);
    
    pesCubicSuperPath csp;
    
private:
    SkRect makeRectFromCenter(float x, float y, float w, float h);
};

class PesPolygon{
public:
    PesPolygon();
    
    void reset();
    void addCornerPoint(float x, float y);
    void addCurvePoint(float x, float y);
    void removePreviousPoint();
    void draw(SkCanvas & canvas);
    void drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot);
    
    pesCubicSuperPath csp;
    
private:
    SkRect makeRectFromCenter(float x, float y, float w, float h);
};

class PesSelectedObject{
public:
    PesSelectedObject(){
        reset();
    }
    
    ~PesSelectedObject(){
        reset();
    }
    
    void reset(){
        node.cmd_ptr = NULL;
        node.prev_cp_ptr = NULL;
        node.next_cp_ptr = NULL;
        node.path_ptr = NULL;
    }
    
    struct{
        pesVec2f *cmd_ptr, *prev_cp_ptr, *next_cp_ptr;
        pesPath *path_ptr;
    }node;
    
    void translate(float x, float y){
        if(node.path_ptr){
            if(node.cmd_ptr){
                node.cmd_ptr->translate(x, y);
            }
            if(node.prev_cp_ptr){
                node.prev_cp_ptr->translate(x, y);
            }
            if(node.next_cp_ptr){
                node.next_cp_ptr->translate(x, y);
            }
            node.path_ptr->flagShapeChanged();
        }
    }
};

class PesPathEdit{
public:
    PesPathEdit();
    
    void reset();
    void draw(SkCanvas & canvas);
    bool canUndo();
    bool canRedo();
    void undo();
    void redo();
    void pushCommand(Command command);
    std::string getRedoName();
    std::string getUndoName();
    
    pesData *pesData;
    std::vector<pesPath> paths;
    pesVec2f center;
    float rotateDegree;
    PesSelectedObject selectedObject;
    pesPath::Command * hoverCommand;
    UndoRedo undoRedo;
    
private:
    SkRect makeRectFromCenter(float x, float y, float w, float h);
};

class PesStitchEdit{
public:
    PesStitchEdit();
    
    void reset();
    void draw(SkCanvas & canvas);
    bool canUndo();
    bool canRedo();
    void undo();
    void redo();
    void pushCommand(Command command);
    std::string getRedoName();
    std::string getUndoName();
    
    pesData *pesData;
    pesStitchBlockList fillBlocks;
    pesStitchBlockList strokeBlocks;
    pesVec2f center;
    float rotateDegree;
    pesVec2f * selectedNode;
    pesStitchBlock * selectedBlock;
    int selectedIndex;
    pesVec2f * hoveredNode;
    UndoRedo undoRedo;
    
private:
    SkRect makeRectFromCenter(float x, float y, float w, float h);
};

#endif /* PesSatinColumn_hpp */
