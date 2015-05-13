package com.arcsoft.ais.arcvc.fragment;

import java.io.IOException;
import java.util.List;

import android.app.Activity;
import android.app.Fragment;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import com.arcsoft.ais.arcvc.R;
import com.es.app.videochat.recorder.ESNativeH264Encoder;
import com.es.app.videochat.recorder.ESVideoQuality;
import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;

public class CameraFragment extends Fragment implements PreviewCallback {
	private final int previewFormat = ImageFormat.NV21;
	private final String Tag = this.getClass().getSimpleName();
	private int cameraID = CameraInfo.CAMERA_FACING_FRONT;
	private SurfaceView surface;
	private SurfaceHolder holder;
	private Camera camera;
	private Activity activity;
	private int previewWidth;
	private int previewHeight;
	private int previewFps;

	private ESNativeH264Encoder h264Encoder;
	private boolean isSendingData = false;
	
	public CameraFragment(int previewWidth, int previewHeight, int previewfps) {
		this.previewWidth = previewWidth;
		this.previewHeight = previewHeight;
		this.previewFps = previewfps;
	}
	
	@Override
	public void onAttach(Activity activity) {
		this.activity = activity;
		super.onAttach(activity);
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.camera_fragment, container, false);
		surface = (SurfaceView)view.findViewById(R.id.camera_surface);
		holder = surface.getHolder();
		holder.addCallback(new SurfaceHolder.Callback() {
			
			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				stopPreview();
			}
			
			@Override
			public void surfaceCreated(SurfaceHolder holder) {

				if(camera != null)
					try {
						camera.setPreviewDisplay(holder);
					} catch (IOException e) {
						e.printStackTrace();
					}
				startPreview();
			}
			
			@Override
			public void surfaceChanged(SurfaceHolder holder, int format, int width,
					int height) {
			}
		});
		return view;
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		initH264Encoder();
		super.onActivityCreated(savedInstanceState);
	}
	
	@Override
	public void onResume() {
		openCamera();
		super.onResume();
	}
	
	@Override
	public void onPause() {
		stopCamera();
		super.onPause();
	}
	
	@Override
	public void onDestroy() {
		super.onDestroy();
	}
	
	@Override
	public void onDetach() {
		super.onDetach();
	}

	private void openCamera(){
		camera = Camera.open(cameraID);
		setCameraDisplayOrientation(cameraID, camera);
		setCameraParameters();
		camera.setPreviewCallback(this);
	}
	
	private void stopCamera() {
		if(camera == null)
			return;
		stopPreview();
		camera.release();
		camera = null;
	}
	
	private void setCameraParameters() {
		Camera.Parameters param = camera.getParameters();
		param.setPreviewSize(previewWidth, previewHeight);
		param.setPreviewFormat( previewFormat);
		boolean locked = lockFPS(previewFps);
		Log.d(Tag, "Lock preview fps to "+previewFps+" return :"+locked);
		camera.setParameters(param);
	}
	/**
	 * lock preview data fps, called before startPreview
	 * @param fps fps to lock
	 * @return false if not supported
	 */
	private boolean lockFPS(int fps) {
		boolean ret = false;
		int fps1k = fps*1000;
		Parameters p = camera.getParameters();
		List<int[]> rangeList = p.getSupportedPreviewFpsRange();
		if (rangeList != null) {
			for (int[] is : rangeList) {
				if (is[0] == is[1] && is[0] == fps1k) {
					p.setPreviewFpsRange(fps1k, fps1k);
					camera.setParameters(p);
					ret = true;
					break;
				}
			}
		}
		
		return ret;
	}
	
	private void startPreview() {
		if(camera != null)
			camera.startPreview();
	}
	
	private void stopPreview() {
		if(camera == null)
			return;
		camera.stopPreview();
	}

	private void setCameraDisplayOrientation(int cameraId, Camera camera) {
		CameraInfo info = new android.hardware.Camera.CameraInfo();
		android.hardware.Camera.getCameraInfo(cameraId, info);
		int rotation = activity.getWindowManager().getDefaultDisplay()
				.getRotation();
		int degrees = 0;
		switch (rotation) {
		case Surface.ROTATION_0:
			degrees = 0;
			break;
		case Surface.ROTATION_90:
			degrees = 90;
			break;
		case Surface.ROTATION_180:
			degrees = 180;
			break;
		case Surface.ROTATION_270:
			degrees = 270;
			break;
		}

		int result;
		if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
			result = (info.orientation + degrees) % 360;
			result = (360 - result) % 360; // compensate the mirror
		} else { // back-facing
			result = (info.orientation - degrees + 360) % 360;
		}
		camera.setDisplayOrientation(result);
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		if(isSendingData)
			h264Encoder.putData(data,data.length);
	}
	 
	
	public synchronized void startSendData() {
		isSendingData = true;
		h264Encoder.start(true);
	}
	
	public synchronized void stopSendData() {
		isSendingData = false;
		h264Encoder.stop();
	}
	
	private void initH264Encoder() {
		h264Encoder = new ESNativeH264Encoder(previewFormat);
		h264Encoder.setEncoderListener(encoderListener);	
		ESVideoQuality videoQuality = new ESVideoQuality(previewWidth,previewHeight,previewFps,125000);
		h264Encoder.setupCodec(videoQuality);
		
		
	}
	private OnEncoderListener encoderListener;
	public void setEncoderListener(OnEncoderListener listener) {
		this.encoderListener = listener;
	}
}
