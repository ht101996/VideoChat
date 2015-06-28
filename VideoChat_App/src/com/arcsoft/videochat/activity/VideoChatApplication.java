package com.arcsoft.videochat.activity;

import java.util.ArrayList;

import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.jni.P2PClient.DateReceivedListener;
import com.arcsoft.ais.arcvc.utils.SdCardUtils;
import android.app.Application;
import android.util.Log;

public class VideoChatApplication extends Application {
	private static final String tag = VideoChatApplication.class.getSimpleName();
	private P2PClient p2pClient;
	private ArrayList<DateReceivedListener> dateReceivedListeners = new ArrayList<DateReceivedListener>(); 
	
	@Override
	public void onCreate() {
		super.onCreate();
		initP2PClien();
	}
	
	private void initP2PClien() {
		p2pClient = new P2PClient();
		p2pClient.init(SdCardUtils.APP_DATA_PATH);
		Log.d(tag, "init p2p client ~");
	}
	
	public P2PClient getP2PClient() {
		return p2pClient;
	}

	public void setP2PClient(P2PClient p2pClient) {
		this.p2pClient = p2pClient;
	} 
	
	public void uninitP2PClient() {
		if(p2pClient != null) {
			p2pClient.uninit();
			p2pClient = null;
			Log.d(tag, "uninit p2p client ~");
		}
	}
	
	public void startRTPSession(String remotePeerId) {
		p2pClient.startRTPSession(remotePeerId);
	}
	
	public void registerDateReceivedListener(DateReceivedListener listener) {
		 synchronized (dateReceivedListeners) {
			 dateReceivedListeners.add(listener);
		 }
	}
	
	public void unregisterDateReceivedListener(DateReceivedListener listener) {
		 synchronized (dateReceivedListeners) {
			 dateReceivedListeners.remove(listener);
		 }
	}
}
