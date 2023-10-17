#include "HelloWorldLayer.h"

using namespace sk_ui;

HelloWorldLayer::HelloWorldLayer()
    : fShowImGuiDemoWindow( true )
    , fPause( false )
    , fRotationAngle( 0 )
{   
    SKIAAPP()->setAppTitle( "HelloWorld" );
}

HelloWorldLayer::~HelloWorldLayer() {

}

void HelloWorldLayer::onResize( int w, int h, float s ) {
    width = w;
    height = h;
    scale = s;
}

void HelloWorldLayer::onPaint( SkCanvas& canvas ) {
    
    canvas.clear( SK_ColorWHITE );
    
    canvas.save();
    canvas.scale( scale, scale );

    

    //canvas.clear(SK_ColorCYAN);

    SkPaint paint;
    paint.setColor(SK_ColorRED);

    // Draw a rectangle with red paint
    SkRect rect = SkRect::MakeXYWH(10, 10, 128, 128);
    canvas.drawRect(rect, paint);

    // Set up a linear gradient and draw a circle
    {
        SkPoint linearPoints[] = { { 0, 0 }, { 300, 300 } };
        SkColor linearColors[] = { SK_ColorGREEN, SK_ColorBLACK };
        paint.setShader(SkGradientShader::MakeLinear(linearPoints, linearColors, nullptr, 2,
                                                     SkTileMode::kMirror));
        paint.setAntiAlias(true);

        canvas.drawCircle(200, 200, 64, paint);

        // Detach shader
        paint.setShader(nullptr);
    }

    // Draw a message with a nice black paint
    SkFont *font = &SKIAAPP()->defaultFont;
    font->setSubpixel(true);
    font->setSize(20);
    paint.setColor(SK_ColorBLACK);

    canvas.save();
    static const char message[] = "Hello World ไก่เป่าปี่ดีที่สุด";

    // Translate and rotate
    canvas.translate(300, 300);
    if( !fPause ) {
        fRotationAngle += 0.2f;
    }
    if (fRotationAngle > 360) {
        fRotationAngle -= 360;
    }
    canvas.rotate(fRotationAngle);

    // Draw the text
    //canvas.drawSimpleText(message, strlen(message), SkTextEncoding::kUTF8, 0, 0, *font, paint);
    auto blob = SkTextBlob::MakeFromText(message, strlen(message), *font);
    canvas.drawTextBlob(blob, 0, 0, paint);

    canvas.restore();

    ImGui::ShowDemoWindow(&fShowImGuiDemoWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &fShowImGuiDemoWindow);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }    
    
    canvas.restore();
    
}

bool HelloWorldLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false;
}

// Before use, check key available in function skui::Key get_key(unsigned short vk)
bool HelloWorldLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey mod) {
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

bool HelloWorldLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool HelloWorldLayer::onMouseWheel(float deltax, float delta, skui::ModifierKey) {
    return false;
}

bool HelloWorldLayer::onTouch(intptr_t owner, skui::InputState state, float x, float y) {
    return false;
}

