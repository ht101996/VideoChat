package com.arcsoft.videochat.codec;

public interface EncoderListener {
	public void onH264EncodeFinished(String type, byte[] data, long timestamp);
}

