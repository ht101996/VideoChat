//package com.arcsoft.ais.arcvc.utils;
//
//import java.text.SimpleDateFormat;
//import java.util.Date;
//import java.util.Locale;
//import java.util.Map;
//
//import android.app.AlertDialog;
//import android.content.DialogInterface;
//import android.graphics.Bitmap;
//import android.os.Message;
//import android.text.Editable;
//import android.util.Log;
//import android.widget.EditText;
//
//public class HandlerUtils {
//
//
//	private static void  handlerHandleMsg(Message msg,String currentPeerId,
//			String remotePeerId,
//			EditText editText_receive_msg,
//			String remotePeerId2,
//			String friendNickname,
//			AlertDialog.Builder alertDialogBuilder
//			) {
//		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss", Locale.US);
//		if (msg.what == 1) {
//			remotePeerId = msg.getData().getString("gpid");
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId:======" + currentPeerId);
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId:======" + remotePeerId);
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg:======" + msg.getData().getString("msg"));
//			if (remotePeerId.equals(currentPeerId)) {
//				return;
//			}
//			Map<String, String> msgMap = Global.parseMsg(msg.getData().getString("msg"));
//			String msgType = msgMap.get("msg_type");
//			
////			if(Global.MSG_TYPE_ONLINE_STATUS_DETECT.equals(msgType)){
////				String msgCode = msgMap.get("msg_code");
////				if(Global.MSG_TYPE_ONLINE_STATUS_DETECT_ACK.equals(msgCode)){
////					OnlineStatusDetector.setOnlineStatus(msgMap.get("msg"), remotePeerId, Global.FRIEND_ONLINE_STATUS_ONLINE);
////				}
////				
////			} else 
//				if (Global.MSG_TYPE_TEXT_CHATTING_SEND_MSG.equals(msgType)) {
//				// ///////////////////////////////////////////////////////////////////////
//				// MSG_TYPE_TEXT_CHATTING_MSG
//				Editable text = editText_receive_msg.getText();
//				text.append(friendNickname + " " + format.format(new Date()) + "\n" + msgMap.get("msg") + "\n");
//
//			} else if (Global.MSG_TYPE_VIDEO_CHATTING_REQUEST.equals(msgType)) {
//				// ///////////////////////////////////////////////////////////////////////
//				// MSG_TYPE_VIDEO_CHATTING_REQUEST
//				String nickname = friendNickname;
//				// alertDialog(nickname) ;
//				String msgCode = msgMap.get("msg_code");
//				Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgType======" + msgType);
//				Log.i(Global.TAG, " MSG_TYPE_VIDEO_CHATTING_REQUEST>>>..msgCode======" + msgCode);
//				if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_APPLY)) {
//					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
//							.setMessage(nickname + " want to Video chat with you,would you like to accept it? ")
//							.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
//								@Override
//								public void onClick(DialogInterface dialog, int which) {
//									// 转跳到另外一个Activity
//									Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//									Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//									String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,
//											Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT, "Accept");
//									P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);
//									
//									startCameraRecording() ;
//									Log.i(Global.TAG, "Accept====");
//								}
//							}).setNegativeButton("Reject", new DialogInterface.OnClickListener() {
//								public void onClick(DialogInterface dialog, int which) {
//									Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//									Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//									String message = Global.catMsg(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST,
//											Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT, "Reject");
//									P2PClientManager.getP2PClientInstance().sendMsg(remotePeerId, message);
//									Log.i(Global.TAG, "Reject====");
//
//									dialog.cancel();// 取消弹出框
//								}
//							}).create().show();
//				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_ACCEPT)) {
//					Log.i(Global.TAG, nickname + " has accepted it !" );
//					Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//					Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//					Log.i(Global.TAG, "Accept====" + "Your friend has accepted it !");
//					applyAlertDialog.dismiss();
//					Log.i(Global.TAG, "applyAlertDialog.dismiss()====");
//					startCameraRecording() ;
//				} else if (msgCode.equals(Global.MSG_TYPE_VIDEO_CHATTING_REQUEST_REJECT)) {
//					alertDialogBuilder.setTitle("Video Chatting Request").setIcon(android.R.drawable.ic_dialog_info)
//							.setMessage(nickname + " has rejected it !").setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
//								@Override
//								public void onClick(DialogInterface dialog, int which) {
//									// recieve smg
//									Log.i(Global.TAG, "p2p sendMsg>>>..from peerId======" + currentPeerId);
//									Log.i(Global.TAG, "p2p sendMsg>>>..to peerId======" + remotePeerId);
//									Log.i(Global.TAG, "Accept====" + "Your friend has rejected it !");
//									dialog.cancel();// 取消弹出框
//								}
//							}).create().show();
//
//				}
//
//			}
//
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId: " + Configer.getValue("peer_id"));
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId: " + msg.getData().getString("gpid"));
//			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg: " + msg.getData().getString("msg"));
//		} else if (msg.what == 2) { // video packet
//			Bitmap image = MyBitmap.createMyBitmap(msg.getData().getIntArray("packet"), msg.getData().getInt("width"),
//					msg.getData().getInt("height"));
//			iv_friend.setImageBitmap(image);
//		}
//	}
//};
//
//}
