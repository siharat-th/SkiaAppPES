#include "FPSLayer.h"
#include "imgui.h"
#include "sk_ui.h"

#if defined(_MSC_VER) || defined(_WIN32)
#include "io.h"
#include <windows.h>
#include <sysinfoapi.h>
#pragma comment(lib, "Kernel32")
#else
#include <unistd.h>
#endif


namespace sk_ui {

static const int fontSize = 18;

FPSLayer::FPSLayer(SkColor c, bool bVisible) : frames(0), cpuUsage(0) {
    fVisible = bVisible;
    color = c;

#ifdef __EMSCRIPTEN__
    cpuCores = EM_ASM_INT(return navigator.hardwareConcurrency);
#else
#if defined(SK_BUILD_FOR_WIN)
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    cpuCores = sys_info.dwNumberOfProcessors;
#else
    cpuCores = sysconf(_SC_NPROCESSORS_ONLN);
#endif
#endif

}

FPSLayer::~FPSLayer() {}

void FPSLayer::onResize(int w, int h, float s) {
    scale = s;
    //reset
    frames = 0;
}

void FPSLayer::onUpdate() {
    if (frames == 0) {
        beginTimeStamp = std::chrono::high_resolution_clock::now();
        lastTimeStamp = beginTimeStamp;
        frame_time = 0;
        elapse_time = 0;

        beginCPUTime = clock();
        lastCPUTime = beginCPUTime;
        elapsedCPUTime = 0;
    } else {
        auto endTimeStamp = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeStamp - beginTimeStamp);
        double elapse_time = elapsed.count() * 1e-9;

        auto frameElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeStamp - lastTimeStamp);
        frame_time = frameElapsed.count() * 1e-9;

        lastTimeStamp = endTimeStamp;
        fps = (double)frames / elapse_time;

        clock_t endCPUTime = clock();
        elapsedCPUTime = endCPUTime - beginCPUTime;
        lastCPUTime = endCPUTime;
        cpuUsage = (double)elapsedCPUTime / CLOCKS_PER_SEC / cpuCores / elapse_time * 100.0;
    }
    frames++;
}

void FPSLayer::onPaint(SkCanvas& canvas) {
    if (!fVisible) {
        return;
    }

    SkPaint paint;
    paint.setColor(color);
    SkFont *font = &SKIAAPP()->defaultFont;
    font->setSize(fontSize * scale);

    char message[128] = "";
    sprintf(message, "Frame rate: %.1f fps.", (float)fps);
    canvas.drawSimpleText( message, strlen(message), SkTextEncoding::kUTF8, 2, 2 + fontSize * scale, *font, paint );

    sprintf(message, "CORES:%d, CPU Usage: %.1f%%", (int)cpuCores, (float) cpuUsage);
    canvas.drawSimpleText( message, strlen(message), SkTextEncoding::kUTF8, 2, 2 + 2 * fontSize * scale, *font, paint );          
}

bool FPSLayer::onChar(SkUnichar c, skui::ModifierKey) {
    //SkDebugf("On char %c\n", c);
    if ('\\' == c) {
        this->fVisible = !this->fVisible;
        return true;
    }
    return false;
}

}  // namespace sk_ui