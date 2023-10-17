//
//  PesSatinColumn.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 27/10/2565 BE.
//

#include "PesSatinColumn.hpp"
#include "pesPathUtility.hpp"
#include "PES5Command.hpp"

PesSatinColumn::PesSatinColumn(){
    
}

void PesSatinColumn::reset(){
    csp[0].reset();
    csp[1].reset();
}

void PesSatinColumn::addCornerPoint(float x, float y){
    int idx = (csp[0].knots.size() + csp[1].knots.size())%2;
    csp[idx].addCornerPoint(pesVec2f(x, y));
    csp[idx].calculateCSP();
}

void PesSatinColumn::addCurvePoint(float x, float y){
    int idx = (csp[0].knots.size() + csp[1].knots.size())%2;
    csp[idx].addCurvePoint(pesVec2f(x, y));
    csp[idx].calculateCSP();
}

void PesSatinColumn::removePreviousPoint(){
    int idx = (csp[0].knots.size() + csp[1].knots.size() + 1)%2;
    csp[idx].removeLastPoint();
    csp[idx].calculateCSP();
}

SkRect PesSatinColumn::makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void PesSatinColumn::drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot){
    pesVec2f p = knot.p;
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setAntiAlias(true);
    if(knot.type==pesCubicSuperPath::KNOT_TYPE_CORNER){
        SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 255, 255, 0);
        canvas.drawRect(rect, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawRect(rect, paint);
    }
    else{
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 100, 148, 237);
        canvas.drawCircle(p.x, p.y, r, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawCircle(p.x, p.y, r, paint);
    }
}

void PesSatinColumn::draw(SkCanvas & canvas){
    float doc_scale = PES5_GetZoomScale();
    // draw paths
    SkPath sk0 = toSk(csp[0].path);
    SkPath sk1 = toSk(csp[1].path);
    
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);
    
    paint.setARGB(255, 100, 148, 237);
    canvas.drawPath(sk0, paint);
    paint.setARGB(255, 255, 165, 0);
    canvas.drawPath(sk1, paint);
    
    paint.setARGB(255, 0, 255, 0);
    int num = std::min(csp[0].knots.size(), csp[1].knots.size());
    for(int i=0; i<num; i++){
        pesVec2f p0 = csp[0].knots[i].p;
        pesVec2f p1 = csp[1].knots[i].p;
        canvas.drawLine(p0.x, p0.y, p1.x, p1.y, paint);
    }
    
    // draw knots
    num = std::max(csp[0].knots.size(), csp[1].knots.size());
    for(int i=0; i<num; i++){
        if(i<csp[0].knots.size()) drawKnot(canvas, csp[0].knots[i]);
        if(i<csp[1].knots.size()) drawKnot(canvas, csp[1].knots[i]);
    }
}


// Lines
PesLines::PesLines(){
}

void PesLines::reset(){
    csp.reset();
}

void PesLines::addCornerPoint(float x, float y){
    csp.addCornerPoint(pesVec2f(x, y));
    csp.calculateCSP();
}

void PesLines::addCurvePoint(float x, float y){
    csp.addCurvePoint(pesVec2f(x, y));
    csp.calculateCSP();
}

void PesLines::removePreviousPoint(){
    csp.removeLastPoint();
    csp.calculateCSP();
}

SkRect PesLines::makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void PesLines::drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot){
    pesVec2f p = knot.p;
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setAntiAlias(true);
    if(knot.type==pesCubicSuperPath::KNOT_TYPE_CORNER){
        SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 255, 255, 0);
        canvas.drawRect(rect, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawRect(rect, paint);
    }
    else{
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 100, 148, 237);
        canvas.drawCircle(p.x, p.y, r, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawCircle(p.x, p.y, r, paint);
    }
}

void PesLines::draw(SkCanvas & canvas){
    float doc_scale = PES5_GetZoomScale();
    // draw paths
    SkPath sk0 = toSk(csp.path);
    
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);
    
    paint.setARGB(255, 100, 148, 237);
    canvas.drawPath(sk0, paint);
    
    // draw knots
    paint.setARGB(255, 0, 255, 0);
    int num = csp.knots.size();
    for(int i=0; i<num; i++){
        drawKnot(canvas, csp.knots[i]);
    }
}


// Polygon
PesPolygon::PesPolygon(){
}

void PesPolygon::reset(){
    csp.reset();
}

void PesPolygon::addCornerPoint(float x, float y){
    csp.addCornerPoint(pesVec2f(x, y));
    csp.calculateCSP();
}

void PesPolygon::addCurvePoint(float x, float y){
    csp.addCurvePoint(pesVec2f(x, y));
    csp.calculateCSP();
}

void PesPolygon::removePreviousPoint(){
    csp.removeLastPoint();
    csp.calculateCSP();
}

SkRect PesPolygon::makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void PesPolygon::drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot){
    pesVec2f p = knot.p;
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setAntiAlias(true);
    if(knot.type==pesCubicSuperPath::KNOT_TYPE_CORNER){
        SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 255, 255, 0);
        canvas.drawRect(rect, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawRect(rect, paint);
    }
    else{
        paint.setStyle(SkPaint::kFill_Style);
        paint.setARGB(255, 100, 148, 237);
        canvas.drawCircle(p.x, p.y, r, paint);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setARGB(255, 0, 0, 0);
        canvas.drawCircle(p.x, p.y, r, paint);
    }
}

void PesPolygon::draw(SkCanvas & canvas){
    float doc_scale = PES5_GetZoomScale();
    // draw paths
    SkPath sk0 = toSk(csp.path);
    
    SkPaint paint;
    paint.setStrokeWidth(2/doc_scale);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);
    
    paint.setARGB(255, 100, 148, 237);
    canvas.drawPath(sk0, paint);
    
    // draw knots
    paint.setARGB(255, 0, 255, 0);
    int num = csp.knots.size();
    for(int i=0; i<num; i++){
        drawKnot(canvas, csp.knots[i]);
    }
}


// Path Edit
PesPathEdit::PesPathEdit(){
}

void PesPathEdit::reset(){
    selectedObject.reset();
    pesData = NULL;
    hoverCommand = NULL;
    paths.clear();
    undoRedo.clear();
    PES5_UpdateUndoRedoTooltips();
}

SkRect PesPathEdit::makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

//void PesPolygon::drawKnot(SkCanvas & canvas, const pesCubicSuperPath::Knot & knot){
//    pesVec2f p = knot.p;
//    const float s = 10;
//    const float r = s/2;
//    SkPaint paint;
//    paint.setStrokeWidth(2);
//    paint.setAntiAlias(true);
//    if(knot.type==pesCubicSuperPath::KNOT_TYPE_CORNER){
//        SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
//        paint.setStyle(SkPaint::kFill_Style);
//        paint.setARGB(255, 255, 255, 0);
//        canvas.drawRect(rect, paint);
//        paint.setStyle(SkPaint::kStroke_Style);
//        paint.setARGB(255, 0, 0, 0);
//        canvas.drawRect(rect, paint);
//    }
//    else{
//        paint.setStyle(SkPaint::kFill_Style);
//        paint.setARGB(255, 100, 148, 237);
//        canvas.drawCircle(p.x, p.y, r, paint);
//        paint.setStyle(SkPaint::kStroke_Style);
//        paint.setARGB(255, 0, 0, 0);
//        canvas.drawCircle(p.x, p.y, r, paint);
//    }
//}

void PesPathEdit::draw(SkCanvas & canvas){
    float doc_scale = PES5_GetZoomScale();
    const float pes_scale = std::min(abs(pesData->parameter.ppefScaleX), abs(pesData->parameter.ppefScaleY));
    const float s = 10/doc_scale;
    const float r = s/2;
    
    canvas.save();
    {
        canvas.translate(center.x, center.y);
        canvas.rotate(rotateDegree);
        canvas.translate(-center.x, -center.y);
        SkPaint paint;
        paint.setStrokeWidth(2/doc_scale);
        paint.setAntiAlias(true);

        auto whitesmoke = SkColorSetARGB(0xff, 0xe7, 0xe7, 0xe7);
        
        for (auto& path : paths) {
            if (path.bVisible == false) {
                continue;
            }

            SkPath skpath = toSk(path);
            if (path.isFill()) {
                pesColor c = path.getFillColor();
                paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                paint.setStyle(SkPaint::kFill_Style);
                canvas.drawPath(skpath, paint);
            }
            if (path.isStroke()) {
                pesColor c = path.getStrokeColor();
                paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                paint.setStrokeWidth(abs(path.getStrokeWidth() * pes_scale));
                //paint.setStrokeWidth(abs(path.getStrokeWidth()));
                paint.setStyle(SkPaint::Style::kStroke_Style);
                paint.setStrokeCap(SkPaint::Cap::kButt_Cap);
                paint.setStrokeJoin(SkPaint::Join::kBevel_Join);
                canvas.drawPath(skpath, paint);
                // add details
                // inv-color, hairline
                paint.setColor(SkColorSetARGB(0xff, 0xff - c.r, 0xff - c.g, 0xff - c.b));
                paint.setStrokeWidth(0);
                canvas.drawPath(skpath, paint);
            } else {
                paint.setStyle(SkPaint::Style::kStroke_Style);
                // add details
                // whitesmoke-color, hairline
                paint.setColor(whitesmoke);
                paint.setStrokeWidth(0);
                paint.setBlendMode(SkBlendMode::kDifference);
                canvas.drawPath(skpath, paint);
                paint.setBlendMode(SkBlendMode::kSrcOver);
            }
            
//            const float s = 10/doc_scale;
//            const float r = s/2;
            // add details
            // hairline
            paint.setStrokeWidth(0);
            for (pesPath::Command& c : path.getCommands()) {
                // add details
                //paint.setStrokeWidth(1/doc_scale);
                pesVec2f p = c.to;
                if(c.type==pesPath::Command::_moveTo || c.type==pesPath::Command::_lineTo){
                    SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 255, 255, 0);
                    canvas.drawRect(rect, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawRect(rect, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawRect(rect, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                else if(c.type==pesPath::Command::_bezierTo || c.type==pesPath::Command::_quadBezierTo){
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 100, 148, 237);
                    canvas.drawCircle(p.x, p.y, r, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawCircle(p.x, p.y, r, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawCircle(p.x, p.y, r, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
            }
            
            // draw selected node
            if(selectedObject.node.path_ptr && selectedObject.node.cmd_ptr){
                pesVec2f p = *selectedObject.node.cmd_ptr;
                
                // left arm line
                if(selectedObject.node.prev_cp_ptr){
                    pesVec2f p1 = *selectedObject.node.prev_cp_ptr;
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                // right arm line
                if(selectedObject.node.next_cp_ptr){
                    pesVec2f p1 = *selectedObject.node.next_cp_ptr;
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                
                // selected node
                auto type = selectedObject.node.prev_cp_ptr ? pesPath::Command::_bezierTo : pesPath::Command::_lineTo;
                if(type==pesPath::Command::_moveTo || type==pesPath::Command::_lineTo){
                    SkRect rect = makeRectFromCenter(p.x, p.y, s, s);
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 255, 255, 0);
                    canvas.drawRect(rect, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawRect(rect, paint);
                    //paint.setStyle(SkPaint::kStroke_Style);
                    //paint.setARGB(255, 0, 0, 255);
                    //rect = makeRectFromCenter(p.x, p.y, s+1, s+1);
                    //canvas.drawRect(rect, paint);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    paint.setColor(whitesmoke);
                    canvas.drawRect(rect, paint);
                    rect = makeRectFromCenter(p.x, p.y, s+4/doc_scale, s+4/doc_scale);
                    paint.setStrokeWidth(2/doc_scale);
                    canvas.drawRect(rect, paint);
                    paint.setStrokeWidth(1/doc_scale);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                else if(type==pesPath::Command::_bezierTo){
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 100, 148, 237);
                    canvas.drawCircle(p.x, p.y, r, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawCircle(p.x, p.y, r, paint);
                    //paint.setStyle(SkPaint::kStroke_Style);
                    //paint.setARGB(255, 0, 0, 255);
                    //canvas.drawCircle(p.x, p.y, r+1, paint);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    paint.setColor(whitesmoke);
                    canvas.drawCircle(p.x, p.y, r, paint);
                    paint.setStrokeWidth(2/doc_scale);
                    canvas.drawCircle(p.x, p.y, r + 2/doc_scale, paint);
                    paint.setStrokeWidth(1/doc_scale);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                
                // left arm node
                if(selectedObject.node.prev_cp_ptr){
                    pesVec2f p1 = *selectedObject.node.prev_cp_ptr;
//                    paint.setARGB(255, 0, 0, 0);
//                    canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 255, 0, 255);
                    canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
                // right arm node
                if(selectedObject.node.next_cp_ptr){
                    pesVec2f p1 = *selectedObject.node.next_cp_ptr;
//                    paint.setARGB(255, 0, 0, 0);
//                    canvas.drawLine(p.x, p.y, p1.x, p1.y, paint);
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setARGB(255, 255, 0, 255);
                    canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setStyle(SkPaint::kStroke_Style);
                    // add details
                    //paint.setARGB(255, 0, 0, 0);
                    //canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setColor(whitesmoke);
                    paint.setBlendMode(SkBlendMode::kDifference);
                    canvas.drawCircle(p1.x, p1.y, r, paint);
                    paint.setBlendMode(SkBlendMode::kSrcOver);
                }
            }
            
            skpath.reset();
        }
        
        // mouse hover node
        if(hoverCommand!=NULL){
            auto type = hoverCommand->type;
            pesVec2f p = hoverCommand->to;
            if(type==pesPath::Command::_moveTo || type==pesPath::Command::_lineTo){
                SkRect rect = makeRectFromCenter(p.x, p.y, s+4/doc_scale, s+4/doc_scale);
                paint.setStyle(SkPaint::kFill_Style);
                paint.setARGB(64, 255, 0, 0);
                canvas.drawRect(rect, paint);
                paint.setStyle(SkPaint::kStroke_Style);
                paint.setStrokeWidth(1/doc_scale);
                paint.setARGB(255, 255, 255, 255);
                canvas.drawRect(rect, paint);
            }
            else if(type==pesPath::Command::_bezierTo){
                paint.setStyle(SkPaint::kFill_Style);
                paint.setARGB(64, 255, 0, 0);
                canvas.drawCircle(p.x, p.y, r + 2/doc_scale, paint);
                paint.setStyle(SkPaint::kStroke_Style);
                paint.setStrokeWidth(1/doc_scale);
                paint.setARGB(255, 255, 255, 255);
                canvas.drawCircle(p.x, p.y, r + 2/doc_scale, paint);
            }
        }
    }
    canvas.restore();
}

bool PesPathEdit::canUndo(){
    return undoRedo.canUndo();
}

bool PesPathEdit::canRedo(){
    return undoRedo.canRedo();
}
void PesPathEdit::undo(){
    if(canUndo()){
        undoRedo.undo();
    }
    PES5_UpdateUndoRedoTooltips();
}

void PesPathEdit::redo(){
    if(canRedo()){
        undoRedo.redo();
    }
    PES5_UpdateUndoRedoTooltips();
}

void PesPathEdit::pushCommand(Command command){
    undoRedo.pushCommand(std::make_shared<Command>(command));
    PES5_UpdateUndoRedoTooltips();
}

std::string PesPathEdit::getRedoName(){
    return undoRedo.getRedoName();
}

std::string PesPathEdit::getUndoName(){
    return undoRedo.getUndoName();
}


// Stitch Edit
PesStitchEdit::PesStitchEdit(){
    reset();
}

void PesStitchEdit::reset(){
    pesData = NULL;
    fillBlocks.clear();
    strokeBlocks.clear();
    selectedNode = NULL;
    selectedBlock = NULL;
    selectedIndex = -1;
    hoveredNode = NULL;
    undoRedo.clear();
    PES5_UpdateUndoRedoTooltips();
}

SkRect PesStitchEdit::makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void PesStitchEdit::draw(SkCanvas & canvas){
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    
    canvas.save();
    {
        canvas.translate(center.x, center.y);
        canvas.rotate(rotateDegree);
        canvas.translate(-center.x, -center.y);
        SkPaint paint;
        paint.setStrokeWidth(2/doc_scale);
        paint.setAntiAlias(true);

        auto whitesmoke = SkColorSetARGB(0xff, 0xe7, 0xe7, 0xe7);
        
        // fill blocks
        for(auto & block : fillBlocks){
            std::vector<pesVec2f> & vertices = block.polyline.getVertices();
            pesColor c = block.color;
            paint.setARGB(0xff, c.r, c.g, c.b);
            // lines
            paint.setStrokeWidth(1/doc_scale);
            canvas.drawPoints(SkCanvas::kPolygon_PointMode, vertices.size(), (SkPoint*)vertices.data(), paint);
            // points
            paint.setStrokeWidth(4/doc_scale);
            canvas.drawPoints(SkCanvas::kPoints_PointMode, vertices.size(), (SkPoint*)vertices.data(), paint);
        }
        
        // stroke blocks
        for(auto & block : strokeBlocks){
            std::vector<pesVec2f> & vertices = block.polyline.getVertices();
            pesColor c = block.color;
            paint.setARGB(0xff, c.r, c.g, c.b);
            // lines
            paint.setStrokeWidth(1/doc_scale);
            canvas.drawPoints(SkCanvas::kPolygon_PointMode, vertices.size(), (SkPoint*)vertices.data(), paint);
            // points
            paint.setStrokeWidth(4/doc_scale);
            canvas.drawPoints(SkCanvas::kPoints_PointMode, vertices.size(), (SkPoint*)vertices.data(), paint);
        }
        
        // selected node
        if(selectedNode!=NULL){
            pesVec2f p = *selectedNode;
            paint.setColor(whitesmoke);
            paint.setStyle(SkPaint::kStroke_Style);
            paint.setStrokeWidth(2/doc_scale);
            canvas.drawCircle(p.x, p.y, r, paint);
        }
        
        // mouse hovered node
        if(hoveredNode!=NULL){
            pesVec2f p = *hoveredNode;
            paint.setStyle(SkPaint::kFill_Style);
            paint.setARGB(64, 255, 0, 0);
            canvas.drawCircle(p.x, p.y, r + 2/doc_scale, paint);
            paint.setStyle(SkPaint::kStroke_Style);
            paint.setStrokeWidth(1/doc_scale);
            paint.setARGB(255, 255, 255, 255);
            canvas.drawCircle(p.x, p.y, r + 2/doc_scale, paint);
        }
    }
    canvas.restore();
}

bool PesStitchEdit::canUndo(){
    return undoRedo.canUndo();
}

bool PesStitchEdit::canRedo(){
    return undoRedo.canRedo();
}
void PesStitchEdit::undo(){
    if(canUndo()){
        undoRedo.undo();
    }
    PES5_UpdateUndoRedoTooltips();
}

void PesStitchEdit::redo(){
    if(canRedo()){
        undoRedo.redo();
    }
    PES5_UpdateUndoRedoTooltips();
}

void PesStitchEdit::pushCommand(Command command){
    undoRedo.pushCommand(std::make_shared<Command>(command));
    PES5_UpdateUndoRedoTooltips();
}

std::string PesStitchEdit::getRedoName(){
    return undoRedo.getRedoName();
}

std::string PesStitchEdit::getUndoName(){
    return undoRedo.getUndoName();
}
