package com.arcsoft.ais.arcvc.activity;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.FragmentTransaction;
import android.util.Log;
import android.view.Menu;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout.LayoutParams;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.fragment.CameraFragment;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.DensityUtil;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.MyBitmap;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.ais.arcvc.utils.SocketUtils;
import com.arcsoft.ais.arcvc.utils.audio.receiver.AudioPlayer;
import com.arcsoft.videochat.mediarecorder.AudioRecorderWrapper;

public class VideoChatActivity extends Activity implements View.OnClickListener{
	static String friendNickname;
	static String currentNickname;
	static String startCameraRecordingFlag;
	static String remotePeerId;
	static String currentPeerId;
	static List<String> peerIdsOfFriends;
	static ImageView iv_friend;
	static SurfaceView surfaceView_myself;
//	static DialogAdapter dialogAdapter;
//	private static Camera myCamera;
//	private static MediaRecorder mMediaRecorder;
	private AudioRecorderWrapper mAudioRecorderWrapper;
//	private static MediaRecorder mMediaAudioRecorder;
//	private static boolean usingCam = false;
	private static Dialog applyAlertDialog;
//	private static AudioWrapper audioWrapper;
	boolean isFriendVideoZoomIn = false ;
	boolean isMyselfVideoZoomIn = false ;
	static long friendUserId;
	static ArrayList<String> friendPeerIds;
	private CameraFragment cameraFragment;
	
	private static AudioPlayer audioPlayer = AudioPlayer.getInstance();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_video_chat);
		getUserInfo();
		initUI();
		
		
		// startRTPSession
		P2PClientManager.getP2PClientInstance().startRTPSession(remotePeerId);
		P2PClientManager.getP2PClientInstance().addHandler(handler);

		
//		audioWrapper = AudioWrapper.getInstance();
		
		//保持屏幕常亮 
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		initMediaConfig();
		Log.i(Global.TAG, "VideoActivity: onCreate============finished!");
	}

	private void getUserInfo() {
		Intent intent = getIntent();
		friendUserId = intent.getLongExtra("friendUserId", 0l);
		friendNickname = intent.getStringExtra("friendNickname");
		startCameraRecordingFlag = intent.getStringExtra("startCameraRecordingFlag");
		currentNickname = Configer.getValue("nickname");
		friendPeerIds = intent.getStringArrayListExtra("friendPeerIds");
		currentPeerId = Configer.getValue("peer_id");
		if (friendPeerIds == null || friendPeerIds.size() < 1) {
			Log.i(Global.TAG, "friendPeerIds.size() == 0>>>...error !!");
		} else {
			peerIdsOfFriends = friendPeerIds;
			remotePeerId = friendPeerIds.get(0);
			Log.i(Global.TAG, "friendPeerIds.get(0)>>>..." + friendPeerIds.get(0));
		}
		Log.i(Global.TAG, "remotePeerId>>>..." + remotePeerId);
	}
	
	private void initUI() {
		setTitle(getTitle()+" with "+friendNickname);
		findViewById(R.id.startBtn).setOnClickListener(this);
		findViewById(R.id.stopBtn).setOnClickListener(this);
		addCameraFragment();
	}
	
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.startBtn:
			startChat();
			break;
		case R.id.stopBtn:
			stopChat();
			break;
		default:
			break;
		}
		
	}
	
	private void addCameraFragment() {
		cameraFragment = new CameraFragment(320, 240, 15);
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(R.id.camera_fragment, cameraFragment);
		ft.commit();
	}
	
	private void startChat() {
		if(cameraFragment != null)
			cameraFragment.startSendData();
	}
	
	private void stopChat() {
		if(cameraFragment == null)
			return;
		cameraFragment.stopSendData();
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.remove(cameraFragment);
		ft.commit();
		cameraFragment = null;
	}
	


	@Override
	protected void onResume() {
		super.onResume();
		Log.i(Global.TAG, "VideoActivity --------------onResume!");
		P2PClientManager.getP2PClientInstance().addHandler(handler);
	};

	@Override
	protected void onPause() {
		super.onPause();
		Log.i(Global.TAG, "VideoActivity --------------onPause!");
		P2PClientManager.getP2PClientInstance().removeHandler(handler);
//		releaseMediaRecorder();
		mAudioRecorderWrapper.releaseRecorder();
	};

	@Override
	protected void onStop() {
		super.onStop();
		Log.i(Global.TAG, "VideoActivity --------------onStop!");
		P2PClientManager.getP2PClientInstance().removeHandler(handler);
//		releaseMediaRecorder();
	};


	private void initMediaConfig() {
		mAudioRecorderWrapper = new AudioRecorderWrapper();
	}

//	@Override
//	public void onClick(View v) {
//
//		// 这里必须要给一个延迟，如果不加延迟则没有效果。我现在还没想明白为什么
//		handler.postDelayed(new Runnable() {
//
//			@Override
//			public void run() {
//				// 将ScrollView滚动到底
//				mScrollView.fullScroll(View.FOCUS_DOWN);
//				// mScrollView.setVerticalScrollBarEnabled(true);
//			}
//		}, 100);
//
//	}

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
//		releaseMediaRecorder();
		super.onDestroy();
	}

	@Override
	public void onBackPressed() {
		super.onBackPressed();
		finish();
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
//				releaseMediaRecorder();
//				releaseCamera();
				CameraUtils.setUsingCam(false) ;
//				audioWrapper.stopRecord();
//				usingCam = false;
			}
		}).start();
	}

	public void onApplyClickEvent(View view) {
		
		Log.i(Global.TAG, "onApplyClickEvent>>>from peerId=======" + currentPeerId);
		Log.i(Global.TAG, "onApplyClickEvent>>>..to peerId======" + remotePeerId);
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST, Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY, "Apply");
		P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);

		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(VideoChatActivity.this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		applyAlertDialog.show();
		Log.i(Global.TAG, "applyAlertDialogBuilder.show()====");
		Log.i(Global.TAG, "onApplyClickEvent====");
		
	}
	

//	@SuppressLint("NewApi")
//	public void startCameraRecording() {
//		Log.i(Global.TAG, " --------------startCameraRecording start!");
//
//		if (!CameraUtils.isUsingCam()) {
//			CameraUtils.setUsingCam(true) ;
////			usingCam = true;
//			SocketUtils.initLocalSocket();
//
//			if (myCamera == null) {
//				myCamera = CameraUtils.getNewCameraInstance();
//				myCamera.setDisplayOrientation(90);//90
////				myCamera.setDisplayOrientation(getCameraDisplayOrientationDegrees(myCamera));//90
//
//			}
//
//			if (CameraUtils.getVideoConfig() == null) {
//				try {
//					CameraUtils.getSPSPPS(surfaceView_myself);
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//			}
//
//			myCamera.unlock();
//			mMediaRecorder = CameraUtils.initMediaRecorder(surfaceView_myself,mMediaRecorder,myCamera);
////			mMediaAudioRecorder = CameraUtils.initAudioRecorder(mMediaAudioRecorder);
//			
//			try {
//				mMediaRecorder.prepare();
////				mMediaAudioRecorder.prepare();
//			} catch (IllegalStateException e) {
//				e.printStackTrace();
//			} catch (IOException e) {
//				e.printStackTrace();
//			} 
//			mMediaRecorder.start();
//			CameraUtils.startVideoRecording();
//			
////			mMediaAudioRecorder.start();
//			mAudioRecorderWrapper.startRecorder();
//			CameraUtils.startAudioRecording();
//			
//		} else {
//			Log.i(Global.TAG, " --------------now playing!");
//		}
//		Log.i(Global.TAG, " --------------startCameraRecording end!");
//	}
    
	private final MyHandler handler = new MyHandler(this);

	public Handler getHandler() {
		return handler;
	}

	private  class MyHandler extends Handler {

		// private static Handler handler = new Handler() {
		private final WeakReference<Activity> myActivity;

		public MyHandler(VideoChatActivity activity) {
			myActivity = new WeakReference<Activity>(activity);
		}

		@Override
		public void handleMessage(Message msg) {
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..start====== ");
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg.what====== " + msg.what);
			VideoChatActivity activity = (VideoChatActivity) myActivity.get();
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
					AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(VideoChatActivity.this);
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
//									startCameraRecording();
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
					applyAlertDialog.dismiss();
//					startCameraRecording();
				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT)) {
					applyAlertDialog.dismiss();
					AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(VideoChatActivity.this);
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
//					releaseMediaRecorder();
					CameraUtils.setUsingCam(false) ;
//					audioWrapper.stopRecord();
				}

			}
		} else if (msg.what == 2) { // video packet
			int[] packet=msg.getData().getIntArray("packet");
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

//	private static void releaseMediaRecorder() {
//		Log.i(Global.TAG, "releaseMediaRecorder====" + "starting....");
//		if (mMediaRecorder != null) {
//			Log.i(Global.TAG, "mMediaRecorder.stop====" + "starting....");
////			mMediaRecorder.stop();
//			mMediaRecorder.reset();
//			Log.i(Global.TAG, "mMediaRecorder.stop====" + "ended....");
//			mMediaRecorder.release();
//			mMediaRecorder = null;
//		}
//		
//		if(audioPlayer!=null)
//		{
//			Log.i(Global.TAG, "AudioDecoder.stop====" + "starting....");
//			audioPlayer.stopPlaying();
//		}
//		Log.i(Global.TAG, "releaseMediaRecorder====" + "ended....");
//	}

}
