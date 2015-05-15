package com.arcsoft.ais.arcvc.jni;

import java.io.File;
import java.io.FileOutputStream;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import com.arcsoft.ais.arcvc.utils.Global;


import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class P2PClient {
	public static final String TAG = "P2PClient";
	static Set<Handler> handlersList = new HashSet<Handler>();
	
	static {
//		System.loadLibrary("ffmpeg");
		System.loadLibrary("P2PClient");
	}

	/**
	 * init P2P interface and register callback from jni
	 * 
	 * @param iniDirPath
	 * @return gpid
	 */
	public native void init(String iniDirPath);
	
	public native void uninit();

	/**
	 * send message from jni
	 * 
	 * @param gpid
	 * @param msg
	 */
	public native void sendMsg(String gpid, String msg);

	/**
	 * pausePlaying
	 * @return isPlaying 1-playing  ;0 - pause
	 */
	public native int pausePlaying();
	
	/**
	 * get local gpid from jni
	 * 
	 * @return gpid
	 */
	public native String getLocalGpid();
	
	public native void startRTPSession(String gpidOfRemotePeer);
	
	public native void send264Packet(String packetType, H264Nal nalu);
	
	public native void sendAACPacket(String packetType,AACNal nalu);

	
	String outputFile = Environment.getExternalStorageDirectory().getAbsolutePath()
			.concat(File.separator).concat("chatdump").concat(File.separator).concat("testouputstream");
	int outputindex = 1;
	private void output(String packetType, int nalutype, byte[] input) {
		FileOutputStream fos;
		try{
			///if(input[4] == 0x65 || input[4] == 0x67 || input[4] == 0x68) {
			fos = new FileOutputStream( new File(outputFile.concat(outputindex+"_"+packetType+"_"+nalutype+".txt")), true);
			fos.write(input);
			fos.flush();
			fos.close();
		//	}
			outputindex ++;
		}catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public  void send264Packet2(String packetType, H264Nal nalu){
		
		output(packetType, nalu.getType(), nalu.getPayload());
		
		send264Packet(packetType, nalu);
	}
	
	/**
	 * invoked by jni, when received a msg, show on the UI
	 * 
	 * @param gpid
	 * @param msg
	 */
	public static void onMsgReceive(String gpid, String msg) {
		//Log.i(Global.TAG, "onMsgReceive gpid=" + gpid);
		//Log.i(Global.TAG, "onMsgReceive msg=" + msg);
		Message m = new Message();
		Map<String, String> msgMap = Global.parseMsg(msg);
		String msgType = msgMap.get("msg_type");
		if (Global.MSG_TYPE_ONLINE_STATUS_DETECT.equals(msgType)) {
			m.what = 3;
		} else {
			m.what = 1;
		}
		Bundle bundle = new Bundle();
		bundle.putString("msg", msg);
		bundle.putString("gpid", gpid);
		Log.i(TAG, "onMsgReceive m.what,GPID,MSG=" + m.what+ ","+ gpid+ ","+msg);
		//Log.i(Global.TAG, "onMsgReceive handlersList.size()=" + handlersList.size());
		m.setData(bundle);
		for (Handler handler : handlersList) {
			handler.sendMessage(m);//Pushes a message
			Log.i(TAG, "handler=" + handler.getClass());
		}
	}
	
	public static void onVideoReceive(int[] value, int width, int height) {
		Message m = new Message();
		m.what = 2;
		Bundle bundle = new Bundle();
		bundle.putIntArray("packet", value);
		bundle.putInt("width", width);
		bundle.putInt("height", height);
		m.setData(bundle);

		for (Handler handler : handlersList) {
			handler.sendMessage(m);//Pushes a message
			//Log.i(Global.TAG, "handler=" + handler.getClass());
		}
	}

	public static void onAudioReceive(byte[] value) {
		Message m = new Message();
		m.what = 5;
		Bundle bundle = new Bundle();
		bundle.putByteArray("packet", value);
		m.setData(bundle);

		for (Handler handler : handlersList) {
			handler.sendMessage(m);//Pushes a message
			//Log.i(Global.TAG, "handler=" + handler.getClass());
		}
	}
	public void addHandler(Handler handler){
		Log.i(TAG, "addHandler=");
		handlersList.add(handler);
		for (Handler handler1 : handlersList) {
			Log.i(TAG, "handler1=" + handler1.getClass());
		}
	}

	public void removeHandler(Handler handler){
		Log.i(TAG, "removeHandler=");
		handlersList.remove(handler);
		for (Handler handler1 : handlersList) {
			Log.i(TAG, "handler1=" + handler1.getClass());
		}
	}
}
