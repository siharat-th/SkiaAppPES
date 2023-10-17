#ifndef __FPS_LAYER_H__
#define __FPS_LAYER_H__

#include <chrono>
#include "SkiaAppLayer.h"
//#include "Widgets/Widget.h"
#include "include/core/SkColor.h"

namespace sk_ui {

class FPSLayer : public sk_ui::SkiaAppLayer {
public:
    FPSLayer( SkColor c=SK_ColorGRAY, bool bVisible=false );
    virtual ~FPSLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onUpdate() override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey) override;

    SkColor color;
    
private:
    long frames;
    float scale;

    std::chrono::high_resolution_clock::time_point beginTimeStamp;
    std::chrono::high_resolution_clock::time_point lastTimeStamp;    

    double frame_time;
    double elapse_time;
    double fps;    

    // https://stackoverflow.com/questions/8501706/how-to-get-the-cpu-usage-in-c
    // % CPU usage = (CPU time) / (# of cores) / (wall time)
    // Getting the #of logical cores is also platform - specific :
    // Windows : GetSystemInfo() 
    // Linux : sysconf(_SC_NPROCESSORS_ONLN)
    //
    // Target: Platform: Windows 
    // Header: sysinfoapi.h(include Windows.h) 
    // Library: Kernel32.lib 
    // DLL Kernel32.dll
    // void GetSystemInfo( [out] LPSYSTEM_INFO lpSystemInfo );

    clock_t beginCPUTime;
    clock_t lastCPUTime;

    clock_t elapsedCPUTime;

    double cpuUsage;
    int cpuCores;
};


}

#endif //__FPS_LAYER_H__