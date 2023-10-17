#include "MouseTest.h"
#include "imgui.h"

namespace sk_ui {

MouseTestLayer::MouseTestLayer( ) 
    : touchX(-1)
    , touchY(-1)
{}

MouseTestLayer::~MouseTestLayer() {}

void MouseTestLayer::onResize( int w, int h, float scale ) {
    
}
void MouseTestLayer::onPaint( SkCanvas& canvas ) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::Begin("Mouse Info");

    ImGui::Text("Touch pos: (%0.2f, %0.2f)", touchX, touchY);
    if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
    else
        ImGui::Text("Mouse pos: <INVALID>");
    ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
    ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f)   { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
    ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i))          { ImGui::SameLine(); ImGui::Text("b%d", i); }
    ImGui::Text("Mouse dblclick:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
    ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i))         { ImGui::SameLine(); ImGui::Text("b%d", i); }
    ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
    struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
    const ImGuiKey key_first = ImGuiKey_NamedKey_BEGIN;
#else
    struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
    const ImGuiKey key_first = 0;
    //ImGui::Text("Legacy raw:");       for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key++) { if (io.KeysDown[key]) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
#endif
    //ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f)     { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
    //ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyDown(i))                { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
    ImGui::Text("Keys down:");      for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key++) { if (funcs::IsLegacyNativeDupe(key)) continue; if (ImGui::IsKeyDown(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d (%.02f secs)", ImGui::GetKeyName(key), key, ImGui::GetKeyData(key)->DownDuration); } }
    ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i))             { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
    ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i))            { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
    ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
    ImGui::Text("Chars queue:");    for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

    ImGui::Text("NavInputs down:");     for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f)              { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputs[i]); }
    ImGui::Text("NavInputs pressed:");  for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { ImGui::SameLine(); ImGui::Text("[%d]", i); }
    ImGui::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

    ImGui::Button("Hovering me sets the\nkeyboard capture flag");
    if (ImGui::IsItemHovered())
        ImGui::CaptureKeyboardFromApp(true);
    ImGui::SameLine();
    ImGui::Button("Holding me clears the\nthe keyboard capture flag");
    if (ImGui::IsItemActive())
        ImGui::CaptureKeyboardFromApp(false);


    ImGui::End();   
}

bool MouseTestLayer::onMouse(int x, int y, skui::InputState, skui::ModifierKey) {
    mouseX = x;
    mouseY = y;
    return false;    
}

bool MouseTestLayer::onMouseWheel(float deltax, float delta, skui::ModifierKey) {
    return false;
}

bool MouseTestLayer::onTouch(intptr_t owner, skui::InputState, float x, float y) {
    touchX = x;
    touchY = y;
    return false;
}

}
