PROJECT = TemplateApp

OBJ_DIR = obj

LIB_DIR = lib
SK_UI_OBJ_DIR = lib/obj

ROOT_DIR = ../../..

LIBS_DIR = $(ROOT_DIR)/out/canvaskit_wasm
TOOLS_DIR = $(ROOT_DIR)/tools
MODULES_DIR = $(ROOT_DIR)/modules
THIRDPARTY_DIR = $(ROOT_DIR)/third_party

EXTERNALS_DIR = $(THIRDPARTY_DIR)/externals
SDL_DIR = $(EXTERNALS_DIR)/SDL2
SDL_MIXER_DIR = $(EXTERNALS_DIR)/SDL2_mixer
HARFBUZZ_DIR = $(EXTERNALS_DIR)/harfbuzz/src
FREETYPE_DIR = $(EXTERNALS_DIR)/freetype
IMGUI_DIR = $(EXTERNALS_DIR)/imgui
SKIA_DIR = $(EXTERNALS_DIR)/skia
SDL_GUI_DIR = $(ROOT_DIR)/apps/SDL_gui
JUKPUK_DIR = $(ROOT_DIR)/apps/JukPuk/src
SKUTIL_DIR = $(MODULES_DIR)/skutil

APPS_DIR = ../..
APP_ROOT_DIR = ..
APP_SRC_DIR = ../src

SK_UI_DIR = $(ROOT_DIR)/apps/sk_ui
WIDGET_DIR = $(SK_UI_DIR)/Widgets

MQTT_C_DIR = $(ROOT_DIR)/apps/mqtt_c
HAPPY_HTTP_DIR = $(ROOT_DIR)/apps/happyhttp
JSON_DIR = $(ROOT_DIR)/apps/nlohmann
GLAD_DIR = $(ROOT_DIR)/apps/glad

SOURCES = $(APP_SRC_DIR)/main.cpp

OBJECTS = $(patsubst %.cpp,%.bc,$(patsubst %.c,%.bc,$(SOURCES)))	

EMFLAGS := \
	-s USE_SDL=2 \
	-s USE_ZLIB=1 -s USE_HARFBUZZ=1 -s USE_FREETYPE=1 \

EMFLAGS_LINK := \
	-s TOTAL_MEMORY=67108864 \
	-s ALLOW_MEMORY_GROWTH=0 \
	-s ABORTING_MALLOC=0 \
	-s GL_UNSAFE_OPTS=0 \
	-s USE_WEBGL2=1 \
	-s FETCH=1

CXXFLAGS += -Wall -Wformat \
	-I$(APP_ROOT_DIR) -I$(APP_SRC_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)/src -I$(SK_UI_DIR) -I$(APPS_DIR) \
    -I$(MQTT_C_DIR) -I$(JSON_DIR) -I$(HAPPY_HTTP_DIR) -I$(SDL_MIXER_DIR)
CPPFLAGS = -std=c++17
BUILD = 
CPU = 
PLATFORM =
ARM = armv6
LIBS = \
	-L$(LIBS_DIR) \
	--emrun \
	--no-heap-copy \
	--shell-file template.html \
	--preload-file ../../../resources@resources 

##---------------------------------------------------------------------
## SDL_gui
##---------------------------------------------------------------------

CXXFLAGS += -I$(SDL_GUI_DIR)
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/sdl_image
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/sdl_ttf
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/sdl_net
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/sdl_gfx
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/utf8cpp/include
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/utf8cpp/include/utf8
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/tess2/include
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/pugixml
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/ofxClipper/clipper/src/cpp
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/SQLiteCpp/include
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/potrace/include
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/minizip
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/csvstream
CXXFLAGS += -I$(SDL_GUI_DIR)/libs/inih
CXXFLAGS += -I$(SDL_GUI_DIR)/src
CXXFLAGS += -I$(SDL_GUI_DIR)/src/ext

CXXFLAGS += -I$(HARFBUZZ_DIR)
CXXFLAGS += -I$(FREETYPE_DIR)/include
CXXFLAGS += -I$(SKUTIL_DIR)/include

##---------------------------------------------------------------------
## SDL_gui
##---------------------------------------------------------------------

CXXFLAGS += -I$(JUKPUK_DIR)/emb
CXXFLAGS += -I$(JUKPUK_DIR)/ofxClipper
CXXFLAGS += -I$(JUKPUK_DIR)/ofxClipper/include
CXXFLAGS += -I$(JUKPUK_DIR)/ofxClipper/include/ofx
CXXFLAGS += -I$(JUKPUK_DIR)/PES
CXXFLAGS += -I$(JUKPUK_DIR)/svg
CXXFLAGS += -I$(JUKPUK_DIR)/trace
CXXFLAGS += -I$(JUKPUK_DIR)/trace/potrace
CXXFLAGS += -I$(JUKPUK_DIR)/UIBar
CXXFLAGS += -I$(JUKPUK_DIR)/UIDialog
CXXFLAGS += -I$(JUKPUK_DIR)/UIView
CXXFLAGS += -I$(JUKPUK_DIR)/utility
CXXFLAGS += -I$(JUKPUK_DIR)/src	
CXXFLAGS += -I$(JUKPUK_DIR)	

CXXFLAGS += -I$(PESENGINE_DIR)/include

##---------------------------------------------------------------------
## ImGui
##---------------------------------------------------------------------

CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends 

##---------------------------------------------------------------------
## Skia
##---------------------------------------------------------------------

CXXFLAGS += -I$(SKIA_DIR) -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 
CXXFLAGS += -D__USE_SKIA__=1 -D__NO_CANVASKIT__
LIBS += -lskia -lsk_ui -lmqtt_c -lEGL -lskottie -lsksg -lskunicode -lskshaper
LIBS += -lSDL_gui

release: setup html

setup:
	$(eval CC := emcc)
	$(eval TARGETTRAIL := .html)

%.bc: %.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(EMFLAGS) -c $< -o $@		

html: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(EMFLAGS) $(EMFLAGS_LINK) $(LIBS) $^ -o $(PROJECT)$(TARGETTRAIL)

clean: 
	rm $(OBJECTS)
	rm $(PROJECT)$(TARGETTRAIL)

run:
	emrun --browser chrome $(PROJECT).html