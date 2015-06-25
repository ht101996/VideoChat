package com.arcsoft.ais.arcvc.activity;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.Surface;
import android.view.TextureView;
import android.view.TextureView.SurfaceTextureListener;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.fragment.RemoteDisplayFragment;
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
	private final static String tag = VideoActivity.class.getSimpleName();
	
	private long friendUserId;
	private Button button_pause ;
	private String friendNickname;
	private String currentNickname;
	private String startCameraRecordingFlag;
	private String remotePeerId;
	private String currentPeerId;
	private ArrayList<String> friendPeerIds;
	private List<String> peerIdsOfFriends;
	
	private Dialog applyAlertDialog;
	private AlertDialog.Builder alertDialogBuilder;
	private RemoteDisplayFragment remoteFragment;
//	private TextureView mPlaybackView;
//	private MediaDataCenter mMediaDataCenter;
	private P2PClient p2pClient;
	
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
		
		 
		Log.i(tag, "friendUserId= " + friendUserId);
		Log.i(tag, "friendNickname= " + friendNickname);
		for (String friendPeerId : friendPeerIds) {
			Log.i(tag, "friendPeerId= " + friendPeerId);
		}
		currentPeerId = Configer.getValue("peer_id");
		if (friendPeerIds == null || friendPeerIds.size() < 1) {
			Log.i(tag, "friendPeerIds.size() == 0>>>...error !!");
		} else {
			peerIdsOfFriends = friendPeerIds;
			remotePeerId = friendPeerIds.get(0);
			Log.i(tag, "friendPeerIds.get(0)>>>..." + friendPeerIds.get(0));
		}
		initUI();
//		initDecoders();
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		Log.d(tag, "VideoActivity: oncreate============finished!");
	}
	
	private void initUI() {
		setTitle(getTitle()+" with "+friendNickname);
		findViewById(R.id.startBtn).setOnClickListener(this);
		findViewById(R.id.stopBtn).setOnClickListener(this);
		findViewById(R.id.requestBtn).setOnClickListener(this);
		
		remoteFragment = new RemoteDisplayFragment();
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(R.id.remote_display_fragment, remoteFragment);
		ft.commit();
//		mPlaybackView = (TextureView) findViewById(R.id.PlaybackView);
//		mPlaybackView.setSurfaceTextureListener(new SurfaceTextureListener() {
//			
//			@Override
//			public void onSurfaceTextureUpdated(SurfaceTexture surface) {
//			}
//			
//			@Override
//			public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,
//					int height) {
//			}
//			
//			@Override
//			public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
//				return false;
//			}
//			
//			@Override
//			public void onSurfaceTextureAvailable(SurfaceTexture surface, int width,
//					int height) {
//				Log.d(tag, "onSurfaceTextureAvailable");
//				startH264Decoder();
//			}
//		});
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
			requestChat();
			break;
		default:
			break;
		}
		
	}
	private void initP2PClien(DateReceivedListener dateReceivedListener) {
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


	@Override
	protected void onResume() {
		super.onResume();
		initP2PClien(remoteFragment);
//		mMediaDataCenter.start();
		Log.d(tag, "VideoActivity: onResume============finished!");
	};

	@Override
	protected void onPause() {
		super.onPause();
//		stopDecode();
		p2pClient.uninit();
//		mMediaDataCenter.stop();
	};

	@Override
	public void onBackPressed() {
		Log.i(tag, " --------------onBackPressed!");
//		super.onBackPressed();
		p2pClient.uninit();
		p2pClient = null;
		finish();
		// onDestroy();
	}


	public void onEndClickEvent(View view) {
		Log.i(tag, "onEndClickEvent>>>from peerId=======" + currentPeerId);
		(new Thread() {
			public void run() {
				p2pClient.stopVideoChat(remotePeerId);
			}
		}).start();
	}

	public void onPauseClickEvent(View view) {
		Log.i(tag, "onPauseClickEvent>>>from peerId=======" + currentPeerId);

		Log.i(tag, "onPauseClickEvent run=======" + currentPeerId);
		int ret = 1;//P2PClientManager.getP2PClientInstance().pausePlaying();
		Log.i(tag, "onPauseClickEvent run======ret=" + ret);
		if (ret == 0) {//playing,change to pause
			//@ TODO
			button_pause.setText("Pause");
		} else { //pause,change to playing
			button_pause.setText("Play");
		}
		//return isPlaying 1-playing  ;0 - pause
	
	}
	


	private void requestChat() {
		
		p2pClient.requestVideoChat(remotePeerId);

		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(VideoActivity.this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		applyAlertDialog.show();
		
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
    					Log.i(tag, "msgCode  MSG_TYPE_VIDEO_CHATTING_REQUEST_END===="+msgCode);
    				}

    			}
    		}
    	
    	};
    };


	//for test
//	private H264Decoder h264Decoder;
//	private boolean H264DecoderStarted = false;
//	
//	private AACDecoder aacDecoder;
//	private boolean aacDcoderStarted = false;
//	
//	private void initDecoders() {
//		h264Decoder = new H264Decoder(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, 15);
//		aacDecoder = new AACDecoder();
//		mMediaDataCenter = MediaDataCenter.getInstance(new MediaDataCenter.DataDecodeListener() {
//			
//			@Override
//			public void onVideoDataDecoding(byte[] data, long timestampUs) {
//				try{
//					h264Decoder.decode(data, timestampUs);
//				}catch(Exception e) {
//					e.printStackTrace();
//					h264Decoder.releaseDecoder();
//				}
//			}
//			
//			@Override
//			public void onAudioDataDecoding(byte[] data, long timestampUs) {
//				try{
//					aacDecoder.decode(data, timestampUs);
//				}catch(Exception e) {
//					e.printStackTrace();
//					aacDecoder.reset();
//				}
//				
//			}
//		});
//	}
//	private void startH264Decoder() {
//		if(!H264DecoderStarted) {
//			h264Decoder.setupDecoder(new Surface(mPlaybackView.getSurfaceTexture()));
//			h264Decoder.startDecoder();
//		}
//		H264DecoderStarted = true;
//	}
//	private void stopH264Decoder() {
//		if(h264Decoder != null) {
//			h264Decoder.stopDecoder();
//			h264Decoder.releaseDecoder();
//			h264Decoder = null;
//		}
//		H264DecoderStarted = false;
//	}
//	private void startAACDecoder() {	
//		if(!aacDcoderStarted) {
//			aacDecoder.start();
//		}
//		aacDcoderStarted = true;
//	}
//	private void stopAACDecoder() {
//		if(aacDecoder != null) {
//			aacDecoder.stop();
//			aacDecoder = null;
//		}
//		aacDcoderStarted = false;
//	}
//	private void stopDecode(){
//		stopH264Decoder();
//		stopAACDecoder();
//	}
//	private DateReceivedListener dateReceivedListener = new DateReceivedListener() {
//		
//		@Override
//		public void onH264DataReceived(byte[] arg0, int offset, int length, double timestamp) {
//			mMediaDataCenter.addVideoFrame(new MediaDataCenter.VideoFrameItem(arg0, 0, length, timestamp));
//		}
//		@Override
//		public void onAACDataReceived(byte[] arg0, int offset, int length, double timestamp) {
//			if(!aacDcoderStarted) {
//				startAACDecoder();
//			}
//			mMediaDataCenter.addAudioData(new MediaDataCenter.AudioStreamData(arg0, offset, length, timestamp));
//		}
//		
//		@Override
//		public void onStringMsgReceived(String remoteGPID, String msg) {
//			System.out.println("..cxd, onStringMsgReceived remoteGPID="+remoteGPID+", msg:"+remoteGPID);
//			
//		}
//	};

}
