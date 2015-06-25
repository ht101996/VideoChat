package com.arcsoft.ais.arcvc.activity;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.adapter.ViewAdapter;
import com.arcsoft.ais.arcvc.bean.FriendOnlineStatus;
import com.arcsoft.ais.arcvc.bean.PeerIdOnlineStatus;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.MyBitmap;
import com.arcsoft.ais.arcvc.utils.OnlineStatusDetector;
import com.arcsoft.coreapi.sdk.CoreVCAPI;
import com.arcsoft.coreapi.sdk.CoreVCDef.FriendListResponse;
import com.arcsoft.coreapi.sdk.CoreVCDef.FriendResponse;
import com.arcsoft.coreapi.sdk.CoreVCDef.PeerIdResponse;

/**
 * Activity which displays FriendListActivity screen to the user
 */
public class FriendListActivity extends Activity {
	private static String[] friendListArray;
	private static FriendResponse[] friendResponseArray ;

	private TextView view;
	private Spinner spinner;
	private Button button1;
	private ArrayAdapter<String> adapter;
	private static ViewAdapter viewAdapter;
	private static String remotePeerId;
	private static String currentPeerId;
	private static Dialog applyAlertDialog ;
	private static AlertDialog.Builder alertDialogBuilder;
	private static String friendNickname;
	Map<String,FriendOnlineStatus> friendOnlineStatusMap = new HashMap<String,FriendOnlineStatus>();
	List<Map<String, Object>> listItem = new ArrayList<Map<String, Object>>(); 
//	private P2PClient.OnlineStateDetectionListener onlineListener = new P2PClient.OnlineStateDetectionListener() {
//
//		@Override
//		public void onAckDetected(String arg0, String arg1) {
//			OnlineStatusDetector.setOnlineStatus(arg0, arg1, Global.FRIEND_ONLINE_STATUS_ONLINE);
//		}
//
//		@Override
//		public void onheartbeatDetected(String arg0, String arg1) {
//			P2PClientManager.getP2PClientInstance().sendMsg(arg0, arg1);
//		}
//		
//	};
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);		
		FriendListResponse friendListResponse = new FriendListResponse();
		int ret = CoreVCAPI.getInstance().friendList(friendListResponse);
		Log.i(Global.TAG, "login ret = " + ret);
		setContentView(R.layout.activity_friend_list);

		setTitle(getTitle()+" of "+Configer.getValue("nickname"));
		//初始化好友在线状态
		//Map<String,FriendOnlineStatus> friendOnlineStatusMap = new HashMap<String,FriendOnlineStatus>();
		
		//List<Map<String, Object>> listItem = new ArrayList<Map<String, Object>>(); 
		//显示好友列表
		ListView listView = (ListView) findViewById(R.id.ListView);

		if(0 == ret) {//返回成功
			Log.i(Global.TAG, "friendListResponse.pFriendResponse.length = " 
						+ friendListResponse.pFriendResponse.length);
			
			FriendResponse pFriendResponse[] = friendListResponse.pFriendResponse;
			friendResponseArray = pFriendResponse ;
			
			if (pFriendResponse.length > 0) {
				friendListArray = new String[pFriendResponse.length];
				//friendListArray[0] = "" ;
				for (int i = 0; i < pFriendResponse.length; i++) {
					FriendResponse friendResponse = pFriendResponse[i] ;
					Log.i(Global.TAG, "friendResponse.szNickname == " + friendResponse.szNickname);
					
					Log.i(Global.TAG, "friendResponse.userId == " + friendResponse.userId);
					PeerIdOnlineStatus[] peerIdOnlineStatus = new PeerIdOnlineStatus[friendResponse.pPeerIdResponse.length];
					friendListArray[i] = friendResponse.szNickname;
					//check if the peers of one user is online? TODO
					for (int j = 0; j < friendResponse.pPeerIdResponse.length; j++) {
						PeerIdOnlineStatus pols = new PeerIdOnlineStatus();
						pols.setPeerId(friendResponse.pPeerIdResponse[j].szPeerId);
						pols.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_OFFLINE);
						peerIdOnlineStatus[j] = pols;
						Log.i(Global.TAG, "friendResponse.pPeerIdResponse.szPeerId == " + 
									friendResponse.pPeerIdResponse[j].szPeerId);
					}
					FriendOnlineStatus friendOnlineStatus = new FriendOnlineStatus();
					Long userId = friendResponse.userId;
					friendOnlineStatus.setNickname(friendResponse.szNickname);
					friendOnlineStatus.setOnlineStatus(Global.FRIEND_ONLINE_STATUS_OFFLINE);
					friendOnlineStatus.setPeerIdOnlineStatus(peerIdOnlineStatus);
					friendOnlineStatus.setUserId(friendResponse.userId);
					friendOnlineStatus.setReceivedMsgFlag(Global.RECEIVED_MSG_FLAG_NO);
					friendOnlineStatusMap.put(userId.toString(), friendOnlineStatus);
		            HashMap<String, Object> map = new HashMap<String, Object>();
		            Bitmap mBitmap = BitmapFactory.decodeResource(getApplicationContext().getResources(), R.drawable.ic_launcher); 
		            //ImageView image = (ImageView)findViewById(R.id.ItemImage);
		            //image.setImageBitmap(convertGreyImg(mBitmap));
		            if(Global.FRIEND_ONLINE_STATUS_ONLINE.equals(OnlineStatusDetector.friendOnlineStatusMap.get(pFriendResponse[i].userId)))
		            	map.put("ItemImage", mBitmap);
		            else 
		            	map.put("ItemImage", MyBitmap.convertGreyImg(mBitmap));
		            map.put("ItemText", pFriendResponse[i].szNickname);
		            map.put("userId", pFriendResponse[i].userId);
		            listItem.add(map);  
				}
			}
			
		}
		
		viewAdapter = new ViewAdapter(listItem, this);
        //添加并且显示  
        listView.setAdapter(viewAdapter);  
        
        //设置点击listview事件
        listView.setOnItemClickListener(new ListViewListener());
        
        //发送消息给好友,侦探好友是否在线
        //new TestAsyncTask(viewAdapter, listItem, this, friendOnlineStatusMap, handler).execute(new OnlineStatusDetector(viewAdapter, listItem, this, friendOnlineStatusMap, handler));
       /* Thread mThread = new Thread(new OnlineStatusDetector(viewAdapter, listItem, this, friendOnlineStatusMap, handler));
        mThread.start();*/

        alertDialogBuilder = new AlertDialog.Builder(FriendListActivity.this);

		// getLocalGpid
//		String localGpid = P2PClientManager.getP2PClientInstance().getLocalGpid();
//		Log.i(Global.TAG, "getLocalGpid============" + localGpid);
//		Configer.setValue("peer_id", localGpid);
		currentPeerId = Configer.getValue("peer_id");
//		P2PClientManager.getP2PClientInstance().setOnlineStateDetectionListener(onlineListener);
		Log.i(Global.TAG, "FriendListActivity: onCreate============finished!");
	}
	
	@Override
    protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);
//		Thread mThread = new Thread(new OnlineStatusDetector(viewAdapter, listItem, this, friendOnlineStatusMap, handler, currentPeerId));
//	    mThread.start();
    }

	@Override
	protected void onResume(){
		super.onResume();
		Log.i(Global.TAG, "FriendListActivity --------------onResume!");
	}; 

	
	class ListViewListener implements OnItemClickListener{
		@Override
		public void onItemClick(AdapterView<?> arg0, View arg1, int positionIndex,
				long arg3) {
			 	Intent intent = new Intent();
		        intent.putExtra("friendUserId",friendResponseArray[positionIndex].userId);   
		        intent.putExtra("friendNickname",friendListArray[positionIndex ]);  
		        ArrayList<String> peerIdsListItem = new ArrayList<String>();
		        for (PeerIdResponse peerIdResponse : friendResponseArray[positionIndex].pPeerIdResponse) {
		        	peerIdsListItem.add(peerIdResponse.szPeerId);
				}
		        intent.putExtra("friendPeerIds", peerIdsListItem);
//		        /* 指定intent要启动的类 */
//		        if(android.os.Build.VERSION.SDK_INT > 20)
//		        	intent.setClass(FriendListActivity.this, VideoActivity.class);
//		        else
		        	intent.setClass(FriendListActivity.this, VideoChatActivity.class);
		        /* 启动一个新的Activity */
		        startActivity(intent);
		        /* 关闭当前的Activity */
		        finish();
		}
		
	}
//	class ButtonListener implements OnClickListener{
//		 
//        @Override
//        public void onClick(View v) {
//        	
//        	  AlertDialog.Builder dialog=new AlertDialog.Builder(FriendListActivity.this);
//        	  
//        	  String nickname = "nickname" ;
//              dialog.setTitle("Video Chatting Request")
//                    .setIcon(android.R.drawable.ic_dialog_info)
//                    .setMessage(nickname + " want to Video chat with you,would you like to accept it? ")
//                    .setPositiveButton("Accept",new DialogInterface.OnClickListener() {
//				               @Override
//				               public void onClick(DialogInterface dialog, int which) {
//				                   //转跳到另外一个Activity
//				                   // TODO Auto-generated method stub
//				            	   //Intent intent=new Intent();
//				            	   //intent.setClass(getApplicationContext(), list.class);
//				            	   //startActivity(intent);
//
//					           		Log.i(Global.TAG, "Accept====");
//				               }
//                    		})
//                    .setNegativeButton("Reject", new DialogInterface.OnClickListener() {
//				               public void onClick(DialogInterface dialog, int which) {
//				                   // TODO Auto-generated method stub
//
//				           		Log.i(Global.TAG, "Reject====");
//				                   dialog.cancel();//取消弹出框
//				               }
//                    		})
//                    .create()
//                    .show();
//        }
//   }
	
	// 使用数组形式操作
		class SpinnerSelectedListener implements OnItemSelectedListener {
			@Override
			public void onItemSelected(AdapterView<?> adapterView, View view2, int positionIndex,
					long id) {

	            String selected = adapterView.getItemAtPosition(positionIndex).toString();
	            System.out.println("selected===========>" + selected);
	            
				view.setText("the friend you selected is：" + friendListArray[positionIndex]);
//				FriendResponse pFriendResponse[] = friendListResponse.pFriendResponse;
				//change to video activity
				  /* 新建一个Intent对象 */
		        Intent intent = new Intent();
		        intent.putExtra("friendUserId",friendResponseArray[positionIndex - 1].userId);   
		        intent.putExtra("friendNickname",friendListArray[positionIndex ]);  
		        ArrayList<String> peerIdsListItem = new ArrayList<String>();
		        for (PeerIdResponse peerIdResponse : friendResponseArray[positionIndex - 1].pPeerIdResponse) {
		        	peerIdsListItem.add(peerIdResponse.szPeerId);
				}
		        intent.putExtra("friendPeerIds", peerIdsListItem);
		        /* 指定intent要启动的类 */
//		        intent.setClass(FriendListActivity.this, VideoActivity.class);
		        intent.setClass(FriendListActivity.this, TextActivity.class);
		        /* 启动一个新的Activity */
		        startActivity(intent);
		        /* 关闭当前的Activity */
		        finish();
			}
			@Override
			public void onNothingSelected(AdapterView<?> arg0) {
	            System.out.println("selected===========>" + "Nothing");
			}

		}

//		private final MyHandler handler = new MyHandler(this);
//		public Handler getHandler() {
//			return handler;
//		}
//		 private static class MyHandler extends Handler {
////		private static Handler handler = new Handler() {
//			private final WeakReference<Activity> myActivity;
//			public MyHandler(FriendListActivity activity) {
//			      myActivity = new WeakReference<Activity>(activity);
//			}
//			SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss"); 
//
//			@Override
//			public void handleMessage(Message msg) {
//				if (msg == null) {
//					return ;
//				}
//				//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..msg.what:"+msg.what);
//				Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..msg:"+msg.getData().getString("msg"));
//				//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..currentPeerId:"+currentPeerId);
//				
//				if (3 == msg.what) {
//					Map<String, String> msgMap = Global.parseMsg(msg.getData().getString("msg"));
//					String msgType = msgMap.get("msg_type");
//					if (Global.MSG_TYPE_ONLINE_STATUS_DETECT.equals(msgType)) {
//						String msgCode = msgMap.get("msg_code");
//						if (Global.MSG_TYPE_ONLINE_STATUS_DETECT_ACK.equals(msgCode)) {
//							String str[] = msgMap.get("msg").split(",");
//							OnlineStatusDetector.setOnlineStatus(str[0], str[1] , Global.FRIEND_ONLINE_STATUS_ONLINE);
//						}else if(Global.MSG_TYPE_ONLINE_STATUS_DETECT_HEARTBEAT.equals(msgCode)){
//							String str[] = msgMap.get("msg").split(",");
//		            		String message = Global.catMsg(Global.MSG_TYPE_ONLINE_STATUS_DETECT , Global.MSG_TYPE_ONLINE_STATUS_DETECT_ACK ,
//		            				str[0] + "," + str[1]);
//		            		P2PClientManager.getP2PClientInstance().sendMsg(str[1], message);
//						}
//					}
//				 }
//				
//				if (4 == msg.what) {
//					Map<String, Object> map = new HashMap<String, Object>();
//					map = (Map<String, Object>) msg.obj;
//					int index = msg.arg1;
//					//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..map:"+map);
//					//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..msg.arg1:"+msg.arg1);
//					viewAdapter.changeImage(index, map);
//				}
//				
//				FriendListActivity activity = (FriendListActivity) myActivity.get();
//				if (activity != null) {
//					 //handleMessage
//					if (msg!=null) {
//						handlerHandleMsg(msg,activity);
//					}
//				}
//			}
//		};
//		
//		private static void handlerHandleMsg(Message msg,final FriendListActivity activity) {
//			if (msg.what == 1) {
//				remotePeerId = msg.getData().getString("gpid");
//				//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..current peerId:======" + currentPeerId);
//				//Log.i(Global.TAG, "FriendListActivity Handler handleMessage>>>..from peerId:======" + remotePeerId);
//				//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg:======" + msg.getData().getString("msg"));
//				if (remotePeerId.equals(currentPeerId)) {
//					return;
//				}
//				Map<String, String> msgMap = Global.parseMsg(msg.getData().getString("msg"));
//				String msgType = msgMap.get("msg_type");
//				String msgContent = msgMap.get("msg");
//				friendNickname = msgContent ;
//				//Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgType======" + msgType);
//				//Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgContent======" + msgContent);
//				if (Global.MSG_TYPE_VIDEO_CHATTING_REQUEST.equals(msgType)) {
//					// ///////////////////////////////////////////////////////////////////////
//					// MSG_TYPE_VIDEO_CHATTING_REQUEST
//					// alertDialog(nickname) ;
//					String msgCode = msgMap.get("msg_code");
//					//Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgCode======" + msgCode);
//					if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY)) {
//						alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
//								.setMessage(friendNickname + " want to Video chat with you,would you like to accept it? ")
//								.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
//									@Override
//									public void onClick(DialogInterface dialog, int which) {
//										// 转跳到另外一个Activity
//										//Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//										//Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//										P2PClientManager.getP2PClientInstance().acceptVideoChat(remotePeerId);
//										//startCameraRecording() ;
//										 Intent intent = new Intent();
//									        intent.putExtra("friendUserId",11l);   
//									        intent.putExtra("friendNickname",friendNickname);  
//									        intent.putExtra("startCameraRecordingFlag",Global.START_CAMERA_RECORDING_FLAG_YES);  
//									        ArrayList<String> peerIdsListItem = new ArrayList<String>();
//									        peerIdsListItem.add(remotePeerId);
//									        intent.putExtra("friendPeerIds", peerIdsListItem);
//									        /* 指定intent要启动的类 */
//									        intent.setClass(activity, VideoActivity.class);
//									        /* 启动一个新的Activity */
//									        activity.startActivity(intent);
//									        /* 关闭当前的Activity */
//									        activity.finish();
//									      //Log.i(Global.TAG, "Accept====");
//									}
//								}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
//									public void onClick(DialogInterface dialog, int which) {
//										//Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//										//Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//										P2PClientManager.getP2PClientInstance().rejectVideoChat(remotePeerId);
//										//Log.i(Global.TAG, "Reject====");
//										dialog.cancel();// 取消弹出框
//									}
//								}).create().show();
//					} 
//				}
//			}
//		}
		
	// 使用数组形式操作
//	class SpinnerOnTouchListener implements OnTouchListener {
//
//		@Override
//		public boolean onTouch(View v, MotionEvent event) {
//			if (event.getAction() == MotionEvent.ACTION_DOWN) {
//
//				view.setText("the friend you selected is：" + friendListArray[positionIndex]);
////				FriendResponse pFriendResponse[] = friendListResponse.pFriendResponse;
//				//change to video activity
//				  /* 新建一个Intent对象 */
//		        Intent intent = new Intent();
//		        intent.putExtra("friendUserId",friendResponseArray[positionIndex].userId);   
//		        intent.putExtra("friendNickname",friendListArray[positionIndex]);  
//		        ArrayList<String> peerIdsListItem = new ArrayList<String>();
//		        for (PeerIdResponse peerIdResponse : friendResponseArray[positionIndex].pPeerIdResponse) {
//		        	peerIdsListItem.add(peerIdResponse.szPeerId);
//				}
//		        intent.putExtra("friendPeerIds", peerIdsListItem);
//		        /* 指定intent要启动的类 */
//		        intent.setClass(FriendListActivity.this, VideoActivity.class);
//		        /* 启动一个新的Activity */
//		        //startActivity(intent);
//		        /* 关闭当前的Activity */
//		        //finish();
//			}
//		
//			return false;
//		}
//	}

}
