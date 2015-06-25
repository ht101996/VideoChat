package com.es.app.videochat.recorder;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.LinkedBlockingQueue;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Environment;
import android.sax.StartElementListener;
import android.util.Log;
import android.view.Surface;

public class H264Decoder extends VideoDecoderBase implements Runnable{
	private static final String Tag = H264Decoder.class.getSimpleName();
	private MediaCodec mediaCodec = null;  
	private MediaFormat mediaFormat = null;
	private Surface surface = null;
	private int width, height, frameRate;
//	int mCount = 0;
	private boolean stopped = true;
	public H264Decoder(int width, int height, int frameRate) {
		this.width = width;
		this.height = height;
		this.frameRate = frameRate;
				
	}
	
	public void setupDecoder(Surface surface) {
		 mediaCodec = MediaCodec.createDecoderByType("video/avc");
		 mediaFormat = MediaFormat.createVideoFormat("video/avc", width, height);
		 this.surface = surface;
//	     new Thread(this).start();
	}
	
	public void startDecoder() {
		 mediaCodec.configure(mediaFormat, surface, null, 0);
	     mediaCodec.start();
	     stopped = false;
	}
	public void stopDecoder() {
		if(mediaCodec != null)
			mediaCodec.stop();
		stopped = true;
	}
	
	public void releaseDecoder() {
		if(mediaCodec != null)
			mediaCodec.release();
	}
	
	public void resetDecoder() {
		if(mediaCodec == null)
			return;
		stopDecoder();
		startDecoder();
	}
	
	@Override
	public void run() {
//		byte[]  decodeByte = null;
//		long curTime = System.nanoTime();
//		while(!stopped) {
//			synchronized (inputByteQueue) {
//				try {
//					inputByteQueue.wait();
//					decodeByte = inputByteQueue.poll();
//				} catch (InterruptedException e) {
//					e.printStackTrace();
//				}
//			}
//			
//			if(decodeByte == null){
//				continue;
//			}
//				
//			ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
//			int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
//			if (inputBufferIndex >= 0) {
//				ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
//				inputBuffer.clear();
//				inputBuffer.put(decodeByte, 0, decodeByte.length);
//				mediaCodec.queueInputBuffer(inputBufferIndex, 0, decodeByte.length,
//						mCount * 1000000 / frameRate, 0);
//				mCount++;
//			}
//
//			MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
//			int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
//			while (outputBufferIndex >= 0) {
//				curTime = System.nanoTime();
//				mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
//				outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
//			}
//		}
	}
	
	
//	public void onFrame(byte[] buf, int offset, int length, double timestamp) {
////		byte[] newRevData = new byte[length];
////		System.arraycopy(buf, offset, newRevData, 0, length);
////		synchronized (inputByteQueue) {
////			inputByteQueue.add(newRevData);
////			inputByteQueue.notifyAll();
////		}
//		
//		ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
//		int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
//		if (inputBufferIndex >= 0) {
//			ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
//			inputBuffer.clear();
//			inputBuffer.put(buf, offset, length);
//			mediaCodec.queueInputBuffer(inputBufferIndex, 0, length,
//					mCount * 1000000 / frameRate, 0);
//			mCount++;
//		}
//
//		MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
//		int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
//		while (outputBufferIndex >= 0) {
////			TimeMonitor.videoDecodePresentationTimeUs = bufferInfo.presentationTimeUs;
//			mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
//			outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
//		}
//	}

	@Override
	public void decode(byte[] data, long timestamp) {
		
		ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
		int inputBufferIndex = mediaCodec.dequeueInputBuffer(0);
		Log.d(Tag,"H264Decoder, input buffer index= " + inputBufferIndex+", input data length:"+data.length+", timestamp:"+timestamp);
		if (inputBufferIndex >= 0) {
			ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
			inputBuffer.clear();
			inputBuffer.put(data, 0, data.length);
			mediaCodec.queueInputBuffer(inputBufferIndex, 0, data.length,
					timestamp, 0);
		}

		MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
		int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
		while (outputBufferIndex >= 0) {
//			System.out.println("<--cxd, decode video in java presentationTimeUs:"+(float)bufferInfo.presentationTimeUs/1000000);
			mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
			outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
		}
	}
}
