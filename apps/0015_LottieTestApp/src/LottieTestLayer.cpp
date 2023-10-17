#include "LottieTestLayer.h"
#include "tools/Resources.h"
#include "include/core/SkColor.h"
#include "include/utils/SkAnimCodecPlayer.h"
#include "modules/skottie/include/Skottie.h"
#include "modules/skottie/include/SkottieProperty.h"
#include "modules/skottie/utils/SkottieUtils.h"
#include "modules/skresources/include/SkResources.h"
#include "tools/Resources.h"

using namespace sk_ui;

class ResourceProvider final : public skresources::ResourceProvider {
public:
    sk_sp<skresources::ImageAsset> loadImageAsset(const char *resource_path, const char *resource_name,
                                                    const char *resource_id ) const override {
        SkDebugf( "resource: %s %s %s\n", resource_path, resource_name, resource_id );
        char sz[128];
        sprintf( sz, "skottie/%s%s", resource_path, resource_name );
        return skresources::MultiFrameImageAsset::Make(GetResourceAsData(sz));
    }
};


LottieTestLayer::LottieTestLayer()
    : fPause( false )
{   
    printf( "LottieTestLayer const\n" );
    SKIAAPP()->setAppTitle( "LottieTestApp" );

    
 
    const char *skottie_filename = 
        //"skottie/skottie_sample_1.json";
        //"skottie/skottie_sample_2.json";
        //"skottie/skottie-glow-spread.json";
        //"skottie/skottie-outerglow-style.json";
        //"skottie/skottie_sample_multiframe.json";
        
        "skottie/Start.json";
        //"skottie/Shutdown.json";
        //"skottie/Menu.json";
        //"skottie/Charging.json";
        //"skottie/ChargingFast.json";
        //"skottie/ChargingFull.json";
        //"skottie/BatteryEmpty.json";

    if (auto stream = GetResourceAsStream(skottie_filename)) {
        fAnimation = skottie::Animation::Builder()
                        .setResourceProvider(sk_make_sp<ResourceProvider>())
                        .make(stream.get());

        //fPropManager = std::make_unique<skottie_utils::CustomPropertyManager>();
        //fAnimation   = skottie::Animation::Builder()
        //                    .setPropertyObserver(fPropManager->getPropertyObserver())
        //                    .make(stream.get());                        
        fAnimation->seek(0);
        printf( "Animation OK\n" );
    }
    fAnimTimer.run();    
}

LottieTestLayer::~LottieTestLayer() {

}

void LottieTestLayer::onResize( int w, int h, float s ) {
    width = w;
    height = h;
    scale = s;
}

void LottieTestLayer::onPaint( SkCanvas& canvas ) {
    canvas.clear(SK_ColorBLACK);
    if ( fAnimation == nullptr ) {
        return;
    }
    
    fAnimTimer.updateTime();
    double nanos = fAnimTimer.nanos();

    const auto duration = fAnimation->duration();
    fAnimation->seek(std::fmod(1e-9 * nanos, duration) / duration);  

    auto dest = SkRect::MakeWH(1024, 600);
    fAnimation->render(&canvas, &dest); 
}

bool LottieTestLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false;
}

// Before use, check key available in function skui::Key get_key(unsigned short vk)
bool LottieTestLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey mod) {
    if ( key == skui::Key::k0 ) {
        if( state == skui::InputState::kUp ) {
            fPause = false;
        }
        else if( state == skui::InputState::kDown ) {
            fPause = true;
        }
        return true;
    }
    return false;
}

bool LottieTestLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool LottieTestLayer::onMouseWheel(float deltax, float delta, skui::ModifierKey) {
    return false;
}

bool LottieTestLayer::onTouch(intptr_t owner, skui::InputState state, float x, float y) {
    return false;
}

