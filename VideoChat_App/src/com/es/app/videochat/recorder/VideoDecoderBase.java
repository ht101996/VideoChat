package com.es.app.videochat.recorder;

public abstract class VideoDecoderBase {
	public abstract void decode(byte[] data, long timestamp);
}
