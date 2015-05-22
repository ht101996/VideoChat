 package com.arcsoft.videochat.mediarecorder;

import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.SocketUtils;

import android.media.MediaRecorder;
import android.os.Environment;
import android.util.Log;

public class AudioRecorderWrapper {
	private MediaRecorder audiaRecorder = null;
	private String savePath;
	
	public boolean startRecorder(){
		try{
			if(prepareRecorder())
				audiaRecorder.start();
			}catch(IllegalStateException  e){
				e.printStackTrace();
				return false;
			}
		return true;
	}
	
	public boolean stopRecorder(){
		try{
			if(audiaRecorder != null)
				audiaRecorder.stop();
			}catch(RuntimeException e) {
				e.printStackTrace();
				return false;
			}
		return true;
	}
	
	public boolean releaseRecorder(){
		try{
			if(audiaRecorder != null)
				audiaRecorder.reset();
				audiaRecorder.release();
				audiaRecorder = null;
			}catch(RuntimeException e) {
				e.printStackTrace();
				return false;
			}
		return true;
	} 
	
	private String getAudioFile() {
		savePath = Environment.getExternalStorageDirectory()
				.getPath().concat("/testaudio")
				.concat(String.valueOf(System.currentTimeMillis())).concat(".acc");
		return savePath;
	}
	
    private boolean prepareRecorder(){
        // BEGIN_INCLUDE (configure_media_recorder)
    	if(audiaRecorder == null)
    		audiaRecorder = new MediaRecorder();
    	audiaRecorder.reset();

        // Step 2: Set sources
//    	audiaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
//    	audiaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
//    	audiaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
//    	audiaRecorder.setOutputFile(getAudioFile());

    	
    	audiaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
    	audiaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
    	audiaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
    	audiaRecorder.setAudioChannels(1);
    	audiaRecorder.setAudioEncodingBitRate(128000);//125*1024
    	audiaRecorder.setAudioSamplingRate(8000);//8000  44100

		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getReceiverSocket()==" + SocketUtils.getAudioSenderSocket());
		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getSenderSocket()==" + SocketUtils.getAudioSenderSocket());
		audiaRecorder.setOutputFile(SocketUtils.getAudioSenderSocket().getFileDescriptor());
        try {
        	audiaRecorder.prepare();
        } catch(Exception e) {
        	e.printStackTrace();
        	return false;
        }
        
        return true;
    }
    

}
