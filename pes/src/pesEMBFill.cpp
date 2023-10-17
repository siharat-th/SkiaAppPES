//
//  pesEMBFill.cpp
//  pesEngine
//
//  Created by SIHARAT THAMMAYA on 3/7/2562 BE.
//  Copyright © 2562 SIHARAT THAMMAYA. All rights reserved.
//

#include "pesEMBFill.hpp"
#include "pesUtility.hpp"
#include "pesDocument.hpp"

using namespace std;

#ifndef M_PI_2
//#define M_PI_2 3.14159265358979323846
#define M_PI_2 1.57079632679489661923132169164
#endif


pesEMBFill::pesEMBFill(){
    _sections.clear();
    _rowspacing = 10.0 / 2.5;
    _sewDirection = 0;
}

pesEMBFill::pesEMBFill(const pesPath& path)
:_shape(path)
{
    _sections.clear();
    _rowspacing = 10.0 / 2.5;
    _sewDirection = 0;
}

pesEMBFill::~pesEMBFill(){
    clear();
}

void pesEMBFill::clear(){
    _shape.clear();
    _shapeOutline.clear();
    _patternResized.clear();
    _grids.clear();
    _sections.clear();
}

void pesEMBFill::setPesPath(const pesPath& path){
    clear();
    _shape = path;
}

vector<pesPath> pesEMBFill::createTileGrid(float patternSizeInMM){
    int ppmm = 10;
    if(_shape.getOutline().size()){
        float gridSize = patternSizeInMM * ppmm;
        pesRectangle bound = _shape.getBoundingBox();
        
        pesVec2f direction(1, 0);
        //        direction.rotate(_param.patternDirection);
        
        pesVec2f normal = direction;
        normal.rotateRad((float)M_PI_2);
        
        pesVec2f center = bound.getCenter();
        pesPath dummy;
        dummy.setMode(pesPath::Mode::POLYLINES);
        dummy.rectangle(bound.x, bound.y, bound.width, bound.height);
        dummy.translate(-center.x, -center.y);
        //        dummy.rotate(_param.patternDirection, ofVec3f(0,0,1));
        dummy.translate(center.x, center.y);
        pesRectangle newBound = dummy.getOutline()[0].getBoundingBox();
        
        float start = newBound.getMinY();
        start -= center.y;
        
        float startx = newBound.getMinX();
        startx -= center.x;
        
        pesVec2f lt = center + normal*start - direction*fabs(startx);
        pesVec2f rt = center + normal*start + direction*fabs(startx);
        pesVec2f lb = center - normal*start - direction*fabs(startx);
//        pesVec2f rb = center - normal*start + direction*fabs(startx);
        
        pesPath grid;
        pesVec2f p(0,0);
        grid.moveTo(p.x, p.y);
        p+=direction*gridSize;
        grid.lineTo(p.x, p.y);
        p+=normal*gridSize;
        grid.lineTo(p.x, p.y);
        p-=direction*gridSize;
        grid.lineTo(p.x, p.y);
        grid.close();
        
        _grids.clear();
        
        float w = lt.distance(rt);
        float h = lt.distance(lb);
        int numcol = (int)ceil(w / gridSize);
        int numrow = (int)ceil(h / gridSize);
        
        grid.translate(lt.x, lt.y);
        for(int row=0;row<numrow;row++){
            for(int col=0;col<numcol;col++){
                pesPath g = grid;
                pesVec2f t(0,0);
                t+=direction*(gridSize*col);
                t+=normal*(gridSize*row);
                g.translate(t.x, t.y);
                _grids.push_back(g);
            }
        }
    }

    return _grids;
}

deque<vector<pesEMBFill::Line>> pesEMBFill::createSections(float density, float sewDirection, float compensate, bool fading){
    _rowspacing = 10.0f / density;
    _sewDirection = sewDirection;
    vector<vector<pesEMBFill::Line>> rows_of_segments = intersect_region_with_grating(_rowspacing, sewDirection, compensate, fading);
    _sections = pull_runs(rows_of_segments);
    return _sections;
}

deque<vector<pesEMBFill::Line>> pesEMBFill::createMFSections(float rowspacingInMM, float sewDirection, float compensate){
    _rowspacing = rowspacingInMM * 10;
    _sewDirection = sewDirection;
    vector<vector<pesEMBFill::Line>> rows_of_segments = intersect_region_with_grating(_rowspacing, sewDirection, compensate);
    _sections = pull_runs(rows_of_segments);
    return _sections;
}

float snapToGrid(float v, float gridSize){
    // https://stackoverflow.com/questions/12136032/align-object-position-to-grid
    return std::floor(v/gridSize) * gridSize;
}

deque<vector<pesEMBFill::Line>> pesEMBFill::createCrossStitchSections(float rowspacingInMM, float sewDirection, float compensate){
    _rowspacing = rowspacingInMM * 10;
    _sewDirection = sewDirection;
    vector<vector<pesEMBFill::Line>> rows_of_segments = intersect_region_with_grating(_rowspacing, 0, compensate);
    _sections = pull_runs(rows_of_segments);
    
    // snap to grid
    for(auto & section : _sections){
        for(auto & line : section){
            line.start.x = snapToGrid(line.start.x, _rowspacing);
//            line.start.y = snapToGrid(line.start.y, _rowspacing);
            line.end.x = snapToGrid(line.end.x, _rowspacing);
//            line.end.y = snapToGrid(line.end.y, _rowspacing);
        }
    }
    
    return _sections;
}

vector<pesPolyline> pesEMBFill::toPatches(const pesEMBFill::Preset & preset, std::vector<bool> & jumps, bool fading){
    if(_sections.empty())
        createSections(10.0f/_rowspacing, _sewDirection, fading);
    pesVec2f _lastStitch(0,0);
    _preset = preset;
    float max_stitch_length = 5 * 10.0;
    
    vector<pesPolyline> patches;
    
    deque<vector<pesEMBFill::Line>> copy = _sections;
    
    while(_sections.size()){
        if(_lastStitch.length()>0){
            pesVec2f start_corner;
            int section_index = find_nearest_section(_sections, _lastStitch, start_corner);
            patches.push_back( connect_points(_lastStitch, start_corner, jumps));
            
            vector<Line> section = _sections.at(section_index);
            patches.push_back( section_from_corner( section, start_corner, _sewDirection, _rowspacing, max_stitch_length, preset, jumps));
            _sections.erase(_sections.begin()+section_index);
        }
        else{
            vector<Line> section = _sections.front();
            patches.push_back(section_to_patch(section, _sewDirection, _rowspacing, max_stitch_length, preset, jumps));
            _sections.pop_front();
        }
        
        pesVec2f last = patches.back().getVertices().back();
        _lastStitch = last;
    }
    
    _sections = copy;
    copy.clear();
    return patches;
}

vector<pesPolyline> pesEMBFill::toPatches(const pesPath & patternResized, std::vector<bool> & jumps, bool fading){
    if(_sections.empty())
        createSections(10.0f/_rowspacing, _sewDirection, fading);
    pesVec2f _lastStitch(0,0);
    _patternResized = patternResized;
    
    std::vector<pesPolyline> patches;
    
    std::deque<std::vector<pesEMBFill::Line>> copy = _sections;

    while(_sections.size()){
        if(_lastStitch.length()>0){
            pesVec2f start_corner;
            int section_index = find_nearest_section(_sections, _lastStitch, start_corner);
            patches.push_back( connect_points(_lastStitch, start_corner, jumps));
            
            std::vector<Line> section = _sections.at(section_index);
            patches.push_back( section_from_corner( section, start_corner, _sewDirection, _rowspacing, jumps));
            _sections.erase(_sections.begin()+section_index);
        }
        else{
            std::vector<Line> section = _sections.front();
            patches.push_back(section_to_patch(section, _sewDirection, _rowspacing, jumps));
            _sections.pop_front();
        }
        
        pesVec2f last = patches.back().getVertices().back();
        _lastStitch = last;
    }
    
    _sections = copy;
    copy.clear();
    return patches;
}

pesStitchBlock pesEMBFill::toStitches(const pesEMBFill::Preset & preset){
    if(_sections.empty())
        createSections(10.0f/_rowspacing, _sewDirection);
    pesVec2f _lastStitch(0,0);
    _preset = preset;
    float max_stitch_length = 5 * 10.0;
    
    pesStitchBlock stitches;
    
    deque<vector<pesEMBFill::Line>> copy = _sections;
    
    while(_sections.size()){
        if(_lastStitch.length()>0){
            pesVec2f start_corner;
            int section_index = find_nearest_section(_sections, _lastStitch, start_corner);
            
//            patches.push_back( connect_points(_lastStitch, start_corner));
            connect_points_stitches(_lastStitch, start_corner, stitches);
            
            vector<Line> section = _sections.at(section_index);
//            patches.push_back( section_from_corner( section, start_corner, _sewDirection, _rowspacing, max_stitch_length, preset));
            section_from_corner_stitches( section, start_corner, _sewDirection, _rowspacing, max_stitch_length, preset, stitches);
            _sections.erase(_sections.begin()+section_index);
        }
        else{
            vector<Line> section = _sections.front();
//            patches.push_back(section_to_patch(section, _sewDirection, _rowspacing, max_stitch_length, preset));
            section_to_patch_stitches(section, _sewDirection, _rowspacing, max_stitch_length, preset, stitches);
            _sections.pop_front();
        }
        
        pesVec2f last = stitches.polyline.getVertices().back();
        _lastStitch = last;
    }
    
    _sections = copy;
    copy.clear();
    return stitches;
}

pesStitchBlock pesEMBFill::toStitches(const pesPath & patternResized){
    if(_sections.empty())
        createSections(10.0f/_rowspacing, _sewDirection);
    pesVec2f _lastStitch(0,0);
    _patternResized = patternResized;
    
//    std::vector<pesPolyline> patches;
    pesStitchBlock stitches;
    
    std::deque<std::vector<pesEMBFill::Line>> copy = _sections;

    while(_sections.size()){
        if(_lastStitch.length()>0){
            pesVec2f start_corner;
            int section_index = find_nearest_section(_sections, _lastStitch, start_corner);
//            patches.push_back( connect_points(_lastStitch, start_corner));
            connect_points_stitches(_lastStitch, start_corner, stitches);
            
            std::vector<pesEMBFill::Line> section = _sections.at(section_index);
//            patches.push_back( section_from_corner( section, start_corner, _sewDirection, _rowspacing));
            section_from_corner_stitches( section, start_corner, _sewDirection, _rowspacing, stitches);
            _sections.erase(_sections.begin()+section_index);
        }
        else{
            std::vector<Line> section = _sections.front();
//            patches.push_back(section_to_patch(section, _sewDirection, _rowspacing));
            section_to_patch_stitches(section, _sewDirection, _rowspacing, stitches);
            _sections.pop_front();
        }
        
//        pesVec2f last = patches.back().getVertices().back();
        pesVec2f last = stitches.polyline.getVertices().back();
        _lastStitch = last;
    }
    
    _sections = copy;
    copy.clear();
//    return patches;
    return stitches;
}

int pesEMBFill::find_nearest_section(deque<vector<pesEMBFill::Line>> sections, pesVec2f last_stitch, pesVec2f &start_corner){
    
    int nearestCorner = -1;
    int nearestSection = -1;
    float maxdist = INFINITY;
    for(int i=0; i<(int)sections.size(); i++){
        std::vector<pesEMBFill::Line> section = sections[i];
        std::vector<pesVec2f> corners = get_corner_points(section);
        float dist[4];
        dist[0] = fabs(perimeter_distance(corners[0], last_stitch));
        dist[1] = fabs(perimeter_distance(corners[1], last_stitch));
        dist[2] = fabs(perimeter_distance(corners[2], last_stitch));
        dist[3] = fabs(perimeter_distance(corners[3], last_stitch));
        
        for(int j=0; j<4; j++){
            if(dist[j]<maxdist){
                maxdist = dist[j];
                nearestSection = i;
                nearestCorner = j;
            }
        }
    }
    
    start_corner = get_corner_points(sections[nearestSection])[nearestCorner];
    return nearestSection;
}

pesPolyline pesEMBFill::connect_points(pesVec2f p1, pesVec2f p2, std::vector<bool> & jumps){
    int ppmm = 10;
    float running_stitch_length = 1.0f*ppmm; // 1mm
    
    pesPolyline patch;
    pesPolyline & outline = _shapeOutline;
    
    unsigned int nearestIndex;
    outline.getClosestPoint(p1, &nearestIndex);
    float fpos = outline.getLengthAtIndex(nearestIndex);
    
    float distance = perimeter_distance(p1, p2);
    
    int stitches = abs(int(distance / running_stitch_length));
    
    float direction = copysign(1.0f, distance);
    float one_stitch = running_stitch_length * direction;
    
    float outline_length = outline.getPerimeter();
    for(int i=0; i<stitches; i++){
        
        fpos = fpos + one_stitch;
        if(fpos>outline_length){
            fpos = fpos - outline_length;
        }
        else if(fpos<0.0){
            fpos = outline_length - fpos;
        }
        
        pesVec2f stitch = outline.getPointAtLength(fpos);
        patch.addVertex(stitch);
        if(i==0)
            jumps.push_back(false); // Jimmy
        else
            jumps.push_back(false);
    }
    return patch;
}

void pesEMBFill::connect_points_stitches(pesVec2f p1, pesVec2f p2, pesStitchBlock & stitches){
    int ppmm = 10;
    float running_stitch_length = 1.0f*ppmm; // 1mm
    
    pesPolyline & outline = _shapeOutline;
    
    unsigned int nearestIndex;
    outline.getClosestPoint(p1, &nearestIndex);
    float fpos = outline.getLengthAtIndex(nearestIndex);
    
    float distance = perimeter_distance(p1, p2);
    
    int numStitches = abs(int(distance / running_stitch_length));
    
    float direction = copysign(1.0f, distance);
    float one_stitch = running_stitch_length * direction;
    
    float outline_length = outline.getPerimeter();
    for(int i=0; i<numStitches; i++){
        
        fpos = fpos + one_stitch;
        if(fpos>outline_length){
            fpos = fpos - outline_length;
        }
        else if(fpos<0.0){
            fpos = outline_length - fpos;
        }
        
        pesVec2f stitch = outline.getPointAtLength(fpos);
        
        if(i==0){
            stitches.addJump(stitch);
        }
        else{
            stitches.addStitch(stitch);
        }
    }
}


static const int __ccw = -1;
static const int __cw = 1;
std::vector<std::vector<pesEMBFill::Line>> pesEMBFill::intersect_region_with_grating(float _row_spacing, float degree, float compensate, bool fading){
    
    float row_spacing = _row_spacing;
    int outline_size = (int)_shape.getOutline().size();
    int first_outer_dir = _shape.getOutline()[0].getArea()>=0 ? __cw : __ccw;
    vector<pesPolyline> outlines = _shape.getOutline();
    
    _shapeOutline.clear();
    for(int i=0;i<(int)outlines.size();i++){
        int dir = outlines[i].getArea()>=0 ? __cw : __ccw;
        if(dir==first_outer_dir){
            _shapeOutline.addVertices(outlines[i].getVertices());
            _shapeOutline.close();
        }
    }
    _shapeOutline.simplify();
    
    int numContourGroups = 0;
    for(int i=0; i<outline_size; i++){
        int dir = outlines[i].getArea()>=0 ? __cw : __ccw;
        if(dir==first_outer_dir)
            numContourGroups++;
    }
    std::vector<std::vector<pesPolyline>> contourGroups;
    contourGroups.resize(numContourGroups);
    int groupIndex=0;
    for(int i=0; i<outline_size; i++){
        int dir = outlines[i].getArea()>=0 ? __cw : __ccw;
        if(dir==first_outer_dir){
            contourGroups[groupIndex++].push_back(outlines[i]);
        }
    }
    
    for(int i=0; i<outline_size; i++){
        int dir = outlines[i].getArea()>=0 ? __cw : __ccw;
        if(dir!=first_outer_dir){
            pesVec2f centroid = outlines[i].getCentroid2D();
            for(int j=0;j<(int)contourGroups.size();j++){
                if(contourGroups[j][0].getBoundingBox().inside(centroid.x, centroid.y))
                    contourGroups[j].push_back(outlines[i]);
            }
        }
    }
    
    vector<vector<Line>> rows; // ret
    for(int ii=0; ii<numContourGroups; ii++)
        //    for(int ii=numContourGroups-1; ii>=0; ii--)
    {
        std::vector<pesPolyline> contours = contourGroups[ii];
        std::vector<int> contourDir;
        for(int i=0; i<(int)contours.size(); i++){
            contours[i].simplify();
        }
        
        pesRectangle bound = contours[0].getBoundingBox();
        for(int i=1; i<(int)contours.size(); i++){
            bound.growToInclude(contours[i].getBoundingBox());
        }
        
        if(contours.size()>1){
            contourDir.resize(contours.size());
            for(int i=0; i<(int)contours.size(); i++){
                
                if(contours[i].getArea()>=0){
//                    GUI_LogNotice("contours", "%d is cw area=%g", i, contours[i].getArea());
                    contourDir[i] = __cw;
                }
                else{
//                    GUI_LogNotice("contours", "%d is ccw area=%g", i, contours[i].getArea());
                    contourDir[i] = __ccw;
                }
            }
        }
        
        pesVec2f upper_left(bound.getMinX(), bound.getMinY());
        pesVec2f lower_right(bound.getMaxX(), bound.getMaxY());
        float length = (upper_left - lower_right).length();
        float half_length = length / 2.0f;
        
        pesVec2f direction(1, 0);
        direction.rotate(degree);
        
        pesVec2f normal = direction;
        normal.rotateRad((float)M_PI_2);
        
        pesVec2f center = bound.getCenter();
        
        pesPath dummy;
        dummy.setMode(pesPath::Mode::POLYLINES);
        dummy.rectangle(bound.x, bound.y, bound.width, bound.height);
        dummy.translate(-center.x, -center.y);
        dummy.rotate(degree);
        dummy.translate(center.x, center.y);
        pesRectangle newBound = dummy.getOutline()[0].getBoundingBox();
        float start = newBound.getMinY();
        float end = newBound.getMaxY();
        
        start -= center.y;
        end -= center.y;
        
        // pom - fade
        float start_y = start;
        float end_y = end;
        float length_y = end_y - start_y;
        float start_fade = start + length_y * 0.4;
        float max_row_spacing = row_spacing * 5;
        
        start -= fmod(start + normal.y * center.y, row_spacing);
        //        if(compensate!=0.0){
        //            start+=compensate;
        //            end-=compensate;
        //        }
        
        int lineNumber=0;
        while(start<end){
            pesVec2f p00 = center + normal*start - direction*half_length;
            pesVec2f p01 = center + normal*start + direction*half_length;
            
            pesEMBFill::Line grating_line(p00, p01);
            
            std::vector<Line> res;
            std::vector<pesVec2f> ret;
            int numSegment = (int)contours.size();
            for(int i=0; i<numSegment; i++){
                if(contours[i].isClosed()){
                    auto last = contours[i].getVertices().back();
                    if(last.x!=contours[i][0].x || last.y!=contours[i][0].y)
                        contours[i].addVertex(contours[i][0]);
                }
                auto vertices = contours[i].getVertices();
                for(int j=1; j<(int)vertices.size(); j++){
                    pesVec2f p0 = vertices[j-1];
                    pesVec2f p1 = vertices[j];
                    
                    pesVec2f out;
                    bool intersect = pesLineSegmentIntersection(grating_line.start, grating_line.end, p0, p1, out);
                    if(intersect){
                        ret.push_back(out);
                    }
                }
            }
            
            if(ret.size()>=2){
                
                pesSort(ret, [=](pesVec2f &a, pesVec2f &b)->bool{
                    if(p00.distance(a) < p00.distance(b)){
                        return true;
                    }
                    else{
                        return false;
                    }
                });
                
                
                //            GUI_LogNotice("intersect w/ grating line", "size=%ld", ret.size());
                for(int k=1; k<(int)ret.size(); k++){
                    pesVec2f p0 = ret[k-1];
                    pesVec2f p1 = ret[k];
                    pesVec2f middle = p0.getMiddle(p1);
                    
                    if(contours.size()==1){
                        if(contours[0].inside(middle)){
                            // Jimmy - compensate
                            p1 = p1 + (p1-p0).normalize() * compensate * 10;
                            p0 = p0 - (p1-p0).normalize() * compensate * 10;                            
                            pesEMBFill::Line line(p0, p1);
                            line.lineNumber = lineNumber;
                            res.push_back(line);
                        }
                    }
                    else if(contours.size()>1){ // (PolyWindingMode::POLY_WINDING_ODD)
                        int outer_dir = contourDir[0];
                        bool inside_hole = false;
                        // check with hole
                        for(int i=(int)contours.size()-1; i>=1; i--){
                            if(contourDir[i]!=outer_dir){
                                // this is a hole
                                if(contours[i].inside(middle)){
                                    inside_hole = true;
                                    break;
                                }
                            }
                        }
                        
                        // check with contour
                        if(inside_hole==false){
                            for(int i=(int)contours.size()-1; i>=0; i--){
                                if(contourDir[i]==outer_dir){
                                    // this is a contour
                                    if(contours[i].inside(middle)){
                                        // Jimmy - compensate
                                        p1 = p1 + (p1-p0).normalize() * compensate * 10;
                                        p0 = p0 - (p1-p0).normalize() * compensate * 10;
                                        pesEMBFill::Line line(p0, p1);
                                        line.lineNumber = lineNumber;
                                        res.push_back(line);
                                    }
                                }
                            }
                        }
                    }
                }
                
            }
            
//            fading = true;
            float cur_row_spacing = row_spacing;
            if(fading){
                if(start>=start_fade){
                    cur_row_spacing = pesMap(start, start_fade, length_y, row_spacing, max_row_spacing);
                }
            }
            
            if(res.empty()){
                start += cur_row_spacing;
                lineNumber++;
                //            GUI_LogNotice("not intersect w/ grating line");
                continue;
            }
            
            std::vector<Line> runs = res;
            pesSort(runs, [=](Line &a, Line &b)->bool{
                if(upper_left.distance(a.start) < upper_left.distance(b.start)){ //if(upper_left.distance(a.start) > upper_left.distance(b.start)){
                    return true;
                }
                else{
                    return false;
                }
            });
            
            
            rows.push_back(runs);
            //        GUI_LogNotice("intersect w/ grating line", "rows=%ld runs=%ld", rows.size(), runs.size());
            start += cur_row_spacing;
            lineNumber++;
        }// end while
        
        contours.clear();
        
    } // end for(numContourGroup)
    
    return rows;
    
}

pesEMBFill::Line __embNoneLine = pesEMBFill::Line(pesVec3f::zero(), pesVec3f::zero());
deque<vector<pesEMBFill::Line>> pesEMBFill::pull_runs(vector<vector<pesEMBFill::Line>> &rows){
    std::deque<std::vector<Line>> runs;
    int count = 0;
    
    while(rows.size()>0){
        std::vector<Line> run;
        Line prev = __embNoneLine;
        bool prevIsNone = true;
        
        int size = (int)rows.size();
        
        for(int row_num=0; row_num<size; row_num++){
            vector<Line> row = rows[row_num];
            Line first = row[0];
            
            if(prevIsNone==false && is_same_run(prev, first)==false){
                break;
            }
            
            run.push_back(first);
            prev = first;
            prevIsNone = false;
            rows[row_num].erase(rows[row_num].begin());
            
        }// end for
        
        runs.push_back(run);
        
        pesRemove(rows, [=](vector<Line> &row)->bool{
            return row.empty();
        });
        
        count+=1;
        
    }// end while
    return runs;
}

bool pesEMBFill::is_same_run(const pesEMBFill::Line &segment1, const pesEMBFill::Line &segment2){
    float max_stitch_length = 5 * 10;
    if(segment1.start.distance(segment2.start)>max_stitch_length)
        return false;
    
    if(segment1.end.distance(segment2.end)>max_stitch_length)
        return false;
    
    return true;
}

vector<pesVec2f> pesEMBFill::get_corner_points(vector<pesEMBFill::Line> section){
    pesVec2f corner[4] = {section[0].start, section[0].end, section.back().start, section.back().end};
    std::vector<pesVec2f> c;
    c.resize(4);
    c[0] = corner[0];
    c[1] = corner[1];
    c[2] = corner[2];
    c[3] = corner[3];
    return c;
}

float pesEMBFill::perimeter_distance(pesVec2f p1, pesVec2f p2){
    //    # how far around the perimeter (and in what direction) do I need to go
    //    # to get from p1 to p2?
    
    pesPolyline &outline = _shapeOutline;
    
    unsigned int nearestIndex_p1, nearestIndex_p2;
    outline.getClosestPoint(p1, &nearestIndex_p1);
    outline.getClosestPoint(p2, &nearestIndex_p2);
    
    float fpos_p1 = outline.getLengthAtIndex(nearestIndex_p1);
    float fpos_p2 = outline.getLengthAtIndex(nearestIndex_p2);
    
    //        float distance = (p2_projection - p1_projection).length();
    float distance = fpos_p2 - fpos_p1;
    float outline_length = outline.getPerimeter();
    
    if (fabs(distance) > outline_length / 2.0){
        //    # if we'd have to go more than halfway around, it's faster to go
        //    # the other way
        if (distance < 0)
            return distance + outline_length;
        else if (distance > 0)
            return distance - outline_length;
        else
            //    # this ought not happen, but just for completeness, return 0 if
            //    # p1 and p0 are the same point
            return 0;
    }
    else
        return distance;
}

pesVec2f east(float angle){
    // "east" is the name of the direction that is to the right along a row
    //    return ofPoint(1, 0).rotate(-angle, ofVec3f(0,0,1));
    return pesVec2f(1, 0).rotate(angle);
}

pesPolyline pesEMBFill::section_to_patch(vector<Line> &group_of_segments, float angle, float row_spacing, float max_stitch_length, const Preset& preset, std::vector<bool> & jumps){
    pesPolyline patch;
    
//    bool first_segment = true;
    bool swap = false;
    pesVec2f last_end(0,0);
    
    int pixels_per_mm = 10;
    int num_stagger = (int)preset.row_stagger.size();
    
    if(num_stagger>0){
        std::vector<float> stagger;
        for(auto s : preset.row_stagger){
            stagger.push_back(s*pixels_per_mm);
        }
        
        for(auto segment : group_of_segments){
            
            pesVec2f beg = segment.start;
            pesVec2f end = segment.end;
            if(swap){
                std::swap(beg, end);
            }
            
            pesVec2f row_direction = (end-beg).normalize();
            float segment_length = end.distance(beg);
            
            // only stitch the first point if it's a reasonable distance away from the last stitch
            //            if(last_end.length()==0 || beg.distance(last_end)>=0.3*pixels_per_mm){
            //                patch.addStitch(beg);
            //            }
//            if(last_end.length()==0 || beg.distance(last_end)>=row_spacing*0.9){
//                patch.addVertex(beg);
//            }
            const float unit_per_mm = 10;
            if(last_end.length()==0){
                jumps.push_back(true);
            }
            else if(beg.distance(last_end)>pesGetDocument()->getTrimAtLength() * unit_per_mm){ // Jimmy - fill end before expected
                jumps.push_back(true);
            }
            else{
                jumps.push_back(false);
            }
            patch.addVertex(beg);
            
            int row_num = segment.lineNumber%num_stagger;
            float stagger_offset = stagger[row_num];
            float stitch_offset = fmodf(beg.dot(east(angle)) - stagger_offset , max_stitch_length);
            pesVec2f first_stitch = beg - stitch_offset * east(angle);
            
            // we might have chosen our first stitch just outside this row, so move back in
            //        if(((first_stitch - beg) * row_direction).x < 0)
            if((first_stitch - beg).dot(row_direction) < 0)
                first_stitch += row_direction * max_stitch_length;
            
            float offset = (first_stitch - beg).length();
            
            while(offset < segment_length){
                patch.addVertex(beg + offset * row_direction);
                jumps.push_back(false);
                offset += max_stitch_length;
            }
            
            if ((end - patch.getVertices().back()).length() >= 0.1 * pixels_per_mm){
                patch.addVertex(end);
                jumps.push_back(false);
            }
            
            last_end = end;
            swap = !swap;
        }
    }
    
    return patch;
}

void pesEMBFill::section_to_patch_stitches(vector<Line> &group_of_segments, float angle, float row_spacing, float max_stitch_length, const Preset& preset, pesStitchBlock & stitches){
//    pesPolyline patch;
    
//    bool first_segment = true;
    bool swap = false;
    pesVec2f last_end(0,0);
    
    int pixels_per_mm = 10;
    int num_stagger = (int)preset.row_stagger.size();
    bool firstStitches = true;
    
    if(num_stagger>0){
        std::vector<float> stagger;
        for(auto s : preset.row_stagger){
            stagger.push_back(s*pixels_per_mm);
        }
        
        for(auto segment : group_of_segments){
            
            pesVec2f beg = segment.start;
            pesVec2f end = segment.end;
            if(swap){
                std::swap(beg, end);
            }
            
            pesVec2f row_direction = (end-beg).normalize();
            float segment_length = end.distance(beg);
            
            // only stitch the first point if it's a reasonable distance away from the last stitch
            //            if(last_end.length()==0 || beg.distance(last_end)>=0.3*pixels_per_mm){
            //                patch.addStitch(beg);
            //            }
//            if(last_end.length()==0 || beg.distance(last_end)>=row_spacing*0.9){
////                patch.addVertex(beg);
////                if(firstStitches){
//                    stitches.addJump(beg);
//                    firstStitches = false;
////                }
////                else{
////                    stitches.addStitch(beg);
////                }
//            }
            
            if(last_end.length()==0){
                stitches.addJump(beg);
                firstStitches = false;
            }
            else if(beg.distance(last_end)>row_spacing*2){
                stitches.addJump(beg);
                firstStitches = false;
            }
            else{
                stitches.addStitch(beg);
            }
            
            int row_num = segment.lineNumber%num_stagger;
            float stagger_offset = stagger[row_num];
            float stitch_offset = fmodf(beg.dot(east(angle)) - stagger_offset , max_stitch_length);
            pesVec2f first_stitch = beg - stitch_offset * east(angle);
            
            // we might have chosen our first stitch just outside this row, so move back in
            //        if(((first_stitch - beg) * row_direction).x < 0)
            if((first_stitch - beg).dot(row_direction) < 0)
                first_stitch += row_direction * max_stitch_length;
            
            float offset = (first_stitch - beg).length();
            
            while(offset < segment_length){
//                patch.addVertex(beg + offset * row_direction);
                if(firstStitches){
                    stitches.addJump(beg + offset * row_direction);
                    firstStitches = false;
                }
                else{
                    stitches.addStitch(beg + offset * row_direction);
                }
                offset += max_stitch_length;
            }
            
//            if ((end - patch.getVertices().back()).length() >= 0.1 * pixels_per_mm)
//                patch.addVertex(end);
            if ((end - stitches.polyline.getVertices().back()).length() >= 0.1 * pixels_per_mm){
                if(firstStitches){
                    stitches.addJump(end);
                    firstStitches = false;
                }
                else{
                    stitches.addStitch(end);
                }
            }
            
            last_end = end;
            swap = !swap;
        }
    }
}


pesPolyline pesEMBFill::section_to_patch(vector<Line> &group_of_segments, float angle, float row_spacing, std::vector<bool> & jumps){
    
    pesPolyline patch;
//    bool first_segment = true;
    bool swap = false;
    pesVec2f last_end(0,0);
    
    int ppmm = 10;
    
    for(auto segment : group_of_segments){
        
        pesVec2f beg = segment.start;
        pesVec2f end = segment.end;
        if(swap){
            std::swap(beg, end);
        }
        
//        pesVec2f row_direction = (end-beg).normalize();
//        float segment_length = end.distance(beg);
        
        // only stitch the first point if it's a reasonable distance away from the last stitch
//        if(last_end.length()==0 || beg.distance(last_end)>=row_spacing*0.9){
//            patch.addVertex(beg);
//        }
        const float unit_per_mm = 10;
        if(last_end.length()==0){
            jumps.push_back(true);
        }
        else if(beg.distance(last_end)>pesGetDocument()->getTrimAtLength() * unit_per_mm){
            jumps.push_back(true);
        }
        else{
            jumps.push_back(false);
        }
        patch.addVertex(beg);
        
        vector<pesPath> acc_grids;
        for(auto grid : _grids){
            auto & vertices = grid.getOutline().front().getVertices();
            int n = (int)vertices.size();
            if(n>1){
                pesVec2f result;
                bool hasIntersect = false;
                for(int i=1; i<n; i++){
                    pesVec2f p0 = vertices[i-1];
                    pesVec2f p1 = vertices[i];
                    bool b = pesLineSegmentIntersection(beg, end, p0, p1, result);
                    if(b){
                        acc_grids.push_back(grid);
                        hasIntersect = true;
                        break;
                    }
                }
                
                if(!hasIntersect){
                    if(grid.inside(beg.x, beg.y)||grid.inside(end.x, end.y)){
                        acc_grids.push_back(grid);
                    }
                }
            }
        }
        if(acc_grids.size()){
            pesEMBFill::Line line(beg, end);
            vector<pesVec2f> emb_points;
            for(auto acc_grid : acc_grids){
                auto v = acc_grid.getOutline()[0].getVertices()[0];
                _patternResized.translate(v.x, v.y);
                auto points = intersect(_patternResized, line);
                _patternResized.translate(-v.x, -v.y);
                if(points.size()){
                    emb_points.insert(emb_points.end(), points.begin(), points.end());
                    points.clear();
                }
            }
            
            
            if(emb_points.size()>1){
                pesSort(emb_points, [=](pesVec2f &a, pesVec2f &b)->bool{
                    if(line.start.distance(a) < line.start.distance(b)){
                        return true;
                    }
                    else{
                        return false;
                    }
                });
                
                for(auto stitch : emb_points){
                    patch.addVertex(stitch);
                    jumps.push_back(false);
                }
                emb_points.clear();
            }
        }
        
        
        if ((end - patch.getVertices().back()).length() > 0.1 * ppmm){
            patch.addVertex(end);
            jumps.push_back(false);
        }
        
        last_end = end;
        swap = !swap;
    }
    
    return patch;
}

void pesEMBFill::section_to_patch_stitches(vector<Line> &group_of_segments, float angle, float row_spacing, pesStitchBlock & stitches){
    
//    pesPolyline patch;
//    bool first_segment = true;
    bool swap = false;
    pesVec2f last_end(0,0);
    
    int ppmm = 10;
    bool firstStitches = true;
    
    for(auto segment : group_of_segments){
        
        pesVec2f beg = segment.start;
        pesVec2f end = segment.end;
        if(swap){
            std::swap(beg, end);
        }
        
//        pesVec2f row_direction = (end-beg).normalize();
//        float segment_length = end.distance(beg);
        
        // only stitch the first point if it's a reasonable distance away from the last stitch
//        if(last_end.length()==0 || beg.distance(last_end)>=row_spacing*0.9){
////            patch.addVertex(beg);
//            if(firstStitches){
//                stitches.addJump(beg);
//                firstStitches = false;
//            }
//            else{
//                stitches.addStitch(beg);
//            }
//        }
        
        if(last_end.length()==0){
            stitches.addJump(beg);
            firstStitches = false;
        }
        else if(beg.distance(last_end)>row_spacing*2){
            stitches.addJump(beg);
            firstStitches = false;
        }
        else{
            stitches.addStitch(beg);
        }
        
        vector<pesPath> acc_grids;
        for(auto grid : _grids){
            auto & vertices = grid.getOutline().front().getVertices();
            int n = (int)vertices.size();
            if(n>1){
                pesVec2f result;
                bool hasIntersect = false;
                for(int i=1; i<n; i++){
                    pesVec2f p0 = vertices[i-1];
                    pesVec2f p1 = vertices[i];
                    bool b = pesLineSegmentIntersection(beg, end, p0, p1, result);
                    if(b){
                        acc_grids.push_back(grid);
                        hasIntersect = true;
                        break;
                    }
                }
                
                if(!hasIntersect){
                    if(grid.inside(beg.x, beg.y)||grid.inside(end.x, end.y)){
                        acc_grids.push_back(grid);
                    }
                }
            }
        }
        if(acc_grids.size()){
            pesEMBFill::Line line(beg, end);
            vector<pesVec2f> emb_points;
            for(auto acc_grid : acc_grids){
                auto v = acc_grid.getOutline()[0].getVertices()[0];
                _patternResized.translate(v.x, v.y);
                auto points = intersect(_patternResized, line);
                _patternResized.translate(-v.x, -v.y);
                if(points.size()){
                    emb_points.insert(emb_points.end(), points.begin(), points.end());
                    points.clear();
                }
            }
            
            
            if(emb_points.size()>1){
                pesSort(emb_points, [=](pesVec2f &a, pesVec2f &b)->bool{
                    if(line.start.distance(a) < line.start.distance(b)){
                        return true;
                    }
                    else{
                        return false;
                    }
                });
                
                for(auto stitch : emb_points){
//                    patch.addVertex(stitch);
                    if(firstStitches){
                        stitches.addJump(stitch);
                        firstStitches = false;
                    }
                    else{
                        stitches.addStitch(stitch);
                    }
                }
                emb_points.clear();
            }
        }
        
        
//        if ((end - patch.getVertices().back()).length() > 0.1 * ppmm)
//            patch.addVertex(end);
        
        if ((end - stitches.polyline.getVertices().back()).length() > 0.1 * ppmm){
//            patch.addVertex(end);
            if(firstStitches){
                stitches.addJump(end);
                firstStitches = false;
            }
            else{
                stitches.addStitch(end);
            }
        }
        
        last_end = end;
        swap = !swap;
    }
    
//    return patch;
}

pesPolyline pesEMBFill::section_from_corner( vector<pesEMBFill::Line> section, pesVec2f start_corner, float angle, float row_spacing, std::vector<bool> & jumps){
    
//    if(start_corner!=section[0].start && start_corner!=section[0].end){
//        std::reverse(section.begin(), section.end());
//    }
//
//    if(start_corner!=section[0].start){
//        for(int i=0; i<(int)section.size(); i++){
//            std::swap(section[i].start, section[i].end);
//        }
//    }
    
    if(!start_corner.match(section[0].start, 1) && !start_corner.match(section[0].end)){
        std::reverse(section.begin(), section.end());
    }

    if(!start_corner.match(section[0].start, 1)){
        for(int i=0; i<(int)section.size(); i++){
            std::swap(section[i].start, section[i].end);
        }
    }
    
    return section_to_patch(section, angle, row_spacing, jumps);
}

void pesEMBFill::section_from_corner_stitches( std::vector<pesEMBFill::Line> section, pesVec2f start_corner, float angle, float row_spacing, pesStitchBlock & stitches){
    
//    if(start_corner!=section[0].start && start_corner!=section[0].end){
//        std::reverse(section.begin(), section.end());
//    }
//
//    if(start_corner!=section[0].start){
//        for(int i=0; i<(int)section.size(); i++){
//            std::swap(section[i].start, section[i].end);
//        }
//    }
    
    if(!start_corner.match(section[0].start, 1) && !start_corner.match(section[0].end)){
        std::reverse(section.begin(), section.end());
    }

    if(!start_corner.match(section[0].start, 1)){
        for(int i=0; i<(int)section.size(); i++){
            std::swap(section[i].start, section[i].end);
        }
    }
    
    section_to_patch_stitches(section, angle, row_spacing, stitches);
}

pesPolyline pesEMBFill::section_from_corner( vector<pesEMBFill::Line> section, pesVec2f start_corner, float angle, float row_spacing, float max_stitch_length, const Preset& preset, std::vector<bool> & jumps){
    
//    if(start_corner!=section[0].start && start_corner!=section[0].end){
//        std::reverse(section.begin(), section.end());
//    }
//
//    if(start_corner!=section[0].start){
//        for(int i=0; i<(int)section.size(); i++){
//            std::swap(section[i].start, section[i].end);
//        }
//    }
    
    if(!start_corner.match(section[0].start, 1) && !start_corner.match(section[0].end)){
        std::reverse(section.begin(), section.end());
    }

    if(!start_corner.match(section[0].start, 1)){
        for(int i=0; i<(int)section.size(); i++){
            std::swap(section[i].start, section[i].end);
        }
    }
    
    return section_to_patch(section, angle, row_spacing, max_stitch_length, preset, jumps);
}

void pesEMBFill::section_from_corner_stitches( vector<pesEMBFill::Line> section, pesVec2f start_corner, float angle, float row_spacing, float max_stitch_length, const Preset& preset, pesStitchBlock & stitches){
    
//    if(start_corner!=section[0].start && start_corner!=section[0].end){
//        std::reverse(section.begin(), section.end());
//    }
//
//    if(start_corner!=section[0].start){
//        for(int i=0; i<(int)section.size(); i++){
//            std::swap(section[i].start, section[i].end);
//        }
//    }
    
    if(!start_corner.match(section[0].start, 1) && !start_corner.match(section[0].end)){
        std::reverse(section.begin(), section.end());
    }

    if(!start_corner.match(section[0].start, 1)){
        for(int i=0; i<(int)section.size(); i++){
            std::swap(section[i].start, section[i].end);
        }
    }
    
    return section_to_patch_stitches(section, angle, row_spacing, max_stitch_length, preset, stitches);
}

vector<pesVec2f> pesEMBFill::intersect(const pesPath & path, pesEMBFill::Line line){
    vector<pesVec2f> points;
    for(auto & outline : path.getOutline()){
        auto & vertices = outline.getVertices();
        int n = (int)vertices.size();
        if(n>1){
            pesVec2f result;
            for(int i=1; i<n; i++){
                pesVec2f p0 = vertices[i-1];
                pesVec2f p1 = vertices[i];
                bool b = pesLineSegmentIntersection(line.start, line.end, p0, p1, result);
                if(b)
                    points.push_back(result);
            }
        }
    }
    if(points.size()>1){
        pesSort(points, [=](pesVec2f &a, pesVec2f &b)->bool{
            if(line.start.distance(a) < line.start.distance(b)){
                return true;
            }
            else{
                return false;
            }
        });
    }
    return points;
}


// MARK: - pesEMBFillPreset
namespace pesEMBFillPreset{
    
    static pesEMBFillPreset::Preset __preset = pesEMBFillPreset::TRADITIONAL;
    void setDefaultPreset(pesEMBFillPreset::Preset preset){
        __preset = preset;
    }
    
    pesEMBFill::Preset getDefaultPreset(){
        switch (__preset) {
            case pesEMBFillPreset::TRADITIONAL:
                return pesEMBFillPreset::Traditional();
                
            case pesEMBFillPreset::BRICK:
                return pesEMBFillPreset::Brick();
                
            case pesEMBFillPreset::PILLAR:
                return pesEMBFillPreset::Pillar();
                
            case pesEMBFillPreset::ZIGZAG:
                return pesEMBFillPreset::Zigzag();
                
            case pesEMBFillPreset::WAVE:
                return pesEMBFillPreset::Wave();
                
            case pesEMBFillPreset::FAN:
                return pesEMBFillPreset::Fan();
                
            case pesEMBFillPreset::PATT01:
                return pesEMBFillPreset::P1();
                
            case pesEMBFillPreset::PATT02:
                return pesEMBFillPreset::P2();
                
            case pesEMBFillPreset::PATT03:
                return pesEMBFillPreset::P3();
                
            case pesEMBFillPreset::PATT04:
                return pesEMBFillPreset::P4();
                
            case pesEMBFillPreset::PATT05:
                return pesEMBFillPreset::P5();
                
            case pesEMBFillPreset::PATT06:
                return pesEMBFillPreset::P6();
                
            case pesEMBFillPreset::PATT07:
                return pesEMBFillPreset::P7();
                
            case pesEMBFillPreset::PATT08:
                return pesEMBFillPreset::P8();
                
            case pesEMBFillPreset::PATT09:
                return pesEMBFillPreset::P9();
                
            case pesEMBFillPreset::PATT10:
                return pesEMBFillPreset::P10();
                
            default:
                return pesEMBFillPreset::Traditional();
        }
    }
    
    
    
    pesEMBFill::Preset Traditional(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0);
        patt.row_stagger.push_back(1);
        patt.row_stagger.push_back(2);
        patt.row_stagger.push_back(3);
        return patt;
    }
    
    pesEMBFill::Preset Brick(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0);
        patt.row_stagger.push_back(2);
        return patt;
    }
    
    pesEMBFill::Preset Pillar(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0);
        return patt;
    }
    
    pesEMBFill::Preset Zigzag(){
        pesEMBFill::Preset pattern;
        pattern.row_stagger.push_back(0.0);
        pattern.row_stagger.push_back(0.5);
        pattern.row_stagger.push_back(1.0);
        pattern.row_stagger.push_back(1.5);
        pattern.row_stagger.push_back(2.0);
        pattern.row_stagger.push_back(2.5);
        pattern.row_stagger.push_back(3.0);
        pattern.row_stagger.push_back(2.5);
        pattern.row_stagger.push_back(2.0);
        pattern.row_stagger.push_back(1.5);
        pattern.row_stagger.push_back(1.0);
        pattern.row_stagger.push_back(0.5);
        return pattern;
    }
    
    pesEMBFill::Preset Wave(){
        pesEMBFill::Preset patt;
        int num_stagger = 12;
        float step = (float)(TWO_PI / num_stagger);
        for(int i=0; i<num_stagger; i++){
            float angle = step * i;
            patt.row_stagger.push_back(sin(angle));
        }
        return patt;
    }
    
    pesEMBFill::Preset Fan(){
        pesEMBFill::Preset patt;
        int num_stagger = 12;
        float step = (float)(PI / num_stagger);
        for(int i=0; i<num_stagger; i++){
            float angle = step * i;
            patt.row_stagger.push_back(sin(angle)*2);
        }
        return patt;
    }
    
    pesEMBFill::Preset P1(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0);
        patt.row_stagger.push_back(0);
        patt.row_stagger.push_back(2);
        patt.row_stagger.push_back(2);
        
        return patt;
    }
    
    pesEMBFill::Preset P2(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.1f);
        patt.row_stagger.push_back(0.2f);
        patt.row_stagger.push_back(0.3f);
        patt.row_stagger.push_back(0.4f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(2.1f);
        patt.row_stagger.push_back(2.2f);
        patt.row_stagger.push_back(2.3f);
        patt.row_stagger.push_back(2.4f);
        
        return patt;
    }
    
    pesEMBFill::Preset P3(){
        pesEMBFill::Preset patt;
        
        patt.row_stagger.push_back(2);
        patt.row_stagger.push_back(1.5);
        patt.row_stagger.push_back(1);
        patt.row_stagger.push_back(0.5);
        patt.row_stagger.push_back(0);
        patt.row_stagger.push_back(2);
        patt.row_stagger.push_back(2.5);
        patt.row_stagger.push_back(3);
        patt.row_stagger.push_back(3.5);
        patt.row_stagger.push_back(0);
        return patt;
    }
    
    pesEMBFill::Preset P4(){
        pesEMBFill::Preset patt;
        
        int num_stagger = 4;
        float step = (float)(M_PI_2 / num_stagger);
        for(int i=0; i<=num_stagger; i++){
            float angle = step * i;
            patt.row_stagger.push_back(-sin(angle));
        }
        for(int i=0; i<=num_stagger; i++){
            float angle = step * i;
            patt.row_stagger.push_back(sin(angle));
        }
        
        return patt;
    }
    
    pesEMBFill::Preset P5(){
        
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.4f);
        patt.row_stagger.push_back(0.8f);
        patt.row_stagger.push_back(0.4f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(1.6f);
        patt.row_stagger.push_back(1.2f);
        patt.row_stagger.push_back(1.6f);
        patt.row_stagger.push_back(2.0f);
        
        return patt;
    }
    
    pesEMBFill::Preset P6(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(2.4f);
        patt.row_stagger.push_back(0.8f);
        patt.row_stagger.push_back(3.2f);
        patt.row_stagger.push_back(1.6f);
        
        return patt;
    }
    
    pesEMBFill::Preset P7(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.4f);
        patt.row_stagger.push_back(1.6f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(3.2f);
        patt.row_stagger.push_back(3.6f);
        patt.row_stagger.push_back(0.8f);
        patt.row_stagger.push_back(1.2f);
        patt.row_stagger.push_back(2.4f);
        patt.row_stagger.push_back(2.8f);
        
        return patt;
    }
    
    pesEMBFill::Preset P8(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(3.6f);
        patt.row_stagger.push_back(3.2f);
        patt.row_stagger.push_back(2.8f);
        patt.row_stagger.push_back(2.4f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(1.6f);
        patt.row_stagger.push_back(1.2f);
        patt.row_stagger.push_back(0.8f);
        patt.row_stagger.push_back(0.4f);
        
        return patt;
    }
    
    pesEMBFill::Preset P9(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.5f);
        patt.row_stagger.push_back(0.5f);
        patt.row_stagger.push_back(0.5f);
        patt.row_stagger.push_back(0.5f);
        
        return patt;
    }
    
    pesEMBFill::Preset P10(){
        pesEMBFill::Preset patt;
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(0.0f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(2.0f);
        patt.row_stagger.push_back(2.0f);
        
        return patt;
    }
    
}
