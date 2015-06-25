package com.es.app.videochat.recorder;

import java.util.concurrent.ConcurrentLinkedQueue;

import android.util.Log;

import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;


public class MediaDataCenter{
	private final static String tag = MediaDataCenter.class.getSimpleName();
	public interface DataDecodeListener {
		public void onAudioDataDecoding(byte[] data, long timestampUs);
		public void onVideoDataDecoding(byte[] data, long timestampUs);
	}
	
	private static final String Tag = MediaDataCenter.class.getSimpleName();
	private static MediaDataCenter mInstance;// = new MediaDataCenter();
	
	private ConcurrentLinkedQueue<VideoFrameItem> VideoFrameItemQueue = null;//new ConcurrentLinkedQueue<VideoFrameItem>();
	private ConcurrentLinkedQueue<AudioStreamData> AudioStreamDataQueue = null;// new ConcurrentLinkedQueue<AudioStreamData>();
	private DataDecodeListener decodeListener;
	private boolean videoThreadStop = true;
	private boolean audioThreadStop = true;
	private boolean synVideoWithAudio = true;
	private long audioPTS;
	private VideoDecodeThread videoDecoderThread;
	public static MediaDataCenter getInstance(DataDecodeListener decodeListener) {
		if(mInstance == null)
			mInstance = new MediaDataCenter(decodeListener);
		return mInstance;
	}
	
	public static MediaDataCenter getInstance(DataDecodeListener decodeListener, boolean synVideoWithAudio) {
		if(mInstance == null)
			mInstance = new MediaDataCenter(decodeListener, synVideoWithAudio);
		return mInstance;
	}
	
	private MediaDataCenter(DataDecodeListener decodeListener) {
		this.decodeListener = decodeListener;
		VideoFrameItemQueue = new ConcurrentLinkedQueue<VideoFrameItem>();
		AudioStreamDataQueue = new ConcurrentLinkedQueue<AudioStreamData>();
		
	}
	
	private MediaDataCenter(DataDecodeListener decodeListener, boolean synVideoWithAudio) {
		this.decodeListener = decodeListener;
		this.synVideoWithAudio = synVideoWithAudio;
		VideoFrameItemQueue = new ConcurrentLinkedQueue<VideoFrameItem>();
		AudioStreamDataQueue = new ConcurrentLinkedQueue<AudioStreamData>();
		
	}
	
	public void start() {
		videoThreadStop = false;
		audioThreadStop = false;
		videoDecoderThread = new VideoDecodeThread("VideoDecodeThread");
		videoDecoderThread.start();
		new AudioDecodeThread().start();
	}
	
	public void stop() {
		videoThreadStop = true;
		audioThreadStop = true;
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
			while(!audioThreadStop) {
				if(! AudioStreamDataQueue.isEmpty()) {
					AudioStreamData audioData = AudioStreamDataQueue.poll();
					audioPTS = (long)audioData.getTimestamp() * 1000000;
//					mAudioDecoder.decode(audioData.getData(), audioPTS);
					if(decodeListener != null)
						decodeListener.onAudioDataDecoding(audioData.getData(), audioPTS);
				}
			}
		}
	}
	
	private class VideoDecodeThread extends Thread {
		public VideoDecodeThread(String name) {
			super(name);
		}
		@Override
		public void run() {
			while(!videoThreadStop) {
				boolean  videoEmpty = VideoFrameItemQueue.isEmpty();
				if(videoEmpty) {
					try{
						Thread.sleep(50);
					}catch(InterruptedException e){
						e.printStackTrace();
					}
					continue;
				}
				if(synVideoWithAudio) {
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
						if(decodeListener != null)
							decodeListener.onVideoDataDecoding(videoFrame.getData(), videoPTS);
					}
				}
				else {
					VideoFrameItem videoFrame = VideoFrameItemQueue.poll();
					long videoPTS = (long)videoFrame.getTimestamp() * 1000000; 
					if(decodeListener != null)
						decodeListener.onVideoDataDecoding(videoFrame.getData(), videoPTS);
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
