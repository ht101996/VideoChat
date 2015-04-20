package com.arcsoft.ais.arcvc.activity;

import java.io.FileInputStream;

import android.app.Activity;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.service.CameraConfigureThread;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Global;

public class MainActivity extends Activity {

	private SurfaceView sv;
	private ImageView iv;
	private static MediaRecorder mMediaRecorder;
	Camera frontCamera;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		sv = (SurfaceView) findViewById(R.id.surfaceView1);
		iv = (ImageView) findViewById(R.id.imageView1);
		sv.setVisibility(View.GONE);
//		new CameraConfigureThread(sv).start();
		CameraUtils.getFrontCamera();
	}

	private Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			// Bitmap image =
			// MyBitmap.createMyBitmap(msg.getData().getByteArray("packet"),
			// 176, 144);
			// iv.setImageBitmap(image);
		}
	};

	class DisplayThread extends Thread {

		public void run() {
			for (int i = 0; i < 277; i++) {
				try {
					FileInputStream fis = new FileInputStream("/storage/emulated/0/Android/data/com.arcsoft.ais.arcvc/out/out" + i);
					// ByteArrayOutputStream out = new ByteArrayOutputStream();
					byte[] b = new byte[fis.available()];
					fis.read(b);
					// YuvImage yuvImage = new YuvImage(b, ImageFormat.NV21,
					// 200, 200, null);
					// yuvImage.compressToJpeg(new Rect(0, 0, 200, 200), 50,
					// out);
					// byte[] imageBytes = out.toByteArray();
					// Bitmap image = MyBitmap.createMyBitmap(b, 176, 144);
					// Bitmap image = BitmapFactory.decodeByteArray(b, 0,
					// b.length);

					// Canvas canvas = new Canvas(image);
					// sv.draw(canvas);

					Message m = new Message();
					m.what = 2;
					Bundle bundle = new Bundle();
					bundle.putByteArray("packet", b);
					m.setData(bundle);
					getHandler().sendMessage(m);

					fis.close();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				try {
					Thread.sleep(50l);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	public Handler getHandler() {
		return handler;
	}

	@Override
	protected void onDestroy() {
//		mMediaRecorder.stop();
//		mMediaRecorder.reset();
//		mMediaRecorder.release();
//		frontCamera.release();
		Log.i(Global.TAG, "MainActivity: onDestroy>>>...");
		super.onDestroy();
	}

	public void onClickEvent(View view) {
//		frontCamera = CameraUtils.getFrontCamera();
//		mMediaRecorder = new MediaRecorder();
//		frontCamera.unlock();
//		mMediaRecorder.setCamera(frontCamera);
//		mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
//		 mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
//		mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
//		mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
//		 mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
//		mMediaRecorder.setVideoFrameRate(15);
////		mMediaRecorder.setVideoSize(176, 144);
//		mMediaRecorder.setPreviewDisplay(sv.getHolder().getSurface());
//		mMediaRecorder.setMaxDuration(1000);
//		mMediaRecorder.setMaxFileSize(0);
//		// mMediaRecorder.setOutputFile(sender.getFileDescriptor());
//		mMediaRecorder.setOutputFile("/sdcard/stream.3gp");
//		try {
//			mMediaRecorder.prepare();
//			mMediaRecorder.start();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
	}

}
