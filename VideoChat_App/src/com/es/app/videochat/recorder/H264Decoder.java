package com.es.app.videochat.recorder;

import java.nio.ByteBuffer;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;

public class H264Decoder {
	private MediaCodec mediaCodec = null;  
	
	private int width, height, frameRate;
	int mCount = 0;
	public H264Decoder(int width, int height, int frameRate) {
		this.width = width;
		this.height = height;
		this.frameRate = frameRate;
				
	}
	
	public void setupDecoder(Surface surface) {
		 mediaCodec = MediaCodec.createDecoderByType("video/avc");
	     MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", width, height);
	     mediaCodec.configure(mediaFormat, surface, null, 0);
	     mediaCodec.start();
	     
	}
	
	public void stopDecoder() {
		if(mediaCodec != null)
			mediaCodec.stop();
	}
	
	public void releaseDecoder() {
		if(mediaCodec != null)
			mediaCodec.release();
	}
	public void onFrame(byte[] buf, int offset, int length, int flag) {
		ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
		int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
		if (inputBufferIndex >= 0) {
			ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
			inputBuffer.clear();
			inputBuffer.put(buf, offset, length);
			mediaCodec.queueInputBuffer(inputBufferIndex, 0, length,
					mCount * 1000000 / frameRate, 0);
			mCount++;
		}

		MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
		int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
		while (outputBufferIndex >= 0) {
			mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
			outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
		}
	}
}
