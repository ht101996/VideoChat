LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := P2PClient

LOCAL_SRC_FILES := P2PClient.cpp RTPWrapper.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc \
					$(LOCAL_PATH)/inc/inc \
					$(LOCAL_PATH)/inc/libavcodec \
					$(LOCAL_PATH)/inc/libavformat \
					$(LOCAL_PATH)/inc/libavutil \
					$(LOCAL_PATH)/inc/libpostproc \
					$(LOCAL_PATH)/inc/libswresample \
					$(LOCAL_PATH)/inc/libswscale \
					

LOCAL_LDLIBS := -llog -lz -lc -lm \

LOCAL_LDFLAGS += -L$(LOCAL_PATH)/libs \
				 -L$(LOCAL_PATH)/libs/libffmpeg \
				 -lavformat -lavcodec -lavutil -lswscale -lswresample -lp2p_interface -luftp -lrudp -lcms -lcmc -lorm -larcsoft_aes -lmsgcc -lmsgcs -losplatform -lJrtp -lJthread \


#LOCAL_LDLIBS := -Llibs -L$(SYSROOT)/usr/lib -lp2p_interface -luftp -lrudp -lcms -lcmc -lorm -larcsoft_aes -lmsgcc -lmsgcs -losplatform -lJrtp -lJthread -lffmpeg -llog -lstdc++ -lc -ldl -lm
#　移除不必要的符号引用，仅在实际需要的时候才连接，可以生成更高效的代码。
#LOCAL_LDFLAGS := -Wl,--as-needed
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS
include $(BUILD_SHARED_LIBRARY)
