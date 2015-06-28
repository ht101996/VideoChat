package com.arcsoft.ais.arcvc.activity;

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.ScrollView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.adapter.DialogAdapter;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.arcsoft.coreapi.sdk.CoreVCDef.PeerIdResponse;
import com.arcsoft.videochat.activity.VideoChatApplication;

public class TextActivity extends Activity implements View.OnClickListener {

	static EditText editText_send_msg;
	static Button button_pause ;
	static EditText editText_receive_msg;
	static ScrollView mScrollView;
	static String friendNickname;
	static String currentNickname;
	static String remotePeerId;
	static String currentPeerId;
	static List<String> peerIdsOfFriends;
	static ListView listView;
	static DialogAdapter dialogAdapter;
	static long friendUserId;
	static ArrayList<String> friendPeerIds;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
//		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_text);
//		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE,R.layout.mycustomtitle); 
		editText_send_msg = (EditText) findViewById(R.id.editText1);
		editText_receive_msg = (EditText) findViewById(R.id.editText2);
//		button_pause = (Button) findViewById(R.id.button_pause);
		// myCamera = CameraUtils.getFrontCamera();
		// myCamera.setDisplayOrientation(90);
		Intent intent = getIntent();
		friendUserId = intent.getLongExtra("friendUserId", 0l);
		friendNickname = intent.getStringExtra("friendNickname");
		currentNickname = Configer.getValue("nickname");
		friendPeerIds = intent.getStringArrayListExtra("friendPeerIds");
		listView = (ListView) findViewById(R.id.listView);

		setTitle(getTitle()+" with "+friendNickname);
		
		setListViewHeightBasedOnChildren(listView);

		dialogAdapter = new DialogAdapter(this);
		listView.setAdapter(dialogAdapter);
		
		Log.i(Global.TAG, "friendUserId= " + friendUserId);
		Log.i(Global.TAG, "friendNickname= " + friendNickname);
		for (String friendPeerId : friendPeerIds) {
			Log.i(Global.TAG, "friendPeerId= " + friendPeerId);
		}
		currentPeerId = Configer.getValue("peer_id");
		if (friendPeerIds == null || friendPeerIds.size() < 1) {
			Log.i(Global.TAG, "friendPeerIds.size() == 0>>>...error !!");
		} else {
			peerIdsOfFriends = friendPeerIds;
			remotePeerId = friendPeerIds.get(0);
			Log.i(Global.TAG, "friendPeerIds.get(0)>>>..." + friendPeerIds.get(0));
		}
		Log.i(Global.TAG, "remotePeerId>>>..." + remotePeerId);
		// startRTPSession
		Log.i(Global.TAG, "P2PClient.startRTPSession() >>>.start..");
		((VideoChatApplication)getApplication()).getP2PClient().startRTPSession(remotePeerId);
		Log.i(Global.TAG, "P2PClient.startRTPSession() sucessfully");


		mScrollView = (ScrollView) findViewById(R.id.scrollContent);
		// mScrollView.setVerticalScrollBarEnabled(false);//禁用垂直滚动条
		mScrollView.setOnTouchListener(touchListener);
		editText_send_msg.setOnClickListener(this);
		editText_send_msg.setOnKeyListener(myOnKeyListener);
		// mScrollView.setVerticalScrollBarEnabled(false);
		// mScrollView.setHorizontalScrollBarEnabled(false);
		Log.i(Global.TAG, "VideoActivity: onCreate============finished!");
	}

	OnTouchListener touchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_MOVE) {

				// TODO Auto-gener
				InputMethodManager imm = (InputMethodManager) getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
				/*
				 * // 显示或者隐藏输入法 imm.toggleSoftInput(0,
				 * InputMethodManager.HIDE_NOT_ALWAYS);
				 */
				if (imm.isActive()) {
					imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
				}
			}

			return false;
		}

	};

	/**
	 * @param listView
	 */
	private static void setListViewHeightBasedOnChildren(ListView listView) {

		ListAdapter listAdapter = listView.getAdapter();
		if (listAdapter == null) {
			return;
		}

		int totalHeight = 0;
		for (int i = 0; i < listAdapter.getCount(); i++) {
			View listItem = listAdapter.getView(i, null, listView);
			listItem.measure(0, 0);
			totalHeight += listItem.getMeasuredHeight();
		}

		ViewGroup.LayoutParams params = listView.getLayoutParams();
		params.height = totalHeight + (listView.getDividerHeight() * (listAdapter.getCount() - 1));
		listView.setLayoutParams(params);
	}


	@Override
	protected void onResume() {
		super.onResume();
		Log.i(Global.TAG, "VideoActivity --------------onResume!");
//		P2PClientManager.getP2PClientInstance().addHandler(handler);
	};

	@Override
	protected void onPause() {
		super.onPause();
		Log.i(Global.TAG, "VideoActivity --------------onPause!");
//		P2PClientManager.getP2PClientInstance().removeHandler(handler);
	};

	@Override
	protected void onStop() {
		super.onStop();
		Log.i(Global.TAG, "VideoActivity --------------onStop!");
	};

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		Log.i(Global.TAG, "VideoActivity --------------onPostCreate!");
		super.onPostCreate(savedInstanceState);
		
    }
//	@Override
//	private void onPostExecute(Bundle savedInstanceState) {
//		// TODO Auto-generated method stub
//
//	}

	@Override
	public void onClick(View v) {

		// 这里必须要给一个延迟，如果不加延迟则没有效果。我现在还没想明白为什么
		handler.postDelayed(new Runnable() {

			@Override
			public void run() {
				// 将ScrollView滚动到底
				mScrollView.fullScroll(View.FOCUS_DOWN);
				// mScrollView.setVerticalScrollBarEnabled(true);
			}
		}, 100);

	}
	
	public void onVideoBtnClickEvent(View view){
	 	Intent intent = new Intent();
        intent.putExtra("friendUserId",friendUserId);   
        intent.putExtra("friendNickname",friendNickname);  
        intent.putExtra("friendPeerIds", friendPeerIds);
        /* 指定intent要启动的类 */
        intent.setClass(TextActivity.this, VideoActivity.class);
        /* 启动一个新的Activity */
        startActivity(intent);
        /* 关闭当前的Activity */
        //finish();
	}
	
	OnKeyListener myOnKeyListener = new OnKeyListener() {

		@Override
		public boolean onKey(View v, int keyCode, KeyEvent event) {
			if (keyCode == KeyEvent.KEYCODE_ENTER) {
				sendTextMsg();
				InputMethodManager imm = (InputMethodManager) v.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);

				if (imm.isActive()) {
					imm.hideSoftInputFromWindow(v.getApplicationWindowToken(), 0);
				}
				return true;
			}
			return false;
		}

	};

	@Override
	protected void onDestroy() {
		Log.i(Global.TAG, " --------------onDestroy!");
		try {
			// P2PClientManager.getP2PClientInstance().uninit();
//			P2PClientManager.getP2PClientInstance().removeHandler(handler);
		} catch (Exception e) {
			e.printStackTrace();
		}
		super.onDestroy();
	}

	@Override
	public void onBackPressed() {
		Log.i(Global.TAG, " --------------onBackPressed!");
		super.onBackPressed();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.video, menu);
		return true;
	}

	public void onSendClickEvent(View view) {
		sendTextMsg();
	}

	private void sendTextMsg() {
		String msg = editText_send_msg.getText().toString();
		Log.i(Global.TAG, "onSendClickEvent>>>..remotePeerId." + remotePeerId);
		if (msg != null && msg.length() > 0) {
			Log.i(Global.TAG, "p2p sendMsg>>>...msg======" + msg);

			((VideoChatApplication)getApplication()).getP2PClient().sendTextMsg(remotePeerId, msg);
			// Editable text = editText_receive_msg.getText();

			SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss", Locale.US);
			// text.append(currentNickname + " " + format.format(new Date()) +
			// "\n" + msg + "\n");
			dialogAdapter.addList("1," + msg + "," + currentNickname);
			Log.i(Global.TAG, "p2p sendMsg>>> successfully!");
			editText_send_msg.setText("");
			listView.setSelection(listView.getBottom());
		}
	}


	private final MyHandler handler = new MyHandler(this);

	public Handler getHandler() {
		return handler;
	}

	private static class MyHandler extends Handler {

		// private static Handler handler = new Handler() {
		private final WeakReference<Activity> myActivity;

		public MyHandler(TextActivity activity) {
			myActivity = new WeakReference<Activity>(activity);
		}

		@Override
		public void handleMessage(Message msg) {
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..start====== ");
			//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg.what====== " + msg.what);
			TextActivity activity = (TextActivity) myActivity.get();
			if (activity != null) {
				// handleMessage
			}
			if (msg!=null) {
				handlerHandleMsg(msg);
			}
		}

	};

	private static void handlerHandleMsg(Message msg) {
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss", Locale.US);
		//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId: " + Configer.getValue("peer_id"));
		//Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId: " + msg.getData().getString("gpid"));
		if (msg.what == 1) {
			remotePeerId = msg.getData().getString("gpid");
			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..current peerId:======" + currentPeerId);
			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..from peerId:======" + remotePeerId);
			Log.i(Global.TAG, "VideoActivity Handler handleMessage>>>..msg:======" + msg.getData().getString("msg"));
			if (remotePeerId.equals(currentPeerId)) {
				return;
			}
			Map<String, String> msgMap = Global.parseMsg(msg.getData().getString("msg"));
			String msgType = msgMap.get("msg_type");
			if (Global.MSG_TYPE_TEXT_CHATTING_SEND_MSG.equals(msgType)) {
				// ///////////////////////////////////////////////////////////////////////
				// MSG_TYPE_TEXT_CHATTING_MSG
				dialogAdapter.addList("0," + msgMap.get("msg") + "," + friendNickname);
				listView.setSelection(listView.getBottom());
				/*
				 * Editable text = editText_receive_msg.getText();
				 * text.append(friendNickname + " " + format.format(new Date())
				 * + "\n" + msgMap.get("msg") + "\n");
				 */
			}
		} 
	}


}
