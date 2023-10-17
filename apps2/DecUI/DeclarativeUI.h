#ifndef __DECLARATIVE_UI_H__
#define __DECLARATIVE_UI_H__

// cross platforms
#ifdef __EMSCRIPTEN__
#else
#if defined(SK_BUILD_FOR_MAC)
#elif defined(SK_BUILD_FOR_WIN)
#elif defined(SK_BUILD_FOR_UNIX)
#elif defined(SK_BUILD_FOR_ANDROID)
#elif defined(SK_BUILD_FOR_IOS)
#endif
#endif

//#include "SkiaApp.h"
//#include "SkiaAppLayer.h"
//#include "DeclarativeUI.h"
//#include "StatsLayer.h"
//#include "FPSLayer.h"
#include "DecUI.h"
#include "DecUILayer.h"
#include "DecFont.h"
#include "DecWidget.h"
#include "DecContainer.h"
#include "DecFlowContainer.h"
#include "DecRowContainer.h"
#include "DecColumnContainer.h"
#include "DecStackContainer.h"
#include "DecPageContainer.h"
#include "DecImage.h"
#include "DecText.h"
#include "DecIcon.h"
#include "DecLabel.h"
#include "DecButton.h"
#include "DecCheckBox.h"
#include "DecRadioButton.h"
#include "DecSwitch.h"
#include "DecSlider.h"
#include "DecSVGDom.h"
#include "DecSpacer.h"
#include "DecScroller.h"
#include "DecList.h"
#include "DecListItem.h"
#include "DecTree.h"
#include "DecTreeItem.h"
#include "DecModal.h"
#include "DecMessageBox.h"
#include "DecFileDialog.h"
#include "DecComboBox.h"
#include "DecComboBoxItem.h"
#include "DecFileList.h"
#include "DecJob.h"
#include "DecTab.h"
#include "DecParagraph.h"
//#include "DecQRCode.h"

#endif //__DECLARATIVE_UI_H__
