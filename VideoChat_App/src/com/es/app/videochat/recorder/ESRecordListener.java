package com.es.app.videochat.recorder;

public class ESRecordListener {
	
	public static final int VIDEO_RECORD = 1;
	public static final int AUDIO_RECORD = 2;
	
	public interface OnEncoderListener {
		public void onEncodeFinished(byte[] data, long timestamp);
	}
	
	public interface OnRecordServiceListener {
		public void onEncodeFinished(int recordType, byte[] data, int length);
	}
}
