PROJECT = HelloWorldApp

SKIA_DIR = ../../..

LIBS_DIR = $(SKIA_DIR)/libs/wasm/release
TOOLS_DIR = $(SKIA_DIR)/tools
THIRDPARTY_DIR = $(SKIA_DIR)/third_party

EXTERNALS_DIR = $(THIRDPARTY_DIR)/externals
SDL_DIR = $(EXTERNALS_DIR)/sdl
IMGUI_DIR = $(EXTERNALS_DIR)/imgui

APP_DIR = ../src
SK_UI_DIR = ../../sk_ui
WIDGET_DIR = $(SK_UI_DIR)/Widgets
MQTT_C_DIR = $(SK_UI_DIR)/mqtt_c

#$(SK_UI_DIR)/_sdl_app/main.cpp 
SOURCES = ./main.cpp \
	$(APP_DIR)/AppCreator.cpp \
	$(APP_DIR)/HelloWorldLayer.cpp

OBJECTS = $(patsubst %.cpp,%.bc,$(patsubst %.c,%.bc,$(SOURCES)))	

EMFLAGS := \
	-s USE_SDL=2 \
	-s USE_ZLIB=1 -s USE_HARFBUZZ=1 -s USE_FREETYPE=1 \
	-s TOTAL_MEMORY=67108864 \
	-s ALLOW_MEMORY_GROWTH=0 \
	-s ABORTING_MALLOC=0 \
	-s GL_UNSAFE_OPTS=0 \
	-s USE_WEBGL2=1 \
	-s FETCH=1

CXXFLAGS += -Wall -Wformat -I$(APP_DIR) -I$(SK_UI_DIR) -I$(WIDGET_DIR) -I$(MQTT_C_DIR)
CPPFLAGS = -std=c++17
BUILD = 
CPU = 
PLATFORM =
ARM = armv6
LIBS = -lsk_ui -lmqtt_c -lEGL \
	-L$(LIBS_DIR) \
	--emrun \
	--no-heap-copy \
	--shell-file template.html \
	--preload-file ../../../resources@resources 

##---------------------------------------------------------------------
## ImGui
##---------------------------------------------------------------------

CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends 

##---------------------------------------------------------------------
## Skia
##---------------------------------------------------------------------

CXXFLAGS += -I$(SKIA_DIR) -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 -D__USE_SKIA__=1
LIBS += -lskia	

release: setup html

setup:
	$(eval CC := emcc)
	$(eval TARGETTRAIL := .html)

%.bc: %.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(EMFLAGS) -c $< -o $@		

html: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(EMFLAGS) $(LIBS) $^ -o $(PROJECT)$(TARGETTRAIL)

run:
	emrun --browser chrome $(PROJECT).html