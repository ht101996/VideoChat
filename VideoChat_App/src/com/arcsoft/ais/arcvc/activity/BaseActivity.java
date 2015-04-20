package com.arcsoft.ais.arcvc.activity;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.R.layout;
import com.arcsoft.ais.arcvc.R.menu;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.text.Editable;
import android.view.Menu;

public class BaseActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		return true;
	}

	private static Handler handler = new Handler() {
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss"); 
		@Override
		public void handleMessage(Message msg) {
//			if (msg.what == 1) { 
//				Editable text = editText_receive_msg.getText();
//				text.append(friendNickname + " " + format.format(new Date()) 
//						+ "\n" + msg.getData().getString("msg") + "\n");
//			}
		}
	};

	public static Handler getHandler() {
		return handler;
	}
}
