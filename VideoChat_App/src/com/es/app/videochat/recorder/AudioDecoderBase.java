package com.es.app.videochat.recorder;

public abstract class AudioDecoderBase {
	public abstract void decode(byte[] data, long timestamp);
}
