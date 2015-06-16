package com.arcsoft.ais.arcvc.utils;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.adapter.ViewAdapter;
import com.arcsoft.ais.arcvc.bean.FriendOnlineStatus;
import com.arcsoft.ais.arcvc.bean.PeerIdOnlineStatus;

public class OnlineStatusDetector implements Runnable {
	
	public static Map<String,FriendOnlineStatus> friendOnlineStatusMap = new HashMap<String,FriendOnlineStatus>();
    ViewAdapter adapter; 
    List<Map<String, Object>> listItem; 
    Context context;
    Handler handler;
    String currentPeerId;
	
   /* public static Map<String,FriendOnlineStatus> init(Map<String,FriendOnlineStatus> onlineStatusMap){
    	
    	return onlineStatusMap ;
    }*/
    
    public OnlineStatusDetector(ViewAdapter adapter, List<Map<String, Object>> listItem, Context context, Map<String,FriendOnlineStatus> onlineStatusMap, Handler handler, String currentPeerId){
    	this.adapter = adapter;
    	this.listItem = listItem;
    	this.context = context;
    	this.handler = handler;
    	this.currentPeerId = currentPeerId;
    	if (onlineStatusMap == null) {
			Log.i(Global.TAG, "this onlineStatusMap is null!");
		}
    	friendOnlineStatusMap = onlineStatusMap ;
		Log.i(Global.TAG, "detect onlineStatusMap: " + onlineStatusMap);
    }
    
    public void run(){
    	 //test(adapter, listItem, context, handler);
    	detect(adapter, listItem, context, handler, currentPeerId);
    }
    
    public void test(ViewAdapter adapter, List<Map<String, Object>> listItem, Context context, Handler handler, String currentPeerId){
    	try {
			Thread.sleep(10*10000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    	
    	for(int i=0; i<listItem.size(); i++){
			Map<String, Object> map = listItem.get(i);
			if("4".equals(map.get("userId").toString())){
				map.put("ItemImage", BitmapFactory.decodeResource(context.getApplicationContext().getResources(), R.drawable.ic_launcher));
				
				Message msg = new Message();
		    	msg.obj = map;
		    	msg.arg1 = i;
		    	msg.what = 4;
		    	handler.sendMessage(msg);
			}
		}
    }

    public static void detect(ViewAdapter adapter, List<Map<String, Object>> listItem, Context context, Handler handler, String currentPeerId){
    	if (friendOnlineStatusMap == null) {
			Log.i(Global.TAG, "this onlineStatusMap is null!");
		}
    	while(true){
        	for (String userId : friendOnlineStatusMap.keySet()) {
        		//detect
        		//onlineStatusMap.put(userId, Global.FRIEND_ONLINE_STATUS_ONLINE);
        		FriendOnlineStatus friendOnlineStatus = friendOnlineStatusMap.get(userId) ;
        		//Log.i(Global.TAG, "detect getUserId: " + friendOnlineStatus.getUserId());
        		//Log.i(Global.TAG, "detect getNickname: " + friendOnlineStatus.getNickname());
        		//send msg
        		PeerIdOnlineStatus peerIdOnlineStatus[] = friendOnlineStatus.peerIdOnlineStatus;
        		
        		for (PeerIdOnlineStatus peerIdOnlineStatus2 : peerIdOnlineStatus) {
            		//msg 包含的信息：对方userId+发送方peerId+对方peerId
        			//Log.i(Global.TAG, "OnlineStatusDetector detect==currentPeerId=="+currentPeerId);
        			//Log.i(Global.TAG, "OnlineStatusDetector detect peerIdOnlineStatus2.getPeerId(): " + peerIdOnlineStatus2.getPeerId());
            		if (!currentPeerId.equals(peerIdOnlineStatus2.getPeerId())) {
            		String message = Global.catMsg(Global.MSG_TYPE_ONLINE_STATUS_DETECT, 
            					Global.MSG_TYPE_ONLINE_STATUS_DETECT_HEARTBEAT,
            					String.valueOf(friendOnlineStatus.getUserId()) + "," + currentPeerId + "," +peerIdOnlineStatus2.getPeerId());
                		P2PClientManager.getP2PClientInstance().sendMsg(peerIdOnlineStatus2.getPeerId(), message);
                		//Log.i(Global.TAG, "OnlineStatusDetector detect==message=="+message);
                		//Log.i(Global.TAG, "P2PClientManager.getP2PClientInstance().sendMsg====sended");
					}else{
						//Log.i(Global.TAG, "P2PClientManager.getP2PClientInstance().sendMsg====not send");
					}
            		//Log.i(Global.TAG, "OnlineStatusDetector detect====ended");
				}
        		
        		friendOnlineStatus.setReceivedMsgFlag(Global.RECEIVED_MSG_FLAG_NO);
        		friendOnlineStatus.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_ONLINE);
        		friendOnlineStatusMap.put(userId, friendOnlineStatus);
        		
    		}
        	//Log.i(Global.TAG, "detect onlineStatusMap: " + friendOnlineStatusMap);
    		try {
				Thread.sleep(10*1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
    		//update offline status
    		for (String userId : friendOnlineStatusMap.keySet()) {
        		FriendOnlineStatus friendOnlineStatus = friendOnlineStatusMap.get(userId) ;
        		if (Global.RECEIVED_MSG_FLAG_NO.equals(friendOnlineStatus.getReceivedMsgFlag())) {
            		friendOnlineStatus.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_OFFLINE);
            		PeerIdOnlineStatus peerIdOnlineStatus[] = friendOnlineStatus.peerIdOnlineStatus;
            		for (PeerIdOnlineStatus peerIdOnlineStatus2 : peerIdOnlineStatus) {
        	    		peerIdOnlineStatus2.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_OFFLINE);	
            		}
            		for(int i=0; i<listItem.size(); i++){
	        			Map<String, Object> map = listItem.get(i);
	        			if(userId.equals(map.get("userId").toString())){
	        				map.put("ItemImage", MyBitmap.convertGreyImg(BitmapFactory.decodeResource(context.getApplicationContext().getResources(), R.drawable.ic_launcher)));
	        				//map.put("ItemImage", MyBitmap.convertGreyImg(BitmapFactory.decodeResource(context.getApplicationContext().getResources(), R.drawable.ic_launcher)));
	        				Message msg = new Message();
	        		    	msg.obj = map;
	        		    	msg.arg1 = i;
	        		    	msg.what = 4;
	        		    	handler.sendMessage(msg);
	        			}
	        		}
				} else {
					PeerIdOnlineStatus peerIdOnlineStatus[] = friendOnlineStatus.peerIdOnlineStatus;
					for (PeerIdOnlineStatus peerIdOnlineStatus2 : peerIdOnlineStatus) {
//	        			if (peerIdOnlineStatus2.getPeerId().equals(peerId)) {
//	        	    		Log.i(Global.TAG, "detect peerIdOnlineStatus2.getPeerId(): " + peerIdOnlineStatus2.getPeerId());
//	        	    		peerIdOnlineStatus2.setOnlineStatus(Global.RECEIVED_MSG_FLAG_YES);	
//	        			} 
	        		}
	        		friendOnlineStatus.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_ONLINE);
	        		//listView.
	        		for(int i=0; i<listItem.size(); i++){
	        			Map<String, Object> map = listItem.get(i);
	        			if(userId.equals(map.get("userId").toString())){
	        				map.put("ItemImage", BitmapFactory.decodeResource(context.getApplicationContext().getResources(), R.drawable.ic_launcher));
	        				Message msg = new Message();
	        		    	msg.obj = map;
	        		    	msg.arg1 = i;
	        		    	msg.what = 4;
	        		    	handler.sendMessage(msg);
	        			}
	        		}
				}

        		PeerIdOnlineStatus peerIdOnlineStatus[] = friendOnlineStatus.peerIdOnlineStatus;
        		
        		friendOnlineStatusMap.put(userId, friendOnlineStatus);
        		
        		//Log.i(Global.TAG, "detect key: " + userId);
        		//Log.i(Global.TAG, "detect value: " + userId);
    		}
    	}
    }
    
    public static String getOnlineStatus(String userId){
    	return friendOnlineStatusMap.get(userId).getOnlineStatus() ;
    }

    public static void setOnlineStatus(String userId,String peerId,String onlineStatus){
    	//Log.i(Global.TAG, "OnlineStatusDetector setOnlineStatus===userId=" + userId);
    	//Log.i(Global.TAG, "OnlineStatusDetector setOnlineStatus===peerId=" + peerId);
    	//Log.i(Global.TAG, "OnlineStatusDetector setOnlineStatus===onlineStatus=" + onlineStatus);
		FriendOnlineStatus friendOnlineStatus = friendOnlineStatusMap.get(userId) ;
		if(friendOnlineStatus == null)
			return;
		//Log.i(Global.TAG, "detect getUserId: " + friendOnlineStatus.getUserId());
		//Log.i(Global.TAG, "detect getNickname: " + friendOnlineStatus.getNickname());
		friendOnlineStatus.setReceivedMsgFlag(Global.RECEIVED_MSG_FLAG_YES);
		friendOnlineStatus.setOnlineStatus(onlineStatus);

		PeerIdOnlineStatus peerIdOnlineStatus[] = friendOnlineStatus.peerIdOnlineStatus;
		for (PeerIdOnlineStatus peerIdOnlineStatus2 : peerIdOnlineStatus) {
			if (peerIdOnlineStatus2.getPeerId().equals(peerId)) {
	    		Log.i(Global.TAG, "detect peerIdOnlineStatus2.getPeerId(): " + peerIdOnlineStatus2.getPeerId());
	    		peerIdOnlineStatus2.setOnlineStatus(Global.RECEIVED_MSG_FLAG_YES);	
			} 
		}
		
		friendOnlineStatusMap.put(userId, friendOnlineStatus);
    }

	public static Map<String, FriendOnlineStatus> getFriendOnlineStatusMap() {
		return friendOnlineStatusMap;
	}

	public static void setFriendOnlineStatusMap(
			Map<String, FriendOnlineStatus> friendOnlineStatusMap) {
		OnlineStatusDetector.friendOnlineStatusMap = friendOnlineStatusMap;
	}

}
