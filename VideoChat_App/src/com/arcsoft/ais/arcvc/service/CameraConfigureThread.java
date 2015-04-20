package com.arcsoft.ais.arcvc.service;

import com.arcsoft.ais.arcvc.utils.CameraUtils;

import android.content.Context;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.view.SurfaceView;

public class CameraConfigureThread extends Thread {

	private SurfaceView sv;

	public CameraConfigureThread(SurfaceView sv) {
		this.sv = sv;
	}

	@Override
	public void run() {
		Camera frontCamera = CameraUtils.getFrontCamera();
		MediaRecorder mMediaRecorder = new MediaRecorder();
		frontCamera.unlock();
		mMediaRecorder.setCamera(frontCamera);
		mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
		// mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
		mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
		mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
		// mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
		mMediaRecorder.setVideoFrameRate(15);
		// mMediaRecorder.setVideoSize(176, 144);
		mMediaRecorder.setPreviewDisplay(sv.getHolder().getSurface());
		mMediaRecorder.setMaxDuration(1000);
		mMediaRecorder.setMaxFileSize(0);
		// mMediaRecorder.setOutputFile(sender.getFileDescriptor());
		mMediaRecorder.setOutputFile("/sdcard/stream.h264");
		try {
			mMediaRecorder.prepare();
			mMediaRecorder.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		mMediaRecorder.stop();
		mMediaRecorder.reset();
		mMediaRecorder.release();
		frontCamera.release();
	}

}
