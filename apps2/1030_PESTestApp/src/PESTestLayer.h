#ifndef __DECPES_LAYER_APP_H__
#define __DECPES_LAYER_APP_H__

#include "DeclarativeUI.h"
#include "DecUILayer.h"
//#include "pesEngine.h"

#include <future>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

namespace sk_ui {

class DecPesWidget;

class DecPesLayer : public DecUILayer {
public:
    DecPesLayer();
};

DecPesLayer& PesLayer();


class DecPesEditor : public DecRowContainer {
public:
    DecPesEditor();
    virtual ~DecPesEditor() override;

    SkSize fWigetSize = SkSize::MakeEmpty();
    virtual void onResized(int w, int h) override;
    virtual void adjustWidth(int w) override;
    virtual void adjustHeight(int h) override;


    virtual std::string type() override { return "DecPesEditor"; }
    
    void reloadPesFiles(int id = -1);
    void updateLabels();

protected:
    DecPesWidget* _pPesWidget;
    DecText* _pTextHead;
    DecText* _pTextBody;
    DecText* _pTextEye;
    DecText* _pTextMouth;
    DecText* _pTextGlasses;

    DecTextEdit* _pTextEdit1;
    DecTextEdit* _pTextEdit2;
};

DecPesEditor& PesEditor();



class DecPesWidget : public DecWidget {
public:
    DecPesWidget();

    virtual ~DecPesWidget() override;

    virtual std::string type() override { return "PesWidget"; }

    SkSize fWigetSize = SkSize::MakeEmpty();
    virtual void onResized(int w, int h) override;
    virtual void adjustWidth(int w) override;
    virtual void adjustHeight(int h) override;

    //virtual void dirty(bool b = true, bool updateLayout = false) override;
    
    void reset();
    void updatePesImages();
    void updatePesTextImage();
    
    void loadPPES(const std::string & path);
    void savePPES(const std::string & path);
    
    bool importPES(const std::string & path);
    void exportPES(const std::string & path);
    
    void reloadFiles(const std::string & bodyFilepath, const std::string & headFilepath,
                     const std::string & eyeFilepath, const std::string & mouthFilepath,
                     const std::string & glassesFilepath);

    void reloadPesObject(int id, sk_sp<SkData> data = nullptr);
    void reloadPesObjectAsync(int id, sk_sp<SkData> data = nullptr);
    bool asyncReadyAll(int skip = -1);
    
    void insertPPEFText();


#ifdef __EMSCRIPTEN__
    int tasks;
#else
    std::future<void> futWorkerTasks[5];
#endif
    

protected:
    virtual void _draw_content(SkCanvas& canvas) override;


private:
    std::vector<sk_sp<SkImage>> pesImages;

};

DecPesWidget& PesWidget();

void PesTestApp_UpdateImages();
void PesTestApp_UpdateTextImage();

}

#endif //__DECPES_LAYER_APP_H__
