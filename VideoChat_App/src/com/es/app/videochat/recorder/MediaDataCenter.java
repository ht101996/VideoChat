package com.es.app.videochat.recorder;

import java.util.concurrent.ConcurrentLinkedQueue;

import android.util.Log;

import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;


public class MediaDataCenter{
	private static final String Tag = MediaDataCenter.class.getSimpleName();
	private static MediaDataCenter mInstance;// = new MediaDataCenter();
	
	private ConcurrentLinkedQueue<VideoFrameItem> VideoFrameItemQueue = null;//new ConcurrentLinkedQueue<VideoFrameItem>();
	private ConcurrentLinkedQueue<AudioStreamData> AudioStreamDataQueue = null;// new ConcurrentLinkedQueue<AudioStreamData>();
	private AudioDecoderBase mAudioDecoder;
	private VideoDecoderBase mVideoDecoder;
	private long audioPTS;
	public static MediaDataCenter getInstance(AudioDecoderBase audioDecoder, VideoDecoderBase videoDecoder) {
		if(mInstance == null)
			mInstance = new MediaDataCenter(audioDecoder, videoDecoder);
		return mInstance;
	}
	
	private MediaDataCenter(AudioDecoderBase audioDecoder, VideoDecoderBase videoDecoder) {
		this.mAudioDecoder = audioDecoder;
		this.mVideoDecoder = videoDecoder;
		VideoFrameItemQueue = new ConcurrentLinkedQueue<VideoFrameItem>();
		AudioStreamDataQueue = new ConcurrentLinkedQueue<AudioStreamData>();
		new AudioDecodeThread().start();
		new VideoDecodeThread().start();
	}
	
	public void addVideoFrame(VideoFrameItem item) {
		VideoFrameItemQueue.add(item);
	}
	public void addAudioData(AudioStreamData data) {
		AudioStreamDataQueue.add(data);
	}
	
	private class AudioDecodeThread extends Thread {
		@Override
		public void run() {
			while(true) {
				if(! AudioStreamDataQueue.isEmpty()) {
					try{
					AudioStreamData audioData = AudioStreamDataQueue.poll();
					audioPTS = (long)audioData.getTimestamp() * 1000000;
					mAudioDecoder.decode(audioData.getData(), audioPTS);
					}catch(Exception e) {
						e.printStackTrace();
						Log.e(Tag, "!!!!!!!!!!!!!! AudioDecodeThread error");
					}
				}
			}
		}
	}
	
	private class VideoDecodeThread extends Thread {
		@Override
		public void run() {
			while(true) {
				boolean  videoEmpty = VideoFrameItemQueue.isEmpty();
				if(! videoEmpty) {
					try{
					VideoFrameItem videoFrame = VideoFrameItemQueue.peek();
					long videoPTS = (long)videoFrame.getTimestamp() * 1000000; 
					long VAInterval = videoPTS - audioPTS;
					if(VAInterval > 500000){
						try{
							Thread.sleep(50);
						}catch(InterruptedException e){
							e.printStackTrace();
						}
						
						Log.d(Tag, "VAInterval > 500000, Video="+videoPTS+"   audio="+audioPTS);
					}
					else if(VAInterval < -500000) {
						VideoFrameItemQueue.poll();
						Log.d(Tag, "~~VAInterval < -500000, Video="+videoPTS+"   audio="+audioPTS);
					}
					else {
						Log.d(Tag, "500000 > VAInterval > -500000");
						videoFrame = VideoFrameItemQueue.poll();
						videoPTS = (long)videoFrame.getTimestamp() * 1000000; 
						mVideoDecoder.decode(videoFrame.getData(), videoPTS);
					}
						
					}catch(Exception e) {
						e.printStackTrace();
					}
				}
			}
		}
	}
	
	public static class VideoFrameItem {
		private byte[] data;
		private double timestamp;
		public VideoFrameItem(byte[] srcData, int offset, int length, double timestamp) {
			this.data = new byte[length];
			System.arraycopy(srcData, offset, this.data, 0, length);
			this.timestamp = timestamp;
		}
		
		public byte[] getData() {
			return data;
		}
		public void setData(byte[] srcData, int offset, int length) {
			if(this.data != null) 
				data = null;
			this.data = new byte[length];
			System.arraycopy(srcData, offset, this.data, 0, length);
		}
		public double getTimestamp() {
			return timestamp;
		}
		public void setTimestamp(double timestamp) {
			this.timestamp = timestamp;
		}
		
	}
	
	public static class AudioStreamData {
		private byte[] data;
		private double timestamp;
		public AudioStreamData(byte[] srcData, int offset, int length, double timestamp) {
			this.data = new byte[length];
			System.arraycopy(srcData, offset, this.data, 0, length);
			this.timestamp = timestamp;
		}
		
		public byte[] getData() {
			return data;
		}
		public void setData(byte[] srcData, int offset, int length) {
			if(this.data != null) 
				data = null;
			this.data = new byte[length];
			System.arraycopy(srcData, offset, this.data, 0, length);
		}
		public double getTimestamp() {
			return timestamp;
		}
		public void setTimestamp(double timestamp) {
			this.timestamp = timestamp;
		}
		
	}
}
