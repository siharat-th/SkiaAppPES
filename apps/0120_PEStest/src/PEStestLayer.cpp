#include "iostream"
#include "fstream"
#include "PEStestLayer.h"
#include "pesDocument.hpp"
#include "PPEF_Reader.hpp"
#include "imgui_internal.h"

using namespace sk_ui;

static int revision = 1;
static pesDocument *pesDoc = NULL;
static PPEF_Reader *reader = NULL;
static sk_sp<SkImage> image_ppef = nullptr;

static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

ImGuiID dockspace_id;
static bool fShowImGuiDemoWindow = false;

static bool bShowLayerPane = true;
static bool bShowToolPane = true;

static pesVec2f hoopSize = pesVec2f(0,0);

PEStestLayer::PEStestLayer()
    : fShowImGuiDock( true )
    , fPause( false )
    , fRotationAngle( 0 )
{   
    SKIAAPP()->setAppTitle( "PES Test" );

    pesDoc = pesGetDocument();
    reader = new PPEF_Reader("./resources/PPEF/Thai001.ppef");

    newDocument();

}

PEStestLayer::~PEStestLayer() {

}

void PEStestLayer::onResize( int w, int h, float s ) {
    width = w;
    height = h;
    scale = s;
}

void PEStestLayer::addText() {
    SkDebugf( "Add Text\n" );
    vector<pesPath> ppef;

    ppef = reader->getStringAsShapes("ภิญญ์จักรปัก");
    if(ppef.size()){
        pesData data;
        data.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT);
        data.paths = ppef;
        data.applyPPEFFill();
        int cnt = pesGetDocument()->addObject(data);
        if(cnt>0){
            image_ppef = pesGetDocument()->makePesImageSnapshot(cnt-1);
        }
    }    
}

void PEStestLayer::saveDoc() {
    SkDebugf( "Save Doc\n" );
    pesBuffer buffer = pesDoc->getPES5Buffer();

    ofstream myfile;
    myfile.open ("example.pes");
    buffer.writeTo( myfile );
    myfile.close();    
}

void PEStestLayer::newDocument() {
    SkDebugf( "This is new doc\n" );
    pesDoc->newDocument();
    updateInfo();
}

void PEStestLayer::updateInfo() {
    pesVec2f hoopSize = pesDoc->getHoopSizeInMM();
    SkDebugf( "Hoop size: %0.2f, %0.2f\n", hoopSize.x, hoopSize.y );
}

// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note: You can use most Docking facilities without calling any API. You DO NOT need to call DockSpace() to use Docking!
// - Drag from window title bar or their tab to dock/undock. Hold SHIFT to disable docking.
// - Drag from window menu button (upper-left button) to undock an entire node (all windows).
// - When io.ConfigDockingWithShift == true, you instead need to hold SHIFT to _enable_ docking/undocking.
// About dockspaces:
// - Use DockSpace() to create an explicit dock node _within_ an existing window.
// - Use DockSpaceOverViewport() to create an explicit dock node covering the screen or a specific viewport.
//   This is often used with ImGuiDockNodeFlags_PassthruCentralNode.
// - Important: Dockspaces need to be submitted _before_ any window they can host. Submit it early in your frame! (*)
// - Important: Dockspaces need to be kept alive if hidden, otherwise windows docked into it will be undocked.
//   e.g. if you have multiple tabs with a dockspace inside each tab: submit the non-visible dockspaces with ImGuiDockNodeFlags_KeepAliveOnly.
// (*) because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node,
// because that window is submitted as part of the part of the NewFrame() call. An easy workaround is that you can create
// your own implicit "Debug##2" window after calling DockSpace() and leave it in the window stack for anyone to use.
void PEStestLayer::ShowAppDockSpace(bool* p_open)
{
    // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
    // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    // - we allow the host window to have padding (when opt_padding == true)
    // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
    // TL;DR; this demo is more complicated than what you would normally use.
    // If we removed all the options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode |
        ImGuiDockNodeFlags_NoDockingInCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }     

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    //if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;      

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        //ShowDockingDisabledMessage();
    }         

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            if( ImGui::MenuItem("New Document", NULL, false, true) ) { newDocument(); };
            if( ImGui::MenuItem("Save", NULL, false, true) ) { saveDoc(); };
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL)) {
                
            }
            ImGui::EndMenu();
        }
        /*
        HelpMarker(
            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
            "- Drag from window title bar or their tab to dock/undock." "\n"
            "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
            "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
            "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
            "This demo app has nothing to do with enabling docking!" "\n\n"
            "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
            "Read comments in ShowExampleAppDockSpace() for more details.");
        */

        ImGui::EndMenuBar();
    }

    ImGui::End();    
}

void PEStestLayer::ShowDocBuilder() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    dockspace_id = ImGui::GetID("MyDockSpace");
    ImVec2 dockspace_size = viewport->WorkSize;

    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

    dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
    dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

    //ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
    ImGui::DockBuilderDockWindow("PES Layer", dock_id_prop);
    ImGui::DockBuilderDockWindow("PES Test App", dock_id_prop);
    //ImGui::DockBuilderDockWindow("Extra", dock_id_prop);
    ImGui::DockBuilderFinish(dockspace_id);    

}

void PEStestLayer::ShowLayerPane(bool *p_open) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    //ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(200, viewport->WorkSize.y), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::Begin("PES Layer", p_open, window_flags);
   
    ImGui::End();

}

void PEStestLayer::ShowToolPane(bool *p_open) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    //ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x-200, 0), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(200, viewport->WorkSize.y), ImGuiCond_Always);

    ImGui::Begin("PES Test App", p_open, ImGuiWindowFlags_None);
    ImGui::Text("Revision %d", revision);
  
    if (ImGui::SmallButton("Add Text"))  {
        addText();
    }    
    ImGui::End();  
}

void PEStestLayer::onPaint( SkCanvas& canvas ) {
    canvas.clear( SK_ColorWHITE );
    canvas.save();
    canvas.scale( scale, scale );

    if(image_ppef != nullptr){
        SkPaint paint;
        canvas.drawImage(image_ppef, 128, 128, SkSamplingOptions(), &paint);
    }    

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "save.ini";
    io.WantSaveIniSettings = true;
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ShowAppDockSpace( &fShowImGuiDock );    

    ShowLayerPane(&bShowLayerPane);

    ShowToolPane(&bShowToolPane);

    static bool bFirst = true;
    if( bFirst ) {
        bFirst = false;
        ImGui::LoadIniSettingsFromDisk("save.ini"); 
    }

    if( fShowImGuiDemoWindow ) {
        ImGui::ShowDemoWindow(&fShowImGuiDemoWindow);
    }
    
    canvas.restore();
    
}

bool PEStestLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
    if( c == 32 ) {
        fShowImGuiDemoWindow = !fShowImGuiDemoWindow;
    }
    return false;
}

// Before use, check key available in function skui::Key get_key(unsigned short vk)
bool PEStestLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey mod) {
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

bool PEStestLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool PEStestLayer::onMouseWheel(float delta, skui::ModifierKey) {
    return false;
}

bool PEStestLayer::onTouch(intptr_t owner, skui::InputState state, float x, float y) {
    return false;
}

