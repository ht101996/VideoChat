package com.arcsoft.ais.arcvc.activity;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.es.app.videochat.recorder.AACDecoder;
import com.es.app.videochat.recorder.H264Decoder;
import com.es.app.videochat.recorder.MediaDataCenter;



public class VideoActivity extends Activity implements View.OnClickListener{

	static Button button_pause ;
	static String friendNickname;
	static String currentNickname;
	static String startCameraRecordingFlag;
	static String remotePeerId;
	static String currentPeerId;
	static List<String> peerIdsOfFriends;
	private static Dialog applyAlertDialog;
	private static AlertDialog.Builder alertDialogBuilder;
	static int displayRotationDegree = 0 ;
	static long friendUserId;
	static ArrayList<String> friendPeerIds;
	private TextureView mPlaybackView;
	private MediaDataCenter mMediaDataCenter;
	private P2PClient p2pClient;
//	private static AudioPlayer audioPlayer = AudioPlayer.getInstance();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_video);
		Intent intent = getIntent();
		friendUserId = intent.getLongExtra("friendUserId", 0l);
		friendNickname = intent.getStringExtra("friendNickname");
		startCameraRecordingFlag = intent.getStringExtra("startCameraRecordingFlag");
		currentNickname = Configer.getValue("nickname");
		friendPeerIds = intent.getStringArrayListExtra("friendPeerIds");
		
		displayRotationDegree = getWindowManager().getDefaultDisplay()
	                .getRotation();
		 
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
		initUI();
		initDecoders();
		initP2PClien();
		
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		Log.i(Global.TAG, "VideoActivity: onCreate============finished!");
	}
	
	private void initUI() {
		setTitle(getTitle()+" with "+friendNickname);
		findViewById(R.id.startBtn).setOnClickListener(this);
		findViewById(R.id.stopBtn).setOnClickListener(this);
		findViewById(R.id.requestBtn).setOnClickListener(this);
		mPlaybackView = (TextureView) findViewById(R.id.PlaybackView);
		
		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		alertDialogBuilder = new AlertDialog.Builder(VideoActivity.this);
	}
	
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.startBtn:
//			startChat();
			break;
		case R.id.stopBtn:
//			stopChat();
			break;
		case R.id.requestBtn:
//			requestChat();
			break;
		default:
			break;
		}
		
	}
	private void initP2PClien() {
		p2pClient = P2PClientManager.getP2PClientInstance();
		p2pClient.startRTPSession(remotePeerId);
		p2pClient.setDateReceivedListener(dateReceivedListener);
	}

//	OnTouchListener touchListener = new OnTouchListener() {
//
//		@Override
//		public boolean onTouch(View v, MotionEvent event) {
//			if (event.getAction() == MotionEvent.ACTION_MOVE) {
//
//				// TODO Auto-gener
//				InputMethodManager imm = (InputMethodManager) getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
//				/*
//				 * // 显示或者隐藏输入法 imm.toggleSoftInput(0,
//				 * InputMethodManager.HIDE_NOT_ALWAYS);
//				 */
//				if (imm.isActive()) {
//					imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
//				}
//			}
//
//			return false;
//		}
//
//	};

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
	protected void onResume() {
		super.onResume();
		Log.i(Global.TAG, "VideoActivity --------------onResume!");
	};

	@Override
	protected void onPause() {
		super.onPause();
		Log.i(Global.TAG, "VideoActivity --------------onPause!");
//		handler.removeMessages(what);//TODO remove msg if necessary
		releaseMediaRecorder();
		stopDecode();
	};


	@Override
	public void onBackPressed() {
		Log.i(Global.TAG, " --------------onBackPressed!");
//		super.onBackPressed();
		p2pClient.uninit();
		finish();
		// onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.video, menu);
		return true;
	}

	public void onEndClickEvent(View view) {
		Log.i(Global.TAG, "onEndClickEvent>>>from peerId=======" + currentPeerId);
		(new Thread() {
			public void run() {
				p2pClient.stopVideoChat(remotePeerId);
				releaseMediaRecorder();
				CameraUtils.setUsingCam(false) ;
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
		p2pClient.requestVideoChat(remotePeerId);

		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(VideoActivity.this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		applyAlertDialog.show();
		Log.i(Global.TAG, "applyAlertDialogBuilder.show()====");
		Log.i(Global.TAG, "onApplyClickEvent====");
		
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
    


    private Handler mHandler = new Handler(){
    	public void handleMessage(Message msg) {

    		if (msg.what == 1) {
    			remotePeerId = msg.getData().getString("gpid");
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
    				if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY)) {
    					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
    							.setMessage(friendNickname + " want to Video chat with you,would you like to accept it? ")
    							.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
    								@Override
    								public void onClick(DialogInterface dialog, int which) {
    									p2pClient.acceptVideoChat(remotePeerId);
    									
    									//TODO start local camera
    								}
    							}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
    								public void onClick(DialogInterface dialog, int which) {
    								
    									p2pClient.rejectVideoChat(remotePeerId);

    									dialog.cancel();
    								}
    							}).create().show();
    				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT)) {
    					applyAlertDialog.dismiss();
    					//TODO request is rejected. start local camera
    				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT)) {
    					applyAlertDialog.dismiss();
    					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
    							.setMessage(friendNickname + " has rejected it !").setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
    								@Override
    								public void onClick(DialogInterface dialog, int which) {
    									dialog.cancel();// 取消弹出框
    								}
    							}).create().show();

    				} else if(msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_END)){
    					Log.i(Global.TAG, "msgCode  MSG_TYPE_VIDEO_CHATTING_REQUEST_END===="+msgCode);
    					releaseMediaRecorder();
    					CameraUtils.setUsingCam(false) ;
    				}

    			}
    		}
    	
    	};
    };

	private static void releaseMediaRecorder() {
	}

	//for test
	private H264Decoder h264Decoder;
	private boolean H264DecoderStarted = false;
	
	private AACDecoder aacDecoder;
	private boolean aacDcoderStarted = false;
	
	private void initDecoders() {
		h264Decoder = new H264Decoder(320, 240, 15);
		aacDecoder = new AACDecoder();
		mMediaDataCenter = MediaDataCenter.getInstance(aacDecoder, h264Decoder);
	}
	private void startH264Decoder(Surface surface) {
		if(!H264DecoderStarted) 
			h264Decoder.setupDecoder(surface);
		H264DecoderStarted = true;
	}
	private void stopH264Decoder() {
		if(h264Decoder != null) {
			h264Decoder.stopDecoder();
			h264Decoder.releaseDecoder();
			h264Decoder = null;
		}
		H264DecoderStarted = false;
	}
	private void startAACDecoder() {	
		if(!aacDcoderStarted) {
			aacDecoder.start();
		}
		aacDcoderStarted = true;
	}
	private void stopAACDecoder() {
		if(aacDecoder != null) {
			aacDecoder.stop();
			aacDecoder = null;
		}
		aacDcoderStarted = false;
	}
	private void stopDecode(){
		stopH264Decoder();
		stopAACDecoder();
	}
	private DateReceivedListener dateReceivedListener = new DateReceivedListener() {
		
		@Override
		public void onH264DataReceived(byte[] arg0, int offset, int length, double timestamp) {
			if(!H264DecoderStarted) {
				startH264Decoder(new Surface(mPlaybackView.getSurfaceTexture()));
				
			}
//			h264Decoder.onFrame(arg0, 0, length, timestamp);
			mMediaDataCenter.addVideoFrame(new MediaDataCenter.VideoFrameItem(arg0, 0, length, timestamp));
		}
		@Override
		public void onAACDataReceived(byte[] arg0, int offset, int length, double timestamp) {
			if(!aacDcoderStarted) {
				startAACDecoder();
			}
//			aacDecoder.onFrame(arg0, offset, length, timestamp);
			mMediaDataCenter.addAudioData(new MediaDataCenter.AudioStreamData(arg0, offset, length, timestamp));
		}
		
		@Override
		public void onStringMsgReceived(String remoteGPID, String msg) {
			System.out.println("..cxd, onStringMsgReceived remoteGPID="+remoteGPID+", msg:"+remoteGPID);
			
		}
	};
	
	private void startChat() {
		startH264Decoder(new Surface(mPlaybackView.getSurfaceTexture()));
		startAACDecoder();
	}
	

	

}
