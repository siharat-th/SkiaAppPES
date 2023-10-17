#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL2
#

#CXX = g++
#CXX = clang++

EXE = TemplateApp


UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

OBJ_DIR = obj

LIB_DIR = lib
SK_UI_OBJ_DIR = lib/obj

ROOT_DIR = ../../..

LIBS_DIR = $(ROOT_DIR)/libs
TOOLS_DIR = $(ROOT_DIR)/tools
THIRDPARTY_DIR = $(ROOT_DIR)/third_party

EXTERNALS_DIR = $(THIRDPARTY_DIR)/externals
SDL_DIR = $(EXTERNALS_DIR)/SDL2
IMGUI_DIR = $(EXTERNALS_DIR)/imgui
SKIA_DIR = $(ROOT_DIR)

APP_ROOT_DIR = ..
APP_DIR = ../src

LIB_SRC_DIR = ../..
SK_UI_DIR = $(LIB_SRC_DIR)/sk_ui
MQTT_C_DIR = $(LIB_SRC_DIR)/mqtt_c
HAPPY_HTTP_DIR = $(LIB_SRC_DIR)/happyhttp
JSON_DIR = $(LIB_SRC_DIR)/nlohmann
WIDGET_DIR = $(SK_UI_DIR)/Widgets

SOURCES = $(SK_UI_DIR)/_sdl_app/main.cpp \
	$(APP_DIR)/AppCreator.cpp \
	$(APP_DIR)/MyWidgetLayer.cpp	

SK_UI_SOURCES = \
    $(HAPPY_HTTP_DIR)/happyhttp.h \
    $(HAPPY_HTTP_DIR)/happyhttp.cpp \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_demo.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/misc/cpp/imgui_stdlib.cpp \
    $(SK_UI_DIR)/FPSLayer.h \
    $(SK_UI_DIR)/FPSLayer.cpp \
    $(SK_UI_DIR)/ImGuiLayer.h \
    $(SK_UI_DIR)/ImGuiLayer.cpp \
    $(SK_UI_DIR)/MouseTest.h \
    $(SK_UI_DIR)/MouseTest.cpp \
    $(SK_UI_DIR)/SkiaApp.h \
    $(SK_UI_DIR)/SkiaApp.cpp \
    $(SK_UI_DIR)/SkiaAppLayer.h \
    $(SK_UI_DIR)/SkiaAppLayer.cpp \
    $(SK_UI_DIR)/StatsLayer.h \
    $(SK_UI_DIR)/StatsLayer.cpp \
    $(SK_UI_DIR)/TemplateLayer.h \
    $(SK_UI_DIR)/TemplateLayer.cpp \
    $(WIDGET_DIR)/WidgetData.h \
    $(WIDGET_DIR)/WidgetData.cpp \
    $(SK_UI_DIR)/WidgetLayer.h \
    $(SK_UI_DIR)/WidgetLayer.cpp \
    $(WIDGET_DIR)/WidgetMQTT.h \
    $(WIDGET_DIR)/WidgetMQTT.cpp \
    $(TOOLS_DIR)/flags/CommandLineFlags.cpp \
    $(TOOLS_DIR)/Resources.cpp \
    $(WIDGET_DIR)/AppBar.h \
    $(WIDGET_DIR)/AppBar.cpp \
    $(WIDGET_DIR)/Button.h \
    $(WIDGET_DIR)/Button.cpp \
    $(WIDGET_DIR)/Container.h \
    $(WIDGET_DIR)/Container.cpp \
    $(WIDGET_DIR)/Downloader.h \
    $(WIDGET_DIR)/Downloader.cpp \
    $(WIDGET_DIR)/Graph.h \
    $(WIDGET_DIR)/Graph.cpp \
    $(WIDGET_DIR)/Guage.h \
    $(WIDGET_DIR)/Guage.cpp \
    $(WIDGET_DIR)/Image.h \
    $(WIDGET_DIR)/Image.cpp \
    $(WIDGET_DIR)/List.h \
    $(WIDGET_DIR)/List.cpp \
    $(WIDGET_DIR)/Scaffold.h \
    $(WIDGET_DIR)/Scaffold.cpp \
    $(WIDGET_DIR)/Slider.h \
    $(WIDGET_DIR)/Slider.cpp \
    $(WIDGET_DIR)/Switch.h \
    $(WIDGET_DIR)/Switch.cpp \
    $(WIDGET_DIR)/Tab.h \
    $(WIDGET_DIR)/Tab.cpp \
    $(WIDGET_DIR)/Text.h \
    $(WIDGET_DIR)/Text.cpp \
    $(WIDGET_DIR)/Toast.h \
    $(WIDGET_DIR)/Toast.cpp \
    $(WIDGET_DIR)/Widget.h \
    $(WIDGET_DIR)/Widget.cpp 


CXXFLAGS += -Wall -Wformat -Wattributes \
          -I$(APP_ROOT_DIR) -I$(APP_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)/src -I$(SK_UI_DIR) -I$(LIB_SRC_DIR) \
          -I$(MQTT_C_DIR) -I$(JSON_DIR) -I$(HAPPY_HTTP_DIR) 
	  
CPPFLAGS = -std=c++17
CFLAGS = 
BUILD = 
CPU = 
PLATFORM =
ARM = armv6
LIBS = -lsk_ui2 -lmqtt_c 

##---------------------------------------------------------------------
## SDL
##---------------------------------------------------------------------

CXXFLAGS += -D__USE_SDL__=1 

##---------------------------------------------------------------------
## ImGui
##---------------------------------------------------------------------

CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends 

##---------------------------------------------------------------------
## Skia
##---------------------------------------------------------------------

CXXFLAGS += -I$(SKIA_DIR) -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 -D__USE_SKIA__=1
LIBS += -lskia -lskshaper -lskunicode

##---------------------------------------------------------------------

OBJS = $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(basename $(notdir $(SOURCES)))))
SK_UI_OBJS = $(addsuffix .o, $(addprefix $(SK_UI_OBJ_DIR)/, $(basename $(notdir $(SK_UI_SOURCES)))))

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
ifeq ($(UNAME_M), x86_64)
	ECHO_MESSAGE = "linux"
	CPU = x64
	PLATFORM = linux
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/$(CPU)/release -lSDL2 -lEGL
	LIBS += -L$(THIRDPARTY_DIR)/SDL2/x64-linux/lib
	CXXFLAGS +=  -D_REENTRANT
else
	ifeq ($(UNAME_M), aarch64)
		CPU = arm64
	else ifeq ($(UNAME_M), armv7l)
		CPU = armv7
	else
		CPU = $(UNAME_M)
	endif
	ECHO_MESSAGE = "Linux ARM"
	PLATFORM = linux
	LIBS += -L/usr/local/lib
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/$(CPU)/release -lSDL2
	CXXFLAGS +=  -D_REENTRANT
endif
	LIBS += -lGL -ldl -lX11 -lXext -lpthread -lfreetype -lfontconfig  -lpthread -lEGL

	CXXFLAGS += -I$(SDL_DIR)/include -D_THREAD_SAFE
	CXXFLAGS += -Wno-unused-but-set-variable 
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	PLATFORM = macos
	ifeq ($(UNAME_M), x86_64)
		CPU = x64
	else
		CPU = arm64
	endif

	LIBS += -framework Metal -framework MetalKit -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework Carbon
	LIBS += -framework AudioToolbox -framework CoreAudio -framework CoreFoundation -framework CoreHaptics -framework ForceFeedback
	LIBS += -framework GameController -framework QuartzCore -liconv
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/$(CPU)/$(BUILD)
	LIBS += -L$(THIRDPARTY_DIR)/SDL2/$(PLATFORM)
	LIBS += -lSDL2 -lsksg -lexpat -lpathkit -lskresources

	CXXFLAGS += -I$(SDL_DIR)/include -D_THREAD_SAFE
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CXXFLAGS += -fvisibility=hidden
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	ECHO_MESSAGE = "MinGW"
	PLATFORM = windows
	
	LIBS += -lgdi32 -lopengl32 -limm32 
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/x64/release
	LIBS += `sdl2-config --libs` -lskia

	CXXFLAGS += `sdl2-config --cflags` -D_THREAD_SAFE
	
	CFLAGS = $(CXXFLAGS)
endif

SK_UI_LIB_DIR = $(LIB_DIR)/$(PLATFORM)/$(CPU)
SK_UI_LIB = libsk_ui2.a
LIBS += -L$(SK_UI_LIB_DIR)

##---------------------------------------------------------------------
## BUILD RULES for App
##---------------------------------------------------------------------

$(OBJ_DIR)/%.o:$(APP_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

$(OBJ_DIR)/%.o:$(SK_UI_DIR)/_sdl_app/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

##---------------------------------------------------------------------
## BUILD RULES for SK_UI
##---------------------------------------------------------------------

$(SK_UI_OBJ_DIR)/%.o:$(HAPPY_HTTP_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<		

$(SK_UI_OBJ_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<			

$(SK_UI_OBJ_DIR)/%.o:$(IMGUI_DIR)/misc/cpp/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<		

$(SK_UI_OBJ_DIR)/%.o:$(SKIA_DIR)/tools/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<		

$(SK_UI_OBJ_DIR)/%.o:$(SKIA_DIR)/tools/flags/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<			

$(SK_UI_OBJ_DIR)/%.o:$(SK_UI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<		

$(SK_UI_OBJ_DIR)/%.o:$(WIDGET_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	


release: CXXFLAGS += -O3 -g0
release: BUILD = release
release: $(EXE)

debug: CXXFLAGS += -DDEBUG -O0 -g3
debug: BUILD = debug
debug: $(EXE)

$(SK_UI_LIB_DIR)/$(SK_UI_LIB): $(SK_UI_OBJS)
	@mkdir -p $(SK_UI_LIB_DIR)
	ar rcs $@ $^

all: $(release)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS) $(SK_UI_LIB_DIR)/$(SK_UI_LIB)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS) $(SK_UI_OBJS)

reset:
	rm ./imgui.ini

resources:
	ln -s ../../../resources .

