//
//  main.cpp
//  pesEngineTest
//
//  Created by SIHARAT THAMMAYA on 22/2/2563 BE.
//  Copyright © 2563 SIHARAT THAMMAYA. All rights reserved.
//

#include <iostream>

#include "pesEngine.h"
#include "pesEMBClassify.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
//    pes_classify_runtest();
    
    pesPath paths;
//    paths.moveTo(0,0);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.close();
//    paths.moveTo(20,20);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.lineTo(23,21);
//    paths.lineTo(10,5);
//    paths.close();
    
    paths.moveTo(0,0);
    paths.lineTo(10,0);
//    paths.lineTo(10,2);
//    paths.close();
    paths.moveTo(10,2);
    paths.lineTo(0,2);
//    paths.lineTo(0,4);
//    paths.close();
    
    ClipperLib::ClipType currentClipperType = ClipperLib::ctUnion;
    ClipperLib::JoinType currentClipperJoinType = ClipperLib::jtMiter;
    ClipperLib::EndType currentClipperEndType  = ClipperLib::etClosedPolygon;
    float miterLimit = 2;
    float offsetDelta = 0.1;

    std::vector<pesPolyline> clipSubjects;
//    for(auto o: paths.getOutline()){
        vector<pesPolyline>outlines = paths.getOutline();
        clipSubjects.insert(clipSubjects.end(), outlines.begin(), outlines.end());
//    }

    pesClipper clipper;
    clipper.addPolylines(clipSubjects, ClipperLib::ptSubject, false);

    std::vector<pesPolyline> clips = clipper.getClipped(currentClipperType);
//    std::vector<pesPolyline> offsets = pesClipper::getOffsets(clips,
//                                                              offsetDelta,
//                                                              currentClipperJoinType,
//                                                              currentClipperEndType,
//                                                              miterLimit);
    
//    std::vector<pesPolyline> offsets = pesClipper::getOffsets(clipSubjects,
//    offsetDelta,
//    currentClipperJoinType,
//    currentClipperEndType,
//    miterLimit);
    
    std::vector<pesPolyline> offsets = pesClipper::getOffsets(clipSubjects, offsetDelta);
    
    cout << "offset : " << offsets.size();
    
    return 0;
}
