package com.es.app.videochat.recorder;

import java.io.File;
import java.nio.ByteBuffer;

import android.annotation.SuppressLint;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Environment;

@SuppressLint("NewApi")
public class MediaMuxerWrapper {
	private static String outputPath = Environment.getExternalStorageDirectory().getPath().concat(File.separator).concat("muxerText.mp4");
	private static MediaMuxer mMuxer;
	static int videoTrackIndex,audioTrackIndex;
	static int  state = 0;
	static{
		try{
			mMuxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
		}catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void setVideoTrack(MediaFormat videoFormat) {
		videoTrackIndex = mMuxer.addTrack(videoFormat);
		state = state|0x1;
	}

	public static void setAudioTrack(MediaFormat audioFormat) {
		audioTrackIndex = mMuxer.addTrack(audioFormat);
		state = state|0x2;
	}

	public static void start() {
//		if((state&0x3) == 0x3) 
			mMuxer.start();
	}
	
	public static void stop() {
		mMuxer.stop();
		mMuxer.release();
		state = 0;
	}
	
	public static void setVideoData(ByteBuffer data, BufferInfo info) {
		mMuxer.writeSampleData(videoTrackIndex, data, info);
	}
	
	public static void setAudioData(ByteBuffer data, BufferInfo info) {
		mMuxer.writeSampleData(audioTrackIndex, data, info);
	}
}
