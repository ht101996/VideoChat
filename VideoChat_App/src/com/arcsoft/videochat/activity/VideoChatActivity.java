package com.arcsoft.videochat.activity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaRecorder.AudioSource;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.videochat.codec.H264Decoder;
import com.arcsoft.videochat.fragment.AudioRecorderFragment;
import com.arcsoft.videochat.fragment.CameraFragment;
import com.arcsoft.videochat.fragment.RemoteDisplayFragment;
import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;
import com.es.app.videochat.recorder.MediaDataCenter;

public class VideoChatActivity extends Activity implements View.OnClickListener{
	private final String tag = VideoChatActivity.class.getSimpleName();
	private final String audioRecorderFragmentTag = "audioRecorderFragmentTag";
	private String friendNickname;
	private String currentNickname;
	private String startCameraRecordingFlag;
	private String remotePeerId;
	private String currentPeerId;
	private ArrayList<String> friendPeerIds;
	private List<String> peerIdsOfFriends;
	private P2PClient p2pClient;
	
	private long friendUserId;
	private boolean isFinished = false;
	
	/****** views *******/
	private Dialog applyAlertDialog;
	private View startBtn, stopBtn, requestBtn;
	private CameraFragment cameraFragment;
	private AudioRecorderFragment audioRecorderFragment;
	private RemoteDisplayFragment remoteFragment;
	private TextureView mPlaybackView;
	private MediaDataCenter mDataCenter;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_video_chat);
		//do not change function's order
		getUserInfo();
		initConnection();
		initUI();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
//		initP2PClien(remoteFragment);
	};

	@Override
	protected void onPause() {
		super.onPause();
		stopChat();
		if(isFinished)
			stopConnection();
//		uninitP2PClient();
	};
	
	private void getUserInfo() {
		Intent intent = getIntent();
		friendUserId = intent.getLongExtra("friendUserId", 0l);
		friendNickname = intent.getStringExtra("friendNickname");
		startCameraRecordingFlag = intent.getStringExtra("startCameraRecordingFlag");
		currentNickname = Configer.getValue("nickname");
		friendPeerIds = intent.getStringArrayListExtra("friendPeerIds");
		currentPeerId = Configer.getValue("peer_id");
		if (friendPeerIds == null || friendPeerIds.size() < 1) {
			Log.i(tag, "friendPeerIds.size() == 0>>>...error !!");
		} else {
			peerIdsOfFriends = friendPeerIds;
			remotePeerId = friendPeerIds.get(0);
			Log.i(tag, "friendPeerIds.get(0)>>>..." + friendPeerIds.get(0));
		}
	}

	
	private void initUI() {
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setTitle(getTitle()+" with "+friendNickname);
		AlertDialog.Builder applyAlertDialogBuilder = new AlertDialog.Builder(this);
		applyAlertDialog = applyAlertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
				.setMessage("waiting for your friend to accept it ...").create();
		startBtn =findViewById(R.id.startBtn);
		startBtn.setOnClickListener(this);
		
		stopBtn = findViewById(R.id.stopBtn);
		stopBtn.setOnClickListener(this);
		stopBtn.setEnabled(false);
		
		requestBtn = findViewById(R.id.requestBtn);
		requestBtn.setOnClickListener(this);
		mPlaybackView = (TextureView) findViewById(R.id.PlaybackView);
		addCameraFragment();
		addRemoteDisplayFragment();
		addAudioRecorderFragment();
	}
	
	private void initConnection() {
		p2pClient = ((VideoChatApplication)getApplication()).getP2PClient();
		p2pClient.startRTPSession(remotePeerId);
	}
	
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.startBtn:
			startChat();
			v.setEnabled(false);
			stopBtn.setEnabled(true);
			break; 
		case R.id.stopBtn:
			stopChat();
			v.setEnabled(false);
			startBtn.setEnabled(true);
			break;
		case R.id.requestBtn:
//			requestChat();
			break;
		default:
			break;
		}
		
	}
	
	private void setButtonState(int processState) {
		//TODO
	}
	private void requestChat() {
		if(applyAlertDialog != null)
			applyAlertDialog.show();
		p2pClient.requestVideoChat(remotePeerId);
	}
	
	private void addCameraFragment() {
		cameraFragment = new CameraFragment(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, 15, p2pClient);
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(R.id.camera_fragment, cameraFragment);
		ft.commit();
	}
	
	private void addRemoteDisplayFragment() {
		remoteFragment = new RemoteDisplayFragment(p2pClient);
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(R.id.remote_display_fragment, remoteFragment);
		ft.commit();
	}
	
	private void addAudioRecorderFragment() {
		audioRecorderFragment = new AudioRecorderFragment(p2pClient);
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(audioRecorderFragment, audioRecorderFragmentTag);
		ft.commit();
	}

	private void startChat() {
		if(cameraFragment != null)
			cameraFragment.startSendData();
		if(audioRecorderFragment != null)
			audioRecorderFragment.startAudioRecorder();
//		startDecode(new Surface(mPlaybackView.getSurfaceTexture()));//TODO add test surface 
	}
	
	private void stopChat() {
		if(cameraFragment != null)
			cameraFragment.stopSendData();
		if(audioRecorderFragment != null)
			audioRecorderFragment.stopAudioRecorder();
	}
	
	private void stopConnection() {
		((VideoChatApplication)getApplication()).uninitP2PClient();
	}
	

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		Log.d(tag, "onKeyUp, keyCode="+keyCode+", event.isTracking():"+event.isTracking()+", event.isCanceled():"+event.isCanceled());
		if(keyCode == KeyEvent.KEYCODE_BACK) {
			isFinished = true;
			finish();
			return true;
		}
		else
		return super.onKeyUp(keyCode, event);
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		if(!isFinished){
			Log.i(tag, "Stop Connection onSaveInstanceState!");
			stopConnection();
		}
	}
	
	//for test
	private H264Decoder decoder;
	
	private void startDecode(Surface surface) {
		decoder = new H264Decoder();
		decoder.setupDecoder(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, surface);
		decoder.startDecoder();
	
		mDataCenter = MediaDataCenter.getInstance(mDataDecodeListener, false);
		mDataCenter.start();
		
//		cameraFragment.setEncoderListener(encoderListener);//TODO 
	}
	private OnEncoderListener  encoderListener = new OnEncoderListener() {

		@Override
		public void onEncodeFinished(byte[] data, long timestamp) {
			mDataCenter.addVideoFrame(new MediaDataCenter.VideoFrameItem(data, 0, data.length, timestamp));
			
		}
		
	};
	private MediaDataCenter.DataDecodeListener mDataDecodeListener  = new MediaDataCenter.DataDecodeListener() {

		@Override
		public void onAudioDataDecoding(byte[] data, long timestampUs) {
			
		}

		@Override
		public void onVideoDataDecoding(byte[] data, long timestampUs) {
			try{ 
				decoder.decode(data, timestampUs);
			}catch(Exception e) {
				e.printStackTrace();
//				decoder.resetDecoder();
			}
		}
		
	};
	

	private void handlerHandleMsg(Message msg) {
//		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss", Locale.US);
		//Log.i(tag, "VideoActivity Handler handleMessage>>>..current peerId: " + Configer.getValue("peer_id"));
		//Log.i(tag, "VideoActivity Handler handleMessage>>>..from peerId: " + msg.getData().getString("gpid"));
		if (msg.what == 1) {
			remotePeerId = msg.getData().getString("gpid");
			//Log.i(tag, "VideoActivity Handler handleMessage>>>..current peerId:======" + currentPeerId);
			//Log.i(tag, "VideoActivity Handler handleMessage>>>..from peerId:======" + remotePeerId);
			//Log.i(tag, "VideoActivity Handler handleMessage>>>..msg:======" + msg.getData().getString("msg"));
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
				//Log.i(tag, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgType======" + msgType);
				//Log.i(tag, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgCode======" + msgCode);
				if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY)) {
					AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(VideoChatActivity.this);
					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
							.setMessage(friendNickname + " want to Video chat with you,would you like to accept it? ")
							.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									p2pClient.acceptVideoChat(remotePeerId);
								}
							}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int which) {
									Log.i(tag, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									Log.i(tag, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									p2pClient.rejectVideoChat(remotePeerId);
									Log.i(tag, "Reject====");

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
									//Log.i(tag, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									//Log.i(tag, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									//Log.i(tag, "Accept====" + "Your friend has rejected it !");
									dialog.cancel();// 取消弹出框
								}
							}).create().show();

				} else if(msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_END)){
					Log.i(tag, "msgCode  MSG_TYPE_VIDEO_CHATTING_REQUEST_END===="+msgCode);
//					releaseMediaRecorder();
//					audioWrapper.stopRecord();
				}

			}
		} 
//		else if (msg.what == 2) { // video packet
//			int[] packet=msg.getData().getIntArray("packet");
//			Bitmap image = MyBitmap
//					.createMyBitmap(packet, msg.getData().getInt("width"), msg.getData().getInt("height"));
//			if (image != null) {
//				synchronized (image) {
//					iv_friend.setImageBitmap(image);
//				}
//			}
//		} 
//		else if (5 == msg.what) {
//			byte[] packet=msg.getData().getByteArray("packet");
//			Log.i(tag, "AudioHandler handleMessage>>>msg.what=:"+msg.what);
//			Log.i(tag, "playing audio ....");
//			
//			if(packet!=null)
//			{
//				Log.i(tag, "playing audio packet length:"+packet.length);
//				audioPlayer.play(packet,packet.length);
//				Log.i("AudioHandler", "stop recieving AudioMsg");
//				
//			}
//		} 
	}

//	private static void releaseMediaRecorder() {
//		Log.i(tag, "releaseMediaRecorder====" + "starting....");
//		if (mMediaRecorder != null) {
//			Log.i(tag, "mMediaRecorder.stop====" + "starting....");
////			mMediaRecorder.stop();
//			mMediaRecorder.reset();
//			Log.i(tag, "mMediaRecorder.stop====" + "ended....");
//			mMediaRecorder.release();
//			mMediaRecorder = null;
//		}
//		
//		if(audioPlayer!=null)
//		{
//			Log.i(tag, "AudioDecoder.stop====" + "starting....");
//			audioPlayer.stopPlaying();
//		}
//		Log.i(tag, "releaseMediaRecorder====" + "ended....");
//	}

}
