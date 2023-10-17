//
//  PesStitchSimulator.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 9/10/2565 BE.
//

#ifndef PesStitchSimulator_hpp
#define PesStitchSimulator_hpp

#include <stdio.h>

class PesStitchSimulator{
public:
    ~PesStitchSimulator();
    
    static PesStitchSimulator* getInstance();
    
    void update();
    
    void start();
    void pause();
    void resume();
    void stop();
    
    int stitchPerMin;
    float stepPerTick;
    float currentStitchNum;
    
    bool bStop;
    bool bPause;
    
    bool isRunning();
    bool isPause();
    
    void speedup();
    void slowdown();
    
private:
    PesStitchSimulator();
    
    void calcStep();
};

#endif /* PesStitchSimulator_hpp */
