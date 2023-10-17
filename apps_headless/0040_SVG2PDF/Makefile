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

CXX = g++
#CXX = clang++
CC = cc

EXE = svg-2-pdf
OPTIONS = 

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

OBJ_DIR = obj

LIB_DIR = lib

ROOT_DIR = ../..

LIBS_DIR = 
OUT_DIR = $(ROOT_DIR)/out
TOOLS_DIR = $(ROOT_DIR)/tools
THIRDPARTY_DIR = $(ROOT_DIR)/third_party

EXTERNALS_DIR = $(THIRDPARTY_DIR)/externals
SKIA_DIR = $(ROOT_DIR)

APPS_DIR = ../..
APP_ROOT_DIR = .
APP_SRC_DIR = $(APP_ROOT_DIR)/src

GLAD_DIR = $(ROOT_DIR)/apps/glad

SOURCES = \
	$(APP_SRC_DIR)/svg-2-pdf.cpp

CXXFLAGS += -Wall -Wformat -Wattributes \
          -I$(APP_ROOT_DIR) -I$(APP_SRC_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)/src -I$(APPS_DIR) 
	  
CPPFLAGS = -std=c++17
CFLAGS = 
BUILD = 
CPU = 
PLATFORM =
ARM = armv6

LIBS = 


CXXFLAGS += -D__USE_SDL__=1 

##---------------------------------------------------------------------
## Skia
##---------------------------------------------------------------------

CXXFLAGS += -I$(SKIA_DIR) -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 -D__USE_SKIA__=1
LIBS += -lskia -lskshaper -lskunicode -lskresources -lsvg

##---------------------------------------------------------------------

OBJS = $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(basename $(notdir $(SOURCES)))))

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
ifeq ($(UNAME_M), x86_64)
	ECHO_MESSAGE = "linux"
	CPU = x64
	PLATFORM = linux
	LIBS += -L$(ROOT_DIR)/out/linux-$(BUILD) 
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
	LIBS += -lGL -ldl -lX11 -lpthread -lfreetype -lfontconfig  -lpthread 
#-lEGL -lXext 

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
	PLATFORM = macos
	LIBS += -L$(ROOT_DIR)/out/macos-$(CPU)-$(BUILD)
	LIBS += -framework Metal -framework MetalKit -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework Carbon
	LIBS += -framework AudioToolbox -framework CoreAudio -framework CoreFoundation -framework CoreHaptics -framework ForceFeedback
	LIBS += -framework GameController -framework QuartzCore -liconv

endif

##---------------------------------------------------------------------
## BUILD RULES for App
##---------------------------------------------------------------------

$(OBJ_DIR)/%.o:$(APP_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

$(OBJ_DIR)/%.o:$(APP_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(CLAGS) -c -o $@ $<	

$(OBJ_DIR)/%.o:$(GLAD_DIR)/src/%.c
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

##---------------------------------------------------------------------
## BUILD RULES for SK_UI
##---------------------------------------------------------------------

release: CXXFLAGS += -O3 -g0
release: BUILD = release
release: $(EXE)

debug: CXXFLAGS += -DDEBUG -O0 -g3
debug: BUILD = debug
debug: $(EXE)

all: $(release)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS) 
	rm -f *.pdf

reset:
	rm ./imgui.ini

resources:
	ln -s ../../../resources .

run:
	./svg-2-pdf
	../../bin/sysopen svg-2-pdf.pdf
