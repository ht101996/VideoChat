package com.arcsoft.videochat.mediarecorder;

import android.media.MediaPlayer;

public class AudioPlayerWrapper {
	private MediaPlayer audiaPlayer = null;

    public boolean playAudio(String path) {
    	if(audiaPlayer == null)
    		audiaPlayer = new MediaPlayer();
    	audiaPlayer.reset();
    	
        try{  
        	audiaPlayer.setDataSource(path);
        	audiaPlayer.prepare();  
        	audiaPlayer.start();  
        }catch(Exception e){
        	e.printStackTrace();
        	return false;
        }  
        return true;
    }
    
	public boolean stopAudio() {
		try {
			if (audiaPlayer != null) {
				audiaPlayer.stop();
			}
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public boolean releasePlayer(){
		try{
			if(audiaPlayer != null)
				audiaPlayer.release();
				audiaPlayer = null;
			}catch(RuntimeException e) {
				e.printStackTrace();
				return false;
			}
		return true;
	} 

}
