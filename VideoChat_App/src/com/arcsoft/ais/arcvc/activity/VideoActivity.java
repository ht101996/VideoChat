package com.arcsoft.ais.arcvc.activity;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.ScrollView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.adapter.DialogAdapter;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.DensityUtil;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.MyBitmap;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.ais.arcvc.utils.SocketUtils;
import com.arcsoft.ais.arcvc.utils.audio.AudioWrapper;
import com.arcsoft.ais.arcvc.utils.audio.receiver.AudioPlayer;
import com.arcsoft.videochat.mediarecorder.AudioRecorderWrapper;



public class VideoActivity extends Activity implements View.OnClickListener {

	static Button button_pause ;
	static ScrollView mScrollView;
	static String friendNickname;
	static String currentNickname;
	static String startCameraRecordingFlag;
	static String remotePeerId;
	static String currentPeerId;
	static List<String> peerIdsOfFriends;
	static ImageView iv_friend;
	static SurfaceView surfaceView_myself;
	static DialogAdapter dialogAdapter;
	private static Camera myCamera;
	private static MediaRecorder mMediaRecorder;
	private AudioRecorderWrapper mAudioRecorderWrapper;
//	private static MediaRecorder mMediaAudioRecorder;
//	private static boolean usingCam = false;
	private static Dialog applyAlertDialog;
	private static AlertDialog.Builder alertDialogBuilder;
	private static AudioWrapper audioWrapper;
	boolean isFriendVideoZoomIn = false ;
	boolean isMyselfVideoZoomIn = false ;
	static int displayRotationDegree = 0 ;
	static long friendUserId;
	static ArrayList<String> friendPeerIds;
	private static WakeLock mWakeLock ;
	
	private static AudioPlayer audioPlayer = AudioPlayer.getInstance();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_video);
		surfaceView_myself = (SurfaceView) findViewById(R.id.surfaceView1);
		iv_friend = (ImageView) findViewById(R.id.imageView1);
//		button_pause = (Button) findViewById(R.id.button_pause);
		// myCamera = CameraUtils.getFrontCamera();
		// myCamera.setDisplayOrientation(90);
		Intent intent = getIntent();
		friendUserId = intent.getLongExtra("friendUserId", 0l);
		friendNickname = intent.getStringExtra("friendNickname");
		startCameraRecordingFlag = intent.getStringExtra("startCameraRecordingFlag");
		currentNickname = Configer.getValue("nickname");
		friendPeerIds = intent.getStringArrayListExtra("friendPeerIds");

		setTitle(getTitle()+" with "+friendNickname);
		
		displayRotationDegree = getWindowManager().getDefaultDisplay()
	                .getRotation();
		 
		dialogAdapter = new DialogAdapter(this);
		Log.i(Global.TAG, "friendUserId= " + friendUserId);
		Log.i(Global.TAG, "friendNickname= " + friendNickname);
		for (String friendPeerId : friendPeerIds) {
			Log.i(Global.TAG, "friendPeerId= " + friendPeerId);
		}
		currentPeerId = Configer.getValue("peer_id");
		if (friendPeerIds == null || friendPeerIds.size() < 1) {
			Log.i(Global.TAG, "friendPeerIds.size() == 0>>>...error !!");
		} else {
			peerIdsOfFriends = friendPeerIds;
			remotePeerId = friendPeerIds.get(0);
			Log.i(Global.TAG, "friendPeerIds.get(0)>>>..." + friendPeerIds.get(0));
		}
		Log.i(Global.TAG, "remotePeerId>>>..." + remotePeerId);
		// startRTPSession
		Log.i(Global.TAG, "P2PClient.startRTPSession() >>>.start..");
		P2PClientManager.getP2PClientInstance().startRTPSession(remotePeerId);
		Log.i(Global.TAG, "P2PClient.startRTPSession() sucessfully");

		P2PClientManager.getP2PClientInstance().addHandler(handler);

		mScrollView = (ScrollView) findViewById(R.id.scrollContent);
		// mScrollView.setVerticalScrollBarEnabled(false);//禁用垂直滚动条
		mScrollView.setOnTouchListener(touchListener);
		// mScrollView.setVerticalScrollBarEnabled(false);
		// mScrollView.setHorizontalScrollBarEnabled(false);
		alertDialogBuilder = new AlertDialog.Builder(VideoActivity.this);
		audioWrapper = AudioWrapper.getInstance();
		
		//保持屏幕常亮 
		PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE); 
		mWakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "LOCK_TAG"); 
		
		initMediaConfig();
		Log.i(Global.TAG, "VideoActivity: onCreate============finished!");
	}

	OnTouchListener touchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_MOVE) {

				// TODO Auto-gener
				InputMethodManager imm = (InputMethodManager) getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
				/*
				 * // 显示或者隐藏输入法 imm.toggleSoftInput(0,
				 * InputMethodManager.HIDE_NOT_ALWAYS);
				 */
				if (imm.isActive()) {
					imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
				}
			}

			return false;
		}

	};

	/** Check if this device has a camera */
	private static boolean checkCameraHardware(Context context) {
		Log.i(Global.TAG, "VideoActivity: Camera.getNumberOfCameras()============" + Camera.getNumberOfCameras());
		if (context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)) {
			// this device has a camera
			return true;
		} else {
			// no camera on this device
			return false;
		}
	}

	@Override
	protected void onStart() {
		super.onStart();
		Log.i(Global.TAG, "VideoActivity --------------onStart!");
		P2PClientManager.getP2PClientInstance().addHandler(handler);
	};

	@Override
	protected void onResume() {
		super.onResume();
		Log.i(Global.TAG, "VideoActivity --------------onResume!");
		P2PClientManager.getP2PClientInstance().addHandler(handler);
		if(null != mWakeLock)
			mWakeLock.acquire(); 
	};

	@Override
	protected void onPause() {
		super.onPause();
		Log.i(Global.TAG, "VideoActivity --------------onPause!");
		P2PClientManager.getP2PClientInstance().removeHandler(handler);
		releaseMediaRecorder();
		releaseCamera();
		//释放屏幕常亮锁
		if(null != mWakeLock) {
		    mWakeLock.release();
		}
		mAudioRecorderWrapper.releaseRecorder();
	};

	@Override
	protected void onStop() {
		super.onStop();
		Log.i(Global.TAG, "VideoActivity --------------onStop!");
		P2PClientManager.getP2PClientInstance().removeHandler(handler);
		releaseMediaRecorder();
		releaseCamera();
	};

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		Log.i(Global.TAG, "VideoActivity --------------onPostCreate!");
		super.onPostCreate(savedInstanceState);
		if (Global.START_CAMERA_RECORDING_FLAG_YES.equals(startCameraRecordingFlag)) {
			new StartCameraRecording().start();
		}
    }

	private void initMediaConfig() {
		mAudioRecorderWrapper = new AudioRecorderWrapper();
	}
	class StartCameraRecording extends Thread{
		@Override
		public void run() {
			startCameraRecording();
		}

	}

	@Override
	public void onClick(View v) {

		// 这里必须要给一个延迟，如果不加延迟则没有效果。我现在还没想明白为什么
		handler.postDelayed(new Runnable() {

			@Override
			public void run() {
				// 将ScrollView滚动到底
				mScrollView.fullScroll(View.FOCUS_DOWN);
				// mScrollView.setVerticalScrollBarEnabled(true);
			}
		}, 100);

	}

	@Override
	protected void onDestroy() {
		Log.i(Global.TAG, " --------------onDestroy!");
		try {
			// P2PClientManager.getP2PClientInstance().uninit();
			P2PClientManager.getP2PClientInstance().removeHandler(handler);
		} catch (Exception e) {
			e.printStackTrace();
		}
		SocketUtils.releaseLocalSocket();
		releaseMediaRecorder();
		releaseCamera();
		super.onDestroy();
	}

	@Override
	public void onBackPressed() {
		Log.i(Global.TAG, " --------------onBackPressed!");
		super.onBackPressed();
		releaseMediaRecorder();
		releaseCamera();
		// onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.video, menu);
		return true;
	}
	
	public void onImageViewClickEvent(View view) {
		LayoutParams mParams = null ;
		if (!isFriendVideoZoomIn) {
			mParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
			isFriendVideoZoomIn = true ;
		} else {
			mParams = new LayoutParams(DensityUtil.dip2px(getApplicationContext(), 240), DensityUtil.dip2px(getApplicationContext(), 320));
			isFriendVideoZoomIn = false ;
		}
		view.setLayoutParams(mParams);
	}

	public void onSurfaceViewClickEvent(View view) {
//		LayoutParams mParams = null ;
//		if (!isMyselfVideoZoomIn) {
//			mParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
//			 
//			isMyselfVideoZoomIn = true ;
//		} else {
//			mParams = new LayoutParams(DensityUtil.dip2px(getApplicationContext(), 240), DensityUtil.dip2px(getApplicationContext(), 320));
//			isMyselfVideoZoomIn = false ;
//		}
//		view.setLayoutParams(mParams);
//		view.set
	}
	public void onEndClickEvent(View view) {
		Log.i(Global.TAG, "onEndClickEvent>>>from peerId=======" + currentPeerId);
		(new Thread() {
			public void run() {
				String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_END,"End");
				P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);
				releaseMediaRecorder();
				releaseCamera();
				CameraUtils.setUsingCam(false) ;
				audioWrapper.stopRecord();
//				usingCam = false;
			}
		}).start();
	}

	public void onPauseClickEvent(View view) {
		Log.i(Global.TAG, "onPauseClickEvent>>>from peerId=======" + currentPeerId);

		Log.i(Global.TAG, "onPauseClickEvent run=======" + currentPeerId);
		int ret = 1;//P2PClientManager.getP2PClientInstance().pausePlaying();
		Log.i(Global.TAG, "onPauseClickEvent run======ret=" + ret);
		if (ret == 0) {//playing,change to pause
			//@ TODO
			button_pause.setText("Pause");
		} else { //pause,change to playing
			button_pause.setText("Play");
		}
		//return isPlaying 1-playing  ;0 - pause
	
	}
	
	public void onTextClickEvent(View view) {
		
		Log.i(Global.TAG, "onTextClickEvent>>>from peerId=======" + currentPeerId);
		Log.i(Global.TAG, "onTextClickEvent>>>..to peerId======" + remotePeerId);

	 	Intent intent = new Intent();
        intent.putExtra("friendUserId",friendUserId);   
        intent.putExtra("friendNickname",friendNickname);  
        intent.putExtra("friendPeerIds", friendPeerIds);
        /* 指定intent要启动的类 */
        intent.setClass(VideoActivity.this, TextActivity.class);
        /* 启动一个新的Activity */
        startActivity(intent);
        /* 关闭当前的Activity */
        //finish();
		Log.i(Global.TAG, "applyAlertDialogBuilder.show()====");
		Log.i(Global.TAG, "onTextClickEvent====");
		
	}

	public void onApplyClickEvent(View view) {
		
		Log.i(Global.TAG, "onApplyClickEvent>>>from peerId=======" + currentPeerId);
		Log.i(Global.TAG, "onApplyClickEvent>>>..to peerId======" + remotePeerId);
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST, Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY, "Apply");
		P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);

		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(VideoActivity.this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		applyAlertDialog.show();
		Log.i(Global.TAG, "applyAlertDialogBuilder.show()====");
		Log.i(Global.TAG, "onApplyClickEvent====");
		
	}
	
	//
	public void onStartClickEvent(View view) throws IOException {
		Log.i(Global.TAG, " --------------onStartClickEvent!");
		Log.i(Global.TAG, "P2PClientManager startRTPSession>>>..to peerId======" + remotePeerId);
		// start one thread for starting Camera Recording
		startCameraRecording();
		/*(new Thread() {
			public void run() {
				startCameraRecording();
			}
		}).start();*/
	}

	@SuppressLint("NewApi")
	public void startCameraRecording() {
		Log.i(Global.TAG, " --------------startCameraRecording start!");

		if (!CameraUtils.isUsingCam()) {
			CameraUtils.setUsingCam(true) ;
//			usingCam = true;
			SocketUtils.initLocalSocket();

			if (myCamera == null) {
				myCamera = CameraUtils.getNewCameraInstance();
				myCamera.setDisplayOrientation(90);//90
//				myCamera.setDisplayOrientation(getCameraDisplayOrientationDegrees(myCamera));//90

			}

			if (CameraUtils.getVideoConfig() == null) {
				try {
					CameraUtils.getSPSPPS(surfaceView_myself);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

			myCamera.unlock();
			mMediaRecorder = CameraUtils.initMediaRecorder(surfaceView_myself,mMediaRecorder,myCamera);
//			mMediaAudioRecorder = CameraUtils.initAudioRecorder(mMediaAudioRecorder);
			
			try {
				mMediaRecorder.prepare();
//				mMediaAudioRecorder.prepare();
			} catch (IllegalStateException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} 
			mMediaRecorder.start();
			CameraUtils.startVideoRecording();
			
//			mMediaAudioRecorder.start();
			mAudioRecorderWrapper.startRecorder();
			CameraUtils.startAudioRecording();
			
		} else {
			Log.i(Global.TAG, " --------------now playing!");
		}
		Log.i(Global.TAG, " --------------startCameraRecording end!");
	}

    public static int getCameraDisplayOrientationDegrees(Camera camera) {
        android.hardware.Camera.CameraInfo info =
                new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(CameraUtils.getCameraIndex(), info);
        int degrees = 0;
        switch (displayRotationDegree) {
            case Surface.ROTATION_0: degrees = 0; break;
            case Surface.ROTATION_90: degrees = 90; break;
            case Surface.ROTATION_180: degrees = 180; break;
            case Surface.ROTATION_270: degrees = 270; break;
        }
    
        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
		Log.i(Global.TAG, "VideoActivity getCameraDisplayOrientationDegrees>>>..result:======" + result);
        return result ;
    }
    
	private final MyHandler handler = new MyHandler(this);

	public Handler getHandler() {
		return handler;
	}

	private  class MyHandler extends Handler {

		// private static Handler handler = new Handler() {
		private final WeakReference<Activity> myActivity;

		public MyHandler(VideoActivity activity) {
			myActivity = new WeakReference<Activity>(activity);
		}

		@Override
		public void handleMessage(Message msg) {
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..start====== ");
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg.what====== " + msg.what);
			VideoActivity activity = (VideoActivity) myActivity.get();
			if (activity != null) {
				// handleMessage
			}
			if (msg!=null) {
				handlerHandleMsg(msg);
			}
		}

	};

	private void handlerHandleMsg(Message msg) {
//		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss", Locale.US);
		//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId: " + Configer.getValue("peer_id"));
		//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId: " + msg.getData().getString("gpid"));
		if (msg.what == 1) {
			remotePeerId = msg.getData().getString("gpid");
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId:======" + currentPeerId);
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId:======" + remotePeerId);
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg:======" + msg.getData().getString("msg"));
			if (remotePeerId.equals(currentPeerId)) {
				return;
			}
			Map<String, String> msgMap = Global.parseMsg(msg.getData().getString("msg"));
			String msgType = msgMap.get("msg_type");

			if (Global.MSG_TYPE_VIDEO_CHATTING_REQUEST.equals(msgType)) {
				// ///////////////////////////////////////////////////////////////////////
				// MSG_TYPE_VIDEO_CHATTING_REQUEST
				// alertDialog(nickname) ;
				String msgCode = msgMap.get("msg_code");
				//Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgType======" + msgType);
				//Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgCode======" + msgCode);
				if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY)) {
					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
							.setMessage(friendNickname + " want to Video chat with you,would you like to accept it? ")
							.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									// 转跳到另外一个Activity
									//Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									//Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,
											Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT, "Accept");
									P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);
									startCameraRecording();
									//Log.i(Global.TAG, "Accept====");
								}
							}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int which) {
									Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,
											Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT, "Reject");
									P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);
									Log.i(Global.TAG, "Reject====");

									dialog.cancel();// 取消弹出框
								}
							}).create().show();
				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT)) {
					//Log.i(Global.TAG, friendNickname + " has accepted it !");
					//Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
					//Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
					//Log.i(Global.TAG, "Accept====" + "Your friend has accepted it !");
					applyAlertDialog.dismiss();
					//Log.i(Global.TAG, "applyAlertDialog.dismiss()====");
					startCameraRecording();
				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT)) {
					applyAlertDialog.dismiss();
					//Log.i(Global.TAG, "applyAlertDialog.dismiss()====");
					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
							.setMessage(friendNickname + " has rejected it !").setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									// recieve smg
									//Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									//Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									//Log.i(Global.TAG, "Accept====" + "Your friend has rejected it !");
									dialog.cancel();// 取消弹出框
								}
							}).create().show();

				} else if(msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_END)){
					Log.i(Global.TAG, "msgCode  MSG_TYPE_VIDEO_CHATTING_REQUEST_END===="+msgCode);
					releaseMediaRecorder();
					releaseCamera();
					CameraUtils.setUsingCam(false) ;
					audioWrapper.stopRecord();
				}

			}
		} else if (msg.what == 2) { // video packet
			int[] packet=msg.getData().getIntArray("packet");
			Log.i(Global.TAG, "cxd...playing video packet length:" +packet.length);
			Bitmap image = MyBitmap
					.createMyBitmap(packet, msg.getData().getInt("width"), msg.getData().getInt("height"));
			if (image != null) {
				synchronized (image) {
					iv_friend.setImageBitmap(image);
				}
			}
		} else if (5 == msg.what) {
			byte[] packet=msg.getData().getByteArray("packet");
			Log.i(Global.TAG, "AudioHandler handleMessage>>>msg.what=:"+msg.what);
			Log.i(Global.TAG, "playing audio ....");
			
			if(packet!=null)
			{
				Log.i(Global.TAG, "playing audio packet length:"+packet.length);
				audioPlayer.play(packet,packet.length);
				Log.i("AudioHandler", "stop recieving AudioMsg");
				
			}
		} 
	}

	private static void releaseMediaRecorder() {
		Log.i(Global.TAG, "releaseMediaRecorder====" + "starting....");
		if (mMediaRecorder != null) {
			Log.i(Global.TAG, "mMediaRecorder.stop====" + "starting....");
//			mMediaRecorder.stop();
			mMediaRecorder.reset();
			Log.i(Global.TAG, "mMediaRecorder.stop====" + "ended....");
			mMediaRecorder.release();
			mMediaRecorder = null;
			myCamera.lock(); // lock camera for later use
		}
		
		if(audioPlayer!=null)
		{
			Log.i(Global.TAG, "AudioDecoder.stop====" + "starting....");
			audioPlayer.stopPlaying();
		}
		Log.i(Global.TAG, "releaseMediaRecorder====" + "ended....");
	}

	private static void releaseCamera() {
		if (myCamera != null) {
			myCamera.release(); // release the camera for other applications
			myCamera = null;
		}

		if (CameraUtils.isUsingCam()) {
			CameraUtils.setUsingCam(false) ;
//			usingCam = false;
		}

		Log.i(Global.TAG, "myCamera releaseCamera====" + "ended....");
	}

}
