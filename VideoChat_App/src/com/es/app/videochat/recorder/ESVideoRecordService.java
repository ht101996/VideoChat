package com.es.app.videochat.recorder;

import java.io.IOException;

import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;
import com.es.app.videochat.recorder.ESRecordListener.OnRecordServiceListener;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.PreviewCallback;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;

public class ESVideoRecordService implements PreviewCallback, Callback, OnEncoderListener{

	public final String TAG = "ES.ESVideoRecordService";
	
	Camera camera;
	SurfaceHolder surfaceHolder;
	ESVideoEncoder videoEncoder;
	ESVideoQuality videoQuality = null;
	
	OnRecordServiceListener recordServiceListener;
	
	
    
    
	public void startRecord(SurfaceHolder surfaceHolder,ESVideoQuality videoQuality)
	{
		if(videoQuality == null)
			return;
		
		this.videoQuality = videoQuality;
		
		this.surfaceHolder = surfaceHolder;
		this.surfaceHolder.addCallback(this);
	}
	
	private void openCamera(int cameraID) throws IOException
	{
		if(null == camera)
		{
			camera = Camera.open(cameraID); 
	        Camera.Parameters parameters = camera.getParameters();  
	        parameters.setPreviewSize(videoQuality.resX, videoQuality.resY);  
	        parameters.setPictureSize(videoQuality.resX, videoQuality.resY);  
	        parameters.setPreviewFormat(ImageFormat.NV21);  
	        camera.setParameters(parameters);   
	        camera.setPreviewCallback(this);  
	        camera.setDisplayOrientation(videoQuality.orientation);
		}
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		
		videoEncoder = ESVideoRecordService.openVideoEncoder(videoQuality);
		if(videoEncoder != null)
		{
			videoEncoder.setEncoderListener(this);
			videoEncoder.start(true);
		}
		
		try {
            
            this.openCamera(CameraInfo.CAMERA_FACING_FRONT);
            if(camera != null)
            {
            	camera.setPreviewDisplay(this.surfaceHolder);  
            	camera.startPreview();
            }
               
		} catch (Exception e) {
			// TODO: handle exception
		}
		
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {

		camera.setPreviewCallback(null); //！！这个必须在前，不然退出出错  
		camera.stopPreview();   
		camera.release();  
		camera = null;  
		
		if(videoEncoder != null)
		{
			videoEncoder.stop();
			videoEncoder = null;
		}
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		// TODO Auto-generated method stub
		if(videoEncoder != null)
		{
			videoEncoder.putData(data,data.length);
		}
	}
	
	private static final ESVideoEncoder openVideoEncoder(ESVideoQuality videoQuality)
	{
		if (android.os.Build.VERSION.SDK_INT < 16)//OS below 4.1.2
		{
			
		}
		else {
			try {
				ESNativeH264Encoder nativeEncoder = new ESNativeH264Encoder(ImageFormat.NV21);
				if(nativeEncoder.setupCodec(videoQuality))
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
		{
			recordServiceListener.onEncodeFinished(ESRecordListener.VIDEO_RECORD, data, length);
		}
	}

	public OnRecordServiceListener getRecordServiceListener() {
		return recordServiceListener;
	}

	public void setRecordServiceListener(
			OnRecordServiceListener recordServiceListener) {
		this.recordServiceListener = recordServiceListener;
	}
}
