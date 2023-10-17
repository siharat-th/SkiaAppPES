//
//  PesStitchSimulator.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 9/10/2565 BE.
//

#include "PesStitchSimulator.hpp"
#include "pesEngine.h"

PesStitchSimulator* __simulator = nullptr;

PesStitchSimulator::PesStitchSimulator(){
#if DEBUG
    stitchPerMin = 6000;
#else
    stitchPerMin = 3000;
#endif
    currentStitchNum = 0;
    calcStep();
    bStop = true;
    bPause = false;
}

void PesStitchSimulator::calcStep(){
//    stitchPerMin = (int)pesClamp((float)stitchPerMin, 400.0f, 11400.0f);
    stitchPerMin = (int)pesClamp((float)stitchPerMin, 400.0f, 24000.0f);
    stepPerTick = stitchPerMin / 60.0f / 60.0f;
}

PesStitchSimulator::~PesStitchSimulator(){
}

void PesStitchSimulator::update(){
    if(bStop==false && bPause==false){
        currentStitchNum+=stepPerTick;
    }
}

void PesStitchSimulator::speedup(){
#if DEBUG
    stitchPerMin+=1000;
#else
    stitchPerMin+=500;
#endif
    calcStep();
}

void PesStitchSimulator::slowdown(){
#if DEBUG
    stitchPerMin-=1000;
#else
    stitchPerMin-=500;
#endif
    calcStep();
}

bool PesStitchSimulator::isRunning(){
    return (currentStitchNum>0.0f);
}

bool PesStitchSimulator::isPause(){
    return bPause;
}

void PesStitchSimulator::start(){
    currentStitchNum = 0;
    bStop = false;
    bPause = false;
}

void PesStitchSimulator::pause(){
    //    bStop = true;
    if (bStop == false)
        bPause = true;
}

void PesStitchSimulator::resume(){
    //    bStop = false;
    bPause = false;
}

void PesStitchSimulator::stop(){
    currentStitchNum = 0;
    bStop = true;
    bPause = false;
}

PesStitchSimulator* PesStitchSimulator::getInstance(){
    if(__simulator==nullptr){
        __simulator = new PesStitchSimulator();
    }
    return __simulator;
}
