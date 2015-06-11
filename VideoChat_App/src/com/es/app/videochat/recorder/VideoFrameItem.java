package com.es.app.videochat.recorder;

public class VideoFrameItem {
	private byte[] dataBytes;
	private long timeStamp;//at milliseconds 
	
	public VideoFrameItem(byte[] data, long time) {
		dataBytes = new byte[data.length];
		System.arraycopy(data, 0, dataBytes, 0, data.length);
		timeStamp = time;
	}
	
	public byte[] getDataBytes() {
		return dataBytes;
	}
	public void setDataBytes(byte[] dataBytes) {
		this.dataBytes = dataBytes;
	}
	public long getTimeStamp() {
		return timeStamp;
	}
	public void setTimeStamp(long timeStamp) {
		this.timeStamp = timeStamp;
	}
	
	

}
