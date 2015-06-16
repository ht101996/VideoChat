package com.arcsoft.ais.arcvc.jni;

import java.io.File;
import java.io.FileOutputStream;
import java.util.HashSet;
import java.util.Map;
import com.arcsoft.ais.arcvc.utils.Global;


import android.os.Environment;

public class P2PClient {
	public static final String TAG = "P2PClient";
	
	static {
//		System.loadLibrary("ffmpeg");
		System.loadLibrary("P2PClient");
	}
	private DateReceivedListener dataListener;
	private OnlineStateDetectionListener onlineStateDetectionListener;
	
	public interface DateReceivedListener{
		public void onH264DataReceived(byte[] data, int offset, int length, double timestamp);
		public void onAACDataReceived(byte[] data, int offset, int length, double timestamp);
		public void onStringMsgReceived(String remoteGPID, String msg);
	}
	public interface OnlineStateDetectionListener{
		public void onAckDetected(String arg1, String arg2);
		public void onheartbeatDetected(String arg1, String arg2);
	}
	
	public void requestVideoChat(String destGPID) {
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST, Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY, "Apply");
		sendMsg(destGPID, message);
	}
	
	public void rejectVideoChat(String destGPID) {
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,
				Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT, "Reject");
		sendMsg(destGPID, message);
	}
	
	public void acceptVideoChat(String destGPID) {
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST, Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT, "Accept");
		sendMsg(destGPID, message);
	}
	
	public void stopVideoChat(String destGPID) {
		String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_END,"End");
		sendMsg(destGPID, message);
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
	
	public native void send264Packet(String packetType, H264Nal nalu, long timestamp);
	
	public native void sendAACPacket(String packetType,AACNal nalu);
	public native void sendAACESData(byte[] data, int length, long timestamp);

	
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
	
	private void output(byte[] input) {
		FileOutputStream fos;
		try{
			///if(input[4] == 0x65 || input[4] == 0x67 || input[4] == 0x68) {
			fos = new FileOutputStream( new File(outputFile.concat(outputindex + ".txt")), true);
			fos.write(input);
			fos.flush();
			fos.close();
		//	}
			outputindex ++;
		}catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 
	 * @param packetType
	 * @param nalu
	 * @param timestampUs
	 */
	public  void send264Packet2(String packetType, H264Nal nalu, long timestampUs){
		
//		output(packetType, nalu.getType(), nalu.getPayload());
		send264Packet(packetType, nalu, timestampUs);
	}
	
	
	
	public void setDateReceivedListener(DateReceivedListener listener) {
		this.dataListener = listener;
	}
	public void setOnlineStateDetectionListener(OnlineStateDetectionListener listener) {
		this.onlineStateDetectionListener = listener;
	}
	
	//invoked by jni when received H264 data
	public void receiveH264Data(byte[] data, int offset, int length, double timestamp) {
//		output(data);
		if(dataListener != null)
			dataListener.onH264DataReceived(data, offset, length, timestamp);
	}

	public void receiveAACData(byte[] data, int offset, int length, double timestamp) {
//		output(data);
		if(dataListener != null)
			dataListener.onAACDataReceived(data, offset, length, timestamp);
	}
	
	// invoked by jni, when received a string message
	public void receiveStringMsg(String remoteGPID, String msg) {
		Map<String, String> msgMap = Global.parseMsg(msg);
		String msgType = msgMap.get("msg_type");
		if (Global.MSG_TYPE_ONLINE_STATUS_DETECT.equals(msgType)) {
			System.out.println("cxd, msg:"+msg);
			String msgCode = msgMap.get("msg_code");
			if (Global.MSG_TYPE_ONLINE_STATUS_DETECT_ACK.equals(msgCode)) {
				String str[] = msgMap.get("msg").split(",");
				if(onlineStateDetectionListener != null)
					onlineStateDetectionListener.onAckDetected(str[0], str[1]);
			}else if(Global.MSG_TYPE_ONLINE_STATUS_DETECT_HEARTBEAT.equals(msgCode)){
				String str[] = msgMap.get("msg").split(",");
        		String message = Global.catMsg(Global.MSG_TYPE_ONLINE_STATUS_DETECT , Global.MSG_TYPE_ONLINE_STATUS_DETECT_ACK, str[0] + "," + str[1]);
        		if(onlineStateDetectionListener != null)
					onlineStateDetectionListener.onAckDetected(str[1], message);
			}
		} else {
			if(dataListener != null)
				dataListener.onStringMsgReceived(remoteGPID, msg);
		}
		
	}
}
