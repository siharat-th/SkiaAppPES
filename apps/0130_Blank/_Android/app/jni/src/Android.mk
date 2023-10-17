LOCAL_PATH := $(call my-dir)

# skia
#
include $(CLEAR_VARS)
LOCAL_MODULE := skia_static
LOCAL_SRC_FILES := ../../src/main/jniLibs/$(TARGET_ARCH_ABI)/libskia.a
LOCAL_EXPORT_C_INCLUDES := ../../../../../..
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
#LOCAL_EXPORT_LDLIBS := -latomic
endif
include $(PREBUILT_STATIC_LIBRARY)

# sk_ui
#
include $(CLEAR_VARS)
LOCAL_MODULE := sk_ui2_static

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../.. \
                           $(LOCAL_PATH)/../../../../../sk_ui \
                           $(LOCAL_PATH)/../../../../../happyhttp \
                           $(LOCAL_PATH)/../../../../../nlohmann \
                           $(LOCAL_PATH)/../../../../../sk_ui/Widgets

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../.. \
    $(LOCAL_PATH)/../../../../.. \
    $(LOCAL_PATH)/../../../../../sk_ui \
    $(LOCAL_PATH)/../../../../../mqtt_c \
    $(LOCAL_PATH)/../../../../../happyhttp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/sdl/include \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/../../../../../../tools/Resources.cpp \
	$(LOCAL_PATH)/../../../../../../tools/flags/CommandLineFlags.cpp \
	$(LOCAL_PATH)/../../../../../happyhttp/happyhttp.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/SkiaApp.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/SkiaAppLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/TemplateLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/ImGuiLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/StatsLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/MouseTest.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/WidgetLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/FPSLayer.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Widget.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/WidgetData.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Container.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Downloader.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Graph.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Image.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Text.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Scaffold.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/AppBar.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Button.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/List.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Tab.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Toast.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Slider.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Guage.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/WidgetMQTT.cpp \
    $(LOCAL_PATH)/../../../../../sk_ui/Widgets/Switch.cpp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui/imgui.cpp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui/imgui_demo.cpp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui/imgui_draw.cpp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui/imgui_widgets.cpp \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui/misc/cpp/imgui_stdlib.cpp

LOCAL_STATIC_LIBRARIES := skia_static
LOCAL_CPPFLAGS := -std=c++17 -D__USE_SDL__
LOCAL_LDLIBS := -llog
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_EXPORT_LDLIBS := -latomic
endif
include $(BUILD_STATIC_LIBRARY)

# mqtt_c
#
include $(CLEAR_VARS)
LOCAL_MODULE := mqtt_c_static
LOCAL_SRC_FILES := ../../src/main/jniLibs/$(TARGET_ARCH_ABI)/libmqtt_c.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../sk_ui/mqtt_c
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_EXPORT_LDLIBS := -latomic
endif
include $(PREBUILT_STATIC_LIBRARY)

# imgui
#
include $(CLEAR_VARS)
LOCAL_MODULE := imgui_static
LOCAL_SRC_FILES := ../../src/main/jniLibs/$(TARGET_ARCH_ABI)/libimgui.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../third_party/externals/imgui
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_EXPORT_LDLIBS := -latomic
endif
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SK_UI_DIR := $(LOCAL_PATH)/../../../../../sk_ui
WIDGET_DIR := $(LOCAL_PATH)/../../../../../sk_ui/Widgets

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
    $(LOCAL_PATH)/../../../../../.. \
    $(LOCAL_PATH)/../../../../../../third_party/externals/imgui \
    $(LOCAL_PATH)/../../../../src \
    $(LOCAL_PATH)/../../../../../sk_ui

# Add your application source files here...
LOCAL_SRC_FILES :=  $(SK_UI_DIR)/_sdl_app/main.cpp \
    $(LOCAL_PATH)/../../../../../../tools/Resources.cpp \
    $(LOCAL_PATH)/../../../../../../tools/flags/CommandLineFlags.cpp \
    src/AppCreator.cpp \
    src/HelloWorldLayer.cpp

LOCAL_STATIC_LIBRARIES := \
	skia_static \
	imgui_static \
	sk_ui2_static \
	mqtt_c_static

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_CPPFLAGS += -DSK_GL -DGR_GL_CHECK_ERROR=0 -DGR_GL_LOG_CALLS=0 -std=c++17 -D__USE_SDL__
LOCAL_LDLIBS := -lEGL -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
