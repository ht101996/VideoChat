package com.es.app.videochat.recorder;

import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;

public abstract class ESAudioEncoder {
	

	OnEncoderListener encoderListener;
	
	abstract public boolean setupCodec(ESAudioQuality audioQuality);
	abstract public void start();
	abstract public void stop();
	abstract public void putData(byte[] data, int length);
	
	public OnEncoderListener getEncoderListener() {
		return encoderListener;
	}
	public void setEncoderListener(OnEncoderListener encoderListener) {
		this.encoderListener = encoderListener;
	}
	
	
}
