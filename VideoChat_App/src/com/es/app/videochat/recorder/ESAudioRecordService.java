package com.es.app.videochat.recorder;

import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;
import com.es.app.videochat.recorder.ESRecordListener.OnRecordServiceListener;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

public class ESAudioRecordService implements OnEncoderListener {

	public final String TAG = "ES.ESAudioRecordService";
	
	ESAudioQuality audioQuality;
	Thread recordThread = null;
	ESAudioEncoder audioEncoder = null;
	
	OnRecordServiceListener recordServiceListener;
	
	boolean pause = false;
	
	public ESAudioRecordService()
	{
		
	}
	
	public void startRecord(ESAudioQuality audioQuality)
	{
		if(null == audioQuality)
			return;
		
		this.audioQuality = audioQuality.clone();
		
		audioEncoder = this.openAudioEncoder();
		
		if(audioEncoder != null)
		{
			
			audioEncoder.setEncoderListener(this);
			
						
			recordThread = new Thread(new Runnable() {
				
				@Override
				public void run() {
					// TODO Auto-generated method stub
					
					android.os.Process
					.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

					final int bufferSize = AudioRecord.getMinBufferSize(44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT)*2;
					AudioRecord audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, bufferSize);
					
					if(audioRecord != null)
					{
						audioRecord.startRecording();
						
						audioEncoder.start();
						
						byte[] data = new byte[bufferSize];
						
						while (!Thread.interrupted()) {
							
							if(isPause())
							{
								try {
									Thread.sleep(1000);
								} catch (Exception e) {
	
								}
								continue;
							}
							
							int length = audioRecord.read(data,0,bufferSize);
							if (length ==  AudioRecord.ERROR_INVALID_OPERATION || length == AudioRecord.ERROR_BAD_VALUE)
							{
								Log.e(TAG,"An error occured with the AudioRecord API !");
								try {
									Thread.sleep(500);
								} catch (Exception e) {
									// TODO: handle exception
								}
							}
							else
							{
								audioEncoder.putData(data,length);
							}
						}
						audioRecord.stop();
						audioEncoder.stop();
					}
				}
			});
			recordThread.start();
		}
	}
	
	
	private ESAudioEncoder openAudioEncoder()
	{
		if (android.os.Build.VERSION.SDK_INT < 16)//OS below 4.1.2
		{
			
		}
		else {
			try {
				ESNativeAACEncoder nativeEncoder = new ESNativeAACEncoder();
				if(nativeEncoder.setupCodec(audioQuality))
				{
					return nativeEncoder;
				}
				else {
					// Doesn't support
					
				}
			} catch (Exception e) {
				e.printStackTrace();
			}			
		}
        return null;
	}

	@Override
	public void onEncodeFinished(byte[] data, int length) {
		// TODO Auto-generated method stub
		
		if(recordServiceListener != null)
			recordServiceListener.onEncodeFinished(ESRecordListener.AUDIO_RECORD, data, length);
	}
	
	public void stop()
	{
		if(audioEncoder != null)
			audioEncoder.stop();
		
		try {
			recordThread.interrupt();
			recordThread.join();
			
		} catch (Exception e) {
			// TODO: handle exception
		}
	}

	public synchronized boolean isPause() {
		return pause;
	}

	public synchronized void setPause(boolean pause) {
		this.pause = pause;
	}

	public void setRecordServiceListener(
			OnRecordServiceListener recordServiceListener) {
		this.recordServiceListener = recordServiceListener;
	}
}
