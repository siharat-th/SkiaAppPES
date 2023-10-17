#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev libsdl2-mixer-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL2
#

#CXX = g++
#CXX = clang++

EXE = SDLguiTemplate
OPTIONS = 

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

OBJ_DIR = obj

LIB_DIR = lib
SK_UI_OBJ_DIR = lib/obj

ROOT_DIR = ../../..

LIBS_DIR = $(ROOT_DIR)/libs
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



CXXFLAGS += -Wall -Wformat -Wattributes \
          -I$(APP_ROOT_DIR) -I$(APP_SRC_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)/src -I$(SK_UI_DIR) -I$(APPS_DIR) \
          -I$(MQTT_C_DIR) -I$(JSON_DIR) -I$(HAPPY_HTTP_DIR) -I$(SDL_MIXER_DIR)
	  
CPPFLAGS = -std=c++17
CFLAGS = 
BUILD = 
CPU = 
PLATFORM =
ARM = armv6

LIBS = -lsk_ui2 -lmqtt_c 

##---------------------------------------------------------------------
## glad
##---------------------------------------------------------------------
#Using OpenGL loader: GLAD2
#LOADER_GLAD=OPENGL_LOADER_GLAD
LOADER_GLAD=OPENGLES2_LOADER_GLAD

CXXFLAGS += -I$(GLAD_DIR)/include

ifeq ($(LOADER_GLAD), OPENGL_LOADER_GLAD)
	# 1. glad gl
	CXXFLAGS += -DOPENGL_LOADER_GLAD=1
	SOURCES += $(GLAD_DIR)src/gl.c
endif

ifeq ($(LOADER_GLAD), OPENGLES2_LOADER_GLAD)
	# 2. glad egl, gles2
	CXXFLAGS += -DOPENGLES2_LOADER_GLAD=1
	SOURCES += $(GLAD_DIR)/src/egl.c 
	SOURCES += $(GLAD_DIR)/src/gles2.c
endif

##---------------------------------------------------------------------
## SDL
##---------------------------------------------------------------------

CXXFLAGS += -D__USE_SDL__=1 -DTARGET_PES4 -D_HAVE_HARFBUZZ_

##---------------------------------------------------------------------
## ImGui
##---------------------------------------------------------------------

CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends 

##---------------------------------------------------------------------
## Skia
##---------------------------------------------------------------------

CXXFLAGS += -I$(SKIA_DIR) -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 
#CXXFLAGS += -D__USE_SKIA__=1
LIBS += -lskia -lskshaper -lskunicode -lsksg -lpathkit -lskresources -lharfbuzz -lfreetype2
LIBS += -lJukPuk -lSDL_gui -lz -ltess2 -lzlib -lpotrace -lSQLiteCpp -lpugixml -lclipper
LIBS += -lsdl_ttf -lsdl_image -lsdl_net -lcsvstream -lsdl_gfx -lminizip -linih
LIBS += -lskutil

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
CXXFLAGS += -I$(JUKPUK_DIR)

##---------------------------------------------------------------------

OBJS = $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(basename $(notdir $(SOURCES)))))
#SK_UI_OBJS = $(addsuffix .o, $(addprefix $(SK_UI_OBJ_DIR)/, $(basename $(notdir $(SK_UI_SOURCES)))))

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
ifeq ($(UNAME_M), x86_64)
	ECHO_MESSAGE = "linux"
	CPU = x64
	PLATFORM = linux
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/$(CPU)/release -lSDL2 -lEGL
	CXXFLAGS += -I$(SDL_DIR)/include -D_REENTRANT
else
	ifeq ($(UNAME_M), aarch64)
		CPU = arm64
		# ----------------------- Wayland ----------------------------
		LIBS += -L/usr/lib/aarch64-linux-gnu/vivante 
			
		#GBM
		CXXFLAGS += $(shell pkg-config --cflags gbm)
		LIBS += $(shell pkg-config --libs gbm)
	
		#EGL_WAYLAND (-DWL_EGL_PLATFORM => glad/egl.h)
		CXXFLAGS += $(shell pkg-config --cflags egl_wayland)
		LIBS += $(shell pkg-config --libs egl_wayland)
		
		#GLESv2_wl :
		CXXFLAGS += $(shell pkg-config --cflags glesv2_wl)
		LIBS += $(shell pkg-config --libs glesv2_wl)
		# --------------------------------------------------------------		
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
	LIBS += -framework GameController -framework QuartzCore -liconv -lexpat 
	LIBS += -L$(LIBS_DIR)/$(PLATFORM)/$(CPU)/release
	LIBS += -lsdl 

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
	LIBS += `sdl2-config --libs` 

	CXXFLAGS += `sdl2-config --cflags` -D_THREAD_SAFE
	
	CFLAGS = $(CXXFLAGS)
endif

SK_UI_LIB_DIR = $(LIB_DIR)/$(PLATFORM)/$(CPU)
SK_UI_LIB = libsk_ui2.a
LIBS += -L$(SK_UI_LIB_DIR)

##---------------------------------------------------------------------
## BUILD RULES for App
##---------------------------------------------------------------------

$(OBJ_DIR)/%.o:$(APP_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

$(OBJ_DIR)/%.o:$(SK_UI_DIR)/_sdl_app/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
	
$(OBJ_DIR)/%.o:$(GLAD_DIR)/src/%.c
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

sim: CXXFLAGS += -D__USE_EDISON_SIMULATOR__=1
sim: CXXFLAGS += -O3 -g0
sim: BUILD = release
sim: $(EXE)

simdebug: CXXFLAGS += -D__USE_EDISON_SIMULATOR__=1
simdebug: CXXFLAGS += -DDEBUG -O0 -g3
simdebug: BUILD = debug
simdebug: $(EXE)

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

