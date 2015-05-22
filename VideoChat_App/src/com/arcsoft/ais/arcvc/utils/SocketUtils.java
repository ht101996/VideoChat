package com.arcsoft.ais.arcvc.utils;

import java.io.IOException;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

public class SocketUtils {
	private static LocalSocket videoReceiverLocalSocket;
	private static LocalSocket videoSenderLocalSocket;
	private static LocalServerSocket videoLocalServerSocket;
	//audio
	private static LocalSocket audioReceiverLocalSocket;
	private static LocalSocket audioSenderLocalSocket;
	private static LocalServerSocket audioLocalServerSocket;
	
	public static void initLocalSocket() {
		Log.i(Global.TAG, " --------------initLocalSocket start!");
		try {
			if (videoReceiverLocalSocket == null) {
				
				videoLocalServerSocket = new LocalServerSocket("VideoCamera");
				
				videoReceiverLocalSocket = new LocalSocket();
				videoReceiverLocalSocket.connect(new LocalSocketAddress("VideoCamera"));
				videoReceiverLocalSocket.setReceiveBufferSize(20480);// 500000
				videoReceiverLocalSocket.setSendBufferSize(20480);//

				videoSenderLocalSocket = videoLocalServerSocket.accept();
				videoSenderLocalSocket.setReceiveBufferSize(20480);
				videoSenderLocalSocket.setSendBufferSize(20480);
			}
				//audio 
				if (audioLocalServerSocket == null) {
				audioLocalServerSocket = new LocalServerSocket("AudioCamera");
				
				audioReceiverLocalSocket = new LocalSocket();
				audioReceiverLocalSocket.connect(new LocalSocketAddress("AudioCamera"));
				audioReceiverLocalSocket.setReceiveBufferSize(20480);
				audioReceiverLocalSocket.setSendBufferSize(20480);//

				audioSenderLocalSocket = audioLocalServerSocket.accept();
				audioSenderLocalSocket.setReceiveBufferSize(20480);
				audioSenderLocalSocket.setSendBufferSize(20480);
			}
			
			Log.i(Global.TAG, " --------------initLocalSocket ended!receiver=="+videoReceiverLocalSocket);
			Log.i(Global.TAG, " --------------initLocalSocket ended!sender=="+videoSenderLocalSocket);
		} catch (IOException e) {
			e.printStackTrace();
			releaseLocalSocket();
		}
		Log.i(Global.TAG, " --------------initLocalSocket end!");
	}

	public static void releaseLocalSocket() {
		Log.i(Global.TAG, " --------------releaseLocalSocket start!");
		try {
			if (videoReceiverLocalSocket != null) {
				videoReceiverLocalSocket.close();
				videoReceiverLocalSocket = null;
			}
			if (videoSenderLocalSocket != null) {
				videoSenderLocalSocket.close();
				videoSenderLocalSocket = null;
			}
			if (videoLocalServerSocket != null) {
				videoLocalServerSocket.close();
				videoLocalServerSocket = null;
			}
			
			if (audioReceiverLocalSocket != null) {
				audioReceiverLocalSocket.close();
				audioReceiverLocalSocket = null;
			}
			if (audioSenderLocalSocket != null) {
				audioSenderLocalSocket.close();
				audioSenderLocalSocket = null;
			}
			if (audioLocalServerSocket != null) {
				audioLocalServerSocket.close();
				audioLocalServerSocket = null;
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		Log.i(Global.TAG, " --------------releaseLocalSocket end!");
	}

	public static LocalSocket getVideoReceiverSocket() {
		if (videoReceiverLocalSocket == null) {
			initLocalSocket();
		}
		return videoReceiverLocalSocket;
	}

	public static LocalSocket getVideoSenderSocket() {
		if (videoSenderLocalSocket == null) {
			initLocalSocket();
		}
		return videoSenderLocalSocket;
	}

	public static LocalSocket getAudioReceiverSocket() {
		if (audioReceiverLocalSocket == null) {
			initLocalSocket();
		}
		return audioReceiverLocalSocket;
	}

	public static LocalSocket getAudioSenderSocket() {
		if (audioSenderLocalSocket == null) {
			initLocalSocket();
		}
		return audioSenderLocalSocket;
	}
}
