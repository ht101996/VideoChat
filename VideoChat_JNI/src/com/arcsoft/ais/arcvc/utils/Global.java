package com.arcsoft.ais.arcvc.utils;

import java.util.HashMap;
import java.util.Map;

public class Global {
	public static final String TAG = "ArcVC-DEMO";
	public static final String MSG_FORMAT = "1:nia";//MSG_TYPE:MSG_TYPE_INDEX:nickname  eg. 1:9:heng dong
	public static final String MSG_TYPE_VIDEO_CHATTING_REQUEST 			= "1";
	public static final String MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY 	= "9";
	public static final String MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT 	= "1";
	public static final String MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT 	= "0";
	public static final String MSG_TYPE_VIDEO_CHATTING_REQUEST_END      = "5";
	
	public static final String MSG_TYPE_TEXT_CHATTING_SEND_MSG 			     = "2";//2:0:msg
	public static final String MSG_TYPE_TEXT_CHATTING_SEND_MSG_DEFAULT       = "0";

	public static final String MSG_TYPE_ONLINE_STATUS_DETECT 			     = "3";//3:0:userId
	public static final String MSG_TYPE_ONLINE_STATUS_DETECT_HEARTBEAT       = "0";
	public static final String MSG_TYPE_ONLINE_STATUS_DETECT_ACK         	 = "1";//为一则为返回的消息
	
	public static final String FRIEND_ONLINE_STATUS_ONLINE 			    	= "1";
	public static final String FRIEND_ONLINE_STATUS_OFFLINE 			    = "0";
	//receivedMsgFlag  
	public static final String RECEIVED_MSG_FLAG_YES 			    			= "1";
	public static final String RECEIVED_MSG_FLAG_NO		 			    		= "0";
	public static final String RECEIVED_MSG_FLAG_DEFAULT		 			    = "9";
	//startCameraRecordingFlag
	public static final String START_CAMERA_RECORDING_FLAG_YES 			    	= "1";
	public static final String START_CAMERA_RECORDING_FLAG_NO		 			= "0";
	
	public static String catMsg(String msgType,String msgCode,String msg){
		return msgType + ":" + msgCode + ":"+ msg ;
	}

	public static Map<String,String> parseMsg(String msg){
		Map<String,String> msgMap = new HashMap<String,String>();
		String msgArray[] = msg.split(":") ;
		msgMap.put("msg_type", msgArray[0]);
		msgMap.put("msg_code", msgArray[1]);
		msgMap.put("msg", msgArray[2]);
		return msgMap ;
	}
}
