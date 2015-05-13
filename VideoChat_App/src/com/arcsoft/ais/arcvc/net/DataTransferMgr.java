package com.arcsoft.ais.arcvc.net;

import com.arcsoft.ais.arcvc.jni.P2PClient;
import com.arcsoft.ais.arcvc.utils.SdCardUtils;

public class DataTransferMgr {
	private static P2PClient p2pClient = null;
	private boolean initedSession = false;

	public DataTransferMgr() {
		p2pClient = new P2PClient();
		p2pClient.init(SdCardUtils.APP_DATA_PATH);
	}
	
	public void initSession(String remotePeerID) {
		p2pClient.startRTPSession(remotePeerID);
		initedSession = false;
	}
	
	//public void 
}
