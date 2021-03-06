package com.es.app.videochat.recorder;

import android.media.MediaCodecInfo;
import android.media.MediaCodecList;

public class MediaCodecUtils {
	public static MediaCodecInfo selectCodec(String mimeType) {
	        int numCodecs = MediaCodecList.getCodecCount();
	        for (int i = 0; i < numCodecs; i++) {
	            MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);

	            if (!codecInfo.isEncoder()) {
	                continue;
	            }

	            String[] types = codecInfo.getSupportedTypes();
	            for (int j = 0; j < types.length; j++) {
	                if (types[j].equalsIgnoreCase(mimeType)) {
	                    return codecInfo;
	                }
	            }
	        }
	        return null;
	    }
	    
}
