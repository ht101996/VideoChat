package com.es.app.videochat.recorder;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import com.es.app.videochat.recorder.ESRecordListener.OnRecordServiceListener;

import android.view.SurfaceHolder;

public class ESRecorderMgr implements OnRecordServiceListener {
	
	public final static String TAG = "ES.ESRecorderMgr";
	// video
	
	ESVideoRecordService videoRecordService = null;
	ESAudioRecordService audioRecordService = null;
	
	ESVideoQuality videoQuality = ESVideoQuality.DEFAULT_VIDEO_QUALITY.clone();
	ESAudioQuality audioQuality = ESAudioQuality.DEFAULT_AUDIO_QUALITY.clone();
	
	DatagramSocket socket;  
    InetAddress address;
	
	public void startRecord(SurfaceHolder surfaceHolder)
	{
		try {
			socket = new DatagramSocket();  
	        address = InetAddress.getByName("172.17.103.152");
		} catch (Exception e) {
			// TODO: handle exception
		}
		
		videoRecordService = new ESVideoRecordService();
		if(videoRecordService != null)
		{
			videoRecordService.setRecordServiceListener(this);
			videoRecordService.startRecord(surfaceHolder, videoQuality);
		}
		
		audioRecordService = new ESAudioRecordService();
		if(audioRecordService != null)
		{
			audioRecordService.setRecordServiceListener(this);
			audioRecordService.startRecord(audioQuality);
		}
	}

	@Override
	public void onEncodeFinished(int recordType, byte[] data, int length) {
		// TODO Auto-generated method stub
		switch (recordType) {
		case ESRecordListener.VIDEO_RECORD:
		{
			try {
				DatagramPacket packet=new DatagramPacket(data,length, address,5000);  
				System.out.println("..cxd, send video data leng:"+length);
				socket.send(packet);
			} catch (Exception e) {
				// TODO: handle exception
			}
		}
			
			break;
		case ESRecordListener.AUDIO_RECORD:
		{
			try {
				DatagramPacket packet=new DatagramPacket(data,length, address,5000);  
				System.out.println("..cxd, send andio data leng:"+length);
				socket.send(packet);
			} catch (Exception e) {
				// TODO: handle exception
			}
		}
			break;
		default:
			break;
		}
	}
	
}
