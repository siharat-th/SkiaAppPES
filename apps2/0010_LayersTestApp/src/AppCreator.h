#ifndef __LAYERSTEST_CREATOR_H__
#define __LAYERSTEST_CREATOR_H__

#include "SkiaApp.h"
#include "SkiaAppLayer.h"
#include "DeclarativeUI.h"
#include "StatsLayer.h"
#include "FPSLayer.h"

sk_ui::SkiaAppLib * CreateSkiaAppLib();
sk_ui::SkiaAppLib * CreateLayersTestAppSkiaAppLib();

#endif