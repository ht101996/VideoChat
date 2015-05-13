package com.arcsoft.ais.arcvc.utils;

import com.arcsoft.ais.arcvc.jni.P2PClient;

public class P2PClientManager {
	private static P2PClient p2pClient = null;

	public static P2PClient getP2PClientInstance() {
		if (p2pClient == null) {
			p2pClient = new P2PClient();
			p2pClient.init(SdCardUtils.APP_DATA_PATH);
		}
		//Log.i(Global.TAG, " getP2PClientInstance============" + p2pClient);
		return p2pClient;
	}
	
	
}
