package com.arcsoft.videochat.fragment;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;
import com.arcsoft.ais.arcvc.utils.CameraUtils;
import com.arcsoft.videochat.codec.AACDecoder;
import com.arcsoft.videochat.codec.H264Decoder;
import com.es.app.videochat.recorder.MediaDataCenter;

import android.app.Fragment;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.TextureView.SurfaceTextureListener;

public class RemoteDisplayFragment extends Fragment implements DateReceivedListener{
	private static final String tag = RemoteDisplayFragment.class.getSimpleName();
	private MediaDataCenter mMediaDataCenter;
	private H264Decoder h264Decoder;
	private boolean H264DecoderStarted = false;
	
	private AACDecoder aacDecoder;
	private boolean aacDcoderStarted = false;
	
	private TextureView mPlaybackView;
	private P2PClient p2pClient;
	public RemoteDisplayFragment(P2PClient p2pClient) {
		this.p2pClient = p2pClient;
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		initMediaDataCenter();
	}
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
		p2pClient.setDateReceivedListener(this);
	}
	
	@Override
	public void onPause() {
		super.onPause();
		stopDecoder();
		
	}
	
	@Override
	public void onResume() {
		super.onResume();
	}
	
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.remote_display_fragment, container, false);
		mPlaybackView = (TextureView) view.findViewById(R.id.PlaybackView);
		mPlaybackView.setSurfaceTextureListener(new SurfaceTextureListener() {
			
			@Override
			public void onSurfaceTextureUpdated(SurfaceTexture surface) {
			}
			
			@Override
			public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,
					int height) {
			}
			
			@Override
			public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
				return false;
			}
			
			@Override
			public void onSurfaceTextureAvailable(SurfaceTexture surface, int width,
					int height) {
				Log.d(tag, "onSurfaceTextureAvailable");
				initDecoders();
				startDecoder();
			}
		});
		return view;
	}
	
	private void initMediaDataCenter() {
		mMediaDataCenter = MediaDataCenter.getInstance(new MediaDataCenter.DataDecodeListener() {
			
			@Override
			public void onVideoDataDecoding(byte[] data, long timestampUs) {
				try{
					h264Decoder.decode(data, timestampUs);
				}catch(Exception e) {
					e.printStackTrace();
//					h264Decoder.releaseDecoder();
				}
			}
			
			@Override
			public void onAudioDataDecoding(byte[] data, long timestampUs) {
				try{
					aacDecoder.decode(data, timestampUs);
				}catch(Exception e) {
					e.printStackTrace();
					aacDecoder.reset();
				}
				
			}
		});
	}
	
	private void initDecoders() {
		h264Decoder = new H264Decoder();
		h264Decoder.setupDecoder(CameraUtils.previewSize_width, CameraUtils.previewSize_Height, new Surface(mPlaybackView.getSurfaceTexture()));
		aacDecoder = new AACDecoder();
	}
	private void startH264Decoder() {
		if(!H264DecoderStarted) {
			h264Decoder.startDecoder();
		}
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
	
	private void startDecoder() {
		startH264Decoder();
		startAACDecoder();
		mMediaDataCenter.start();
	}
	
	private void stopDecoder(){
		mMediaDataCenter.stop();
		stopH264Decoder();
		stopAACDecoder();
	}
		
	@Override
	public void onH264DataReceived(byte[] arg0, int offset, int length, double timestamp) {
//			if(!H264DecoderStarted) {
//				startH264Decoder(new Surface(mPlaybackView.getSurfaceTexture()));
//				
//			}
//			h264Decoder.onFrame(arg0, 0, length, timestamp);
		if(H264DecoderStarted)
			mMediaDataCenter.addVideoFrame(new MediaDataCenter.VideoFrameItem(arg0, 0, length, timestamp));
	}
	
	@Override
	public void onAACDataReceived(byte[] arg0, int offset, int length, double timestamp) {
		if(aacDcoderStarted) 
			mMediaDataCenter.addAudioData(new MediaDataCenter.AudioStreamData(arg0, offset, length, timestamp));
	}
	
	@Override
	public void onStringMsgReceived(String remoteGPID, String msg) {
		System.out.println("..cxd, onStringMsgReceived remoteGPID="+remoteGPID+", msg:"+remoteGPID);
		
	}

}
