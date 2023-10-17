//
//  pesSatinColumn.cpp
//  pesEngine
//
//  Created by SIHARAT THAMMAYA on 3/7/2562 BE.
//  Copyright © 2562 SIHARAT THAMMAYA. All rights reserved.
//

#include "pesSatinColumn.hpp"

using namespace std;

pesSatinColumn::pesSatinColumn(const vector<pesPath> & paths){
    if(paths.size()==2){
        int count = 0;
        bool isColumn = paths[0].getCommands().size() == paths[1].getCommands().size();
        for(int i=0; i<2; i++){
            for(auto c : paths[i].getCommands()){
                if(c.type==pesPath::Command::Type::_moveTo){
                    count++;
                }
                if(c.type!=pesPath::Command::Type::_moveTo && c.type!=pesPath::Command::Type::_bezierTo && c.type!=pesPath::Command::Type::_lineTo){
                    isColumn = false;
                }
            }
        }
        
        if(isColumn && count==2){
            vector<pesPath::Command> command0 = paths[0].getCommands();
            vector<pesPath::Command> command1 = paths[1].getCommands();
            int num = (int)command0.size();
            for(int i=1; i<num; i++){
                // segment#0
                {
                    pesPath::Command prevCmd = command0[i-1];
                    pesPath::Command currentCmd = command0[i];
                    pesPolyline segment0;
                    segment0.addVertex(prevCmd.to);
                    
                    if(currentCmd.type==pesPath::Command::Type::_lineTo)
                        segment0.lineTo(currentCmd.to.x, currentCmd.to.y);
                    else
                        segment0.bezierTo(currentCmd.cp1, currentCmd.cp2, currentCmd.to);
                    segments[0].push_back(segment0);
                }
                
                // segment#1
                {
                    pesPath::Command prevCmd = command1[i-1];
                    pesPath::Command currentCmd = command1[i];
                    pesPolyline segment1;
                    segment1.addVertex(prevCmd.to);
                    
                    if(currentCmd.type==pesPath::Command::Type::_lineTo)
                        segment1.lineTo(currentCmd.to.x, currentCmd.to.y);
                    else
                        segment1.bezierTo(currentCmd.cp1, currentCmd.cp2, currentCmd.to);
                    segments[1].push_back(segment1);
                }
            }
        }
        else if(count==2){
//            cout << "pesSatinColumn isColumn=false" << endl;
            pesPolyline segment0 = paths[0].getOutline().front();
            pesPolyline segment1 = paths[1].getOutline().front();
            if(segment0.size() && segment1.size()){
                segments[0].push_back(segment0);
                segments[1].push_back(segment1);
            }
        }
        
    }
}

pesSatinColumn::~pesSatinColumn(){
    segments[0].clear();
    segments[1].clear();
}

void pesSatinColumn::doSatin(float zigzagSpace, pesPolyline & patch){
    if(segments[0].size()==segments[1].size()){
        
        unsigned int ppmm = param.pixels_per_mm;
        const float tol = param.satin_auto_shorten_tolerance_mm * ppmm;
        const float shorten_offset = param.satin_shorten_offset_mm * ppmm;
        const bool bAutoShorten = param.satinAutoShorten;
        
        int num = (int)segments[0].size();
        for(int i=0; i<num; i++){
            float len0 = segments[0][i].getPerimeter();
            float len1 = segments[1][i].getPerimeter();
            int num_points = (int)round( max(len0, len1) / zigzagSpace);
            
            // Auto shorten
            bool bShorten = false;
            int shortenSide = 0;
            if(bAutoShorten){
                float sp0 = len0/num_points;
                float sp1 = len1/num_points;
                if(MIN(sp0, sp1)<tol){
                    bShorten = true;
                    if(sp1<sp0) shortenSide = 1;
                }
            }
            
            auto side0 = segments[0][i].getResampledByCount2(num_points);
            auto side1 = segments[1][i].getResampledByCount2(num_points);
            
            if(side0.size()== side1.size()){
                // satin: do a zigzag pattern, alternating between the paths.  The
                // zigzag looks like this to make the satin stitches look perpendicular
                // to the column:
                //
                // /|/|/|/|/|/|/|/|
                
                float pullCompensate = param.pull_compensation_mm * param.pixels_per_mm;
                if(pullCompensate!=0.0){
                    float offset_px = pullCompensate;
                    for(int j=0; j<(int)side0.size(); j++){
                        side0[j] = side0[j] + (side0[j]-side1[j]).normalize() * offset_px;
                        side1[j] = side1[j] + (side1[j]-side0[j]).normalize() * offset_px;
                    }
                }
                
                //                if(i==0)
                {
                    patch.addVertex(side0[0]);
                }
                
                patch.addVertex(side1[0]);
                for(int j=1; j<(int)side0.size(); j++){
                    if(bShorten && j%2==1){
                        float offset = shorten_offset;
                        float half = side0[j].distance(side1[j])/2;
                        if(offset>half)
                            offset=half;
                        if(shortenSide==0){
                            patch.addVertex(side0[j] + pesVec2f(side1[j]-side0[j]).normalize() *  offset);
                            patch.addVertex(side1[j]);
                        }
                        else if(shortenSide==1){
                            patch.addVertex(side0[j]);
                            patch.addVertex(side1[j] + pesVec2f(side0[j]-side1[j]).normalize() *  offset);
                        }
                    }
                    else{
                        // add pair points
                        patch.addVertex(side0[j]);
                        patch.addVertex(side1[j]);
                    }
                }
                
            }
        }
    }
    else{
//        GUI_LogError("SatinColumnCSP") << "csp segments size not equal";
    }
}

// MARK: pesSatinColumnGeneretor

pesSatinColumnGenerator::pesSatinColumnGenerator()
:cnt(0)
{
    csp[0].reset();
    csp[1].reset();
}

pesSatinColumnGenerator::~pesSatinColumnGenerator(){
    csp[0].reset();
    csp[1].reset();
}

void pesSatinColumnGenerator::reset(){
    csp[0].reset();
    csp[1].reset();
    cnt=0;
}

void pesSatinColumnGenerator::addCurvePoint(const pesVec2f & p){
    csp[cnt%2].addCurvePoint(p);
    cnt++;
}

void pesSatinColumnGenerator::addCornerPoint(const pesVec2f & p){
    csp[cnt%2].addCornerPoint(p);
    cnt++;
}

void pesSatinColumnGenerator::calculateCSP(){
    csp[0].calculateCSP();
    csp[1].calculateCSP();
}

vector<pesPath> pesSatinColumnGenerator::getSatinColumnPaths(){
    vector<pesPath> paths;
    paths.push_back(csp[0].path);
    paths.push_back(csp[1].path);
    return paths;
}

//#include "pesEngine.h"
//void example(){
//    pesSatinColumnGenerator gen;
//
//    gen.addCornerPoint(pesVec2f(0,0));      // moveTo
//    gen.addCornerPoint(pesVec2f(0,10));     // moveTo
//
//    gen.addCornerPoint(pesVec2f(10,0));     // lineTo
//    gen.addCornerPoint(pesVec2f(10,10));    // lineTo
//
//    gen.addCurvePoint(pesVec2f(15, 5));     // bezierTo
//    gen.addCurvePoint(pesVec2f(15, 15));    // bezierTo
//
//    gen.calculateCSP();                     // calculate cubic super path
//
//    pesData data;
//    data.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN);
//    data.paths = gen.getSatinColumnPaths();
//    data.applyFill();
//
//    pesGetDocument()->addObject(data);
//}
