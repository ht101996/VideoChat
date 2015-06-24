package com.arcsoft.ais.arcvc.activity;

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
import android.graphics.Bitmap;
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
import android.view.Menu;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout.LayoutParams;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.fragment.CameraFragment;
import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.DensityUtil;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.MyBitmap;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.ais.arcvc.utils.audio.receiver.AudioPlayer;
import com.es.app.videochat.recorder.ESRecordListener;
import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;
import com.es.app.videochat.recorder.H264Decoder;
import com.es.app.videochat.recorder.MediaDataCenter;

public class VideoChatActivity extends Activity implements View.OnClickListener{
	private final String tag = VideoChatActivity.class.getSimpleName();
	static String friendNickname;
	static String currentNickname;
	static String startCameraRecordingFlag;
	static String remotePeerId;
	static String currentPeerId;
	static List<String> peerIdsOfFriends;
	static ImageView iv_friend;
	static SurfaceView surfaceView_myself;
	private Dialog applyAlertDialog;
	private P2PClient p2pClient;
	boolean isFriendVideoZoomIn = false ;
	boolean isMyselfVideoZoomIn = false ;
	static long friendUserId;
	static ArrayList<String> friendPeerIds;
	private CameraFragment cameraFragment;
	private TextureView mPlaybackView;
	private static AudioPlayer audioPlayer = AudioPlayer.getInstance();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_video_chat);
		getUserInfo();
		initUI();
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	private void initP2PClien() {
		Log.d(tag, "initP2PClien1");
		p2pClient = P2PClientManager.getP2PClientInstance();
		Log.d(tag, "initP2PClien2");
		p2pClient.startRTPSession(remotePeerId);
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
	
	private View startBtn, stopBtn, requestBtn;
	private void initUI() {
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
		cameraFragment = new CameraFragment(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, 15);
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		ft.add(R.id.camera_fragment, cameraFragment);
		ft.commit();
		
	}
	//for test
	private H264Decoder decoder;
	private MediaDataCenter mDataCenter;
	private void startDecode(Surface surface) {
		decoder = new H264Decoder(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, 15);
		decoder.setupDecoder(surface);
		decoder.startDecoder();
	
		mDataCenter = MediaDataCenter.getInstance(mDataDecodeListener, false);
		mDataCenter.start();
		
		cameraFragment.setEncoderListener(encoderListener);
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
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onVideoDataDecoding(byte[] data, long timestampUs) {
			try{ 
				decoder.decode(data, timestampUs);
			}catch(Exception e) {
				e.printStackTrace();
				decoder.resetDecoder();
			}
		}
		
	};
	
	private void startChat() {
		if(cameraFragment != null)
			cameraFragment.startSendData();
		
		startAudioRecord();
//		startDecode(new Surface(mPlaybackView.getSurfaceTexture()));//TODO add test surface 
	}
	private void stopChat() {
		bSendingAudio = false;
		if(cameraFragment == null)
			return;
		cameraFragment.stopSendData();
		
	}
//	private static MediaRecorder mMediaAudioRecorder;
//	private void startMediaRecordAudio() {
//		mMediaAudioRecorder = CameraUtils.initAudioRecorder(mMediaAudioRecorder);
//		try {
//			mMediaAudioRecorder.prepare();
//		} catch (IllegalStateException e) {
//			e.printStackTrace();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//		mMediaAudioRecorder.start();
//		CameraUtils.startAudioRecording();
//	}
	private int bufferSize;
	private int audioSampleRate;
	private MediaCodec encoder;
	private AudioRecord recorder;
	private short audioFormat = AudioFormat.ENCODING_PCM_16BIT;
	private short channelConfig = AudioFormat.CHANNEL_IN_MONO;
	private boolean bSendingAudio = false;
	private boolean setEncoder(int rate)
	{
		audioSampleRate = rate;
		encoder = MediaCodec.createEncoderByType("audio/mp4a-latm");
		MediaFormat format = new MediaFormat();
		format.setString(MediaFormat.KEY_MIME, "audio/mp4a-latm");
		format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
		format.setInteger(MediaFormat.KEY_SAMPLE_RATE, rate);// rate = 44100
		format.setInteger(MediaFormat.KEY_BIT_RATE, rate * 16);// 64kbps?
//		format.setInteger(MediaFormat.KEY_IS_ADTS, 1);//lots driver unsupported, throws IllegalStateException
		format.setInteger(MediaFormat.KEY_AAC_PROFILE,
				MediaCodecInfo.CodecProfileLevel.AACObjectLC);
		encoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
		return true;
	}
    private int findAudioRecord()
    {
        for (int rate : new int[]{44100})
        {
            try
            {
                bufferSize = AudioRecord.getMinBufferSize(rate, channelConfig, audioFormat);

                if (bufferSize != AudioRecord.ERROR_BAD_VALUE)
                {
                    // check if we can instantiate and have a success
                    recorder = new AudioRecord(AudioSource.MIC, rate, channelConfig, audioFormat, bufferSize);

                    if (recorder.getState() == AudioRecord.STATE_INITIALIZED)
                    {
                        Log.v(tag, "===========final rate :"+ rate + "Hz, bits: " + audioFormat + ", channel: " + channelConfig);

                        return rate;
                    }
                }
            }
            catch (Exception e)
            {
                Log.v("error", "" + rate);
            }

        }
        return -1;
    }
    private String filePath = Environment.getExternalStorageDirectory().getPath().concat(File.separator).concat("sending.m4p");
    
	private boolean startAudioRecord() {
		int rate = findAudioRecord();
        if (rate == -1) 
        	return false;
        boolean encoderReady = setEncoder(rate);
	    if (!encoderReady)
	         return false;
	    bSendingAudio = true;
		Thread IOrecorder = new Thread(new Runnable()
        {
        	private void addADTStoPacket(byte[] packet, int packetLen) {
        	    int profile = 2;  //AAC LC
        	                      //39=MediaCodecInfo.CodecProfileLevel.AACObjectELD;
        	    int freqIdx = 11;  //44.1KHz
        	    int chanCfg = 1;  //CPE

        	    // fill in ADTS data
        	    packet[0] = (byte)0xFF;
        	    packet[1] = (byte)0xF1;
        	    packet[2] = (byte)(((profile-1)<<6) + (freqIdx<<2) +(chanCfg>>2));
        	    packet[3] = (byte)(((chanCfg&0x3)<<6) + (packetLen>>11));
        	    packet[4] = (byte)((packetLen&0x7FF) >> 3);
        	    packet[5] = (byte)(((packetLen&7)<<5) + 0x1F);
        	    packet[6] = (byte)0xFC;
        	
        	}
            public void run()
            {
                byte[] buffer1 = new byte[bufferSize];

                ByteBuffer[] inputBuffers;
                ByteBuffer[] outputBuffers;

                ByteBuffer inputBuffer;
                ByteBuffer outputBuffer;

                MediaCodec.BufferInfo bufferInfo;
                int inputBufferIndex;
                int outputBufferIndex;
                FileOutputStream fos = null;
                byte[] outData;

                try
                {
                    encoder.start();
                    recorder.startRecording();
                    bSendingAudio = true;
                    File file = new File(filePath);
                    fos = new FileOutputStream(file);;
                    while (bSendingAudio)
                    {
                        int read = recorder.read(buffer1, 0, bufferSize);
                        if(AudioRecord.ERROR_INVALID_OPERATION == read || AudioRecord.ERROR_BAD_VALUE == read)
                        	break;

                        inputBuffers = encoder.getInputBuffers();
                        outputBuffers = encoder.getOutputBuffers();
                        inputBufferIndex = encoder.dequeueInputBuffer(-1);
                        if (inputBufferIndex >= 0)
                        {
                            inputBuffer = inputBuffers[inputBufferIndex];
                            inputBuffer.clear();

                            inputBuffer.put(buffer1);
                            long pts = getJitterFreePTS(System.nanoTime() / 1000, read / 2);
                            encoder.queueInputBuffer(inputBufferIndex, 0, buffer1.length, pts, 0);
                        }

                        bufferInfo = new MediaCodec.BufferInfo();
                        outputBufferIndex = encoder.dequeueOutputBuffer(bufferInfo, 0);

                        while (outputBufferIndex >= 0)
                        {
//                        	System.out.println("<--cxd, encode audio presentationTimeUs:"+(float)bufferInfo.presentationTimeUs / 1000000+",current time:"+System.nanoTime());
                        	
                            outputBuffer = outputBuffers[outputBufferIndex];
                            outputBuffer.position(bufferInfo.offset);
                            outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                            outData = new byte[bufferInfo.size];
                           
                            if(false) {
                            	int outPacketSize = bufferInfo.size + 7; 
                            	byte[] data = new byte[outPacketSize];  //space for ADTS header included
                                addADTStoPacket(data, outPacketSize);
                                outputBuffer.get(data, 7, bufferInfo.size);
                                outputBuffer.position(bufferInfo.offset);
                                fos.write(data, 0, outPacketSize);
                            }
                            outputBuffer.get(outData);
                            P2PClientManager.getP2PClientInstance().sendAACESData(outData, outData.length, bufferInfo.presentationTimeUs);
                            encoder.releaseOutputBuffer(outputBufferIndex, false);
                            outputBufferIndex = encoder.dequeueOutputBuffer(bufferInfo, 0);
                           
                        }
                        if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) 
                        {
                             outputBuffers = encoder.getOutputBuffers();
                        } 
                        else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) 
                        {
                        }
                        // ----------------------;

                    }
//                    fos.close();
//                    encoder.stop();
//                    recorder.stop();
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                finally{
                	if(fos != null)
						try {
							fos.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
                	encoder.stop();
                    recorder.stop();
                }
            }
        });
		IOrecorder.start();
		return true;
	}
	
	 private long startPTS = 0;
	 private long totalSamplesNum = 0;

    /**
     * Ensures that each audio pts differs by a constant amount from the previous one.
     * @param bufferPts presentation timestamp in us
     * @param bufferSamplesNum the number of samples of the buffer's frame
     * @return
     */
    private long getJitterFreePTS(long bufferPts, long bufferSamplesNum) {
        long correctedPts = 0;
        long bufferDuration = (1000000 * bufferSamplesNum) / (audioSampleRate);
        bufferPts -= bufferDuration; // accounts for the delay of acquiring the audio buffer
        if (totalSamplesNum == 0) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
        }
        correctedPts = startPTS +  (1000000 * totalSamplesNum) / (audioSampleRate);
        if(bufferPts - correctedPts >= 2*bufferDuration) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
            correctedPts = startPTS;
        }
        totalSamplesNum += bufferSamplesNum;
        return correctedPts;
    }

	@Override
	protected void onResume() {
		super.onResume();
		initP2PClien();
	};

	@Override
	protected void onPause() {
		super.onPause();
		bSendingAudio = false;
		p2pClient.uninit();
	};

	@Override
	protected void onStop() {
		super.onStop();
		Log.i(Global.TAG, "VideoActivity --------------onStop!");
//		releaseMediaRecorder();
	};


//	private void initMediaConfig() {
//		mAudioRecorderWrapper = new AudioRecorderWrapper();
//	}

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
//		SocketUtils.releaseLocalSocket();
//		releaseMediaRecorder();
		super.onDestroy();
	}

//	@Override
//	public void onBackPressed() {
////		super.onBackPressed();
//		finish();
//	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		Log.d(tag, "onKeyUp, keyCode="+keyCode+", event.isTracking():"+event.isTracking()+", event.isCanceled():"+event.isCanceled());
		if(keyCode == KeyEvent.KEYCODE_BACK) {
			finish();
			return true;
		}
		else
		return super.onKeyUp(keyCode, event);
	}
	
//	@Override
//	public boolean onCreateOptionsMenu(Menu menu) {
//		// Inflate the menu; this adds items to the action bar if it is present.
//		getMenuInflater().inflate(R.menu.video, menu);
//		return true;
//	}
	
//	public void onImageViewClickEvent(View view) {
//		LayoutParams mParams = null ;
//		if (!isFriendVideoZoomIn) {
//			mParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
//			isFriendVideoZoomIn = true ;
//		} else {
//			mParams = new LayoutParams(DensityUtil.dip2px(getApplicationContext(), 240), DensityUtil.dip2px(getApplicationContext(), 320));
//			isFriendVideoZoomIn = false ;
//		}
//		view.setLayoutParams(mParams);
//	}

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
									P2PClientManager.getP2PClientInstance().acceptVideoChat(remotePeerId);
								}
							}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int which) {
									Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
									Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
									P2PClientManager.getP2PClientInstance().rejectVideoChat(remotePeerId);
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
