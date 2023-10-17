LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpotrace
LOCAL_PATH := $(LOCAL_PATH)
LOCAL_CFLAGS += -DHAVE_CONFIG_H
LOCAL_CPPFLAGS += -frtti -fexceptions -std=c++11 -w
LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := \
	backend_dxf.c \
	backend_eps.c \
	backend_geojson.c \
	backend_pdf.c \
	backend_pgm.c \
	backend_svg.c \
	backend_xfig.c \
	bbox.c \
	bitmap_io.c \
	curve.c \
	decompose.c \
	flate.c \
	getopt.c \
	getopt1.c \
	greymap.c \
	lzw.c \
	main.c \
	potracelib.c \
	progress_bar.c \
	render.c \
	trace.c \
	trans.c

include $(BUILD_STATIC_LIBRARY)
