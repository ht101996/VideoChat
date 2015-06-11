package com.es.app.videochat.recorder;

public class TimeMonitor {
	public static long videoEncodePresentationTimeUs;
	public static long audioEncodePresentationTimeUs;
	
	public static void printEncodeInterval() {
		float interval =  ((float)(audioEncodePresentationTimeUs - videoEncodePresentationTimeUs))/1000;
		System.out.println(">>--cxd, Encode time interval(audio-video) ms:"+interval);
	}
	
	public static long videoDecodePresentationTimeUs;
	public static long audioDecodePresentationTimeUs;
	
	public static void printDecodeInterval() {
		float interval =  ((float)(audioDecodePresentationTimeUs - videoDecodePresentationTimeUs))/1000;
		System.out.println(">>--cxd, Decode time interval(audio-video) ms:"+interval);
	}
}
