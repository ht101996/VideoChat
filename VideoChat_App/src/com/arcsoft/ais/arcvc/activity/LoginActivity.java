package com.arcsoft.ais.arcvc.activity;

import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.SdCardUtils;
import com.arcsoft.coreapi.sdk.CoreVCAPI;
import com.arcsoft.coreapi.sdk.CoreVCDef.LoginParam;
import com.arcsoft.coreapi.sdk.CoreVCDef.LoginResponse;
import com.arcsoft.coreapi.sdk.CoreVCDef.VCParam;

/**
 * Activity which displays a login screen to the user, offering registration as
 * well.
 */
public class LoginActivity extends Activity {
	private final static String tag = LoginActivity.class.getSimpleName();
	/**
	 * A dummy authentication store containing known user names and passwords.
	 * TODO: remove after connecting to a real authentication system.
	 */
	private static final String[] DUMMY_CREDENTIALS = new String[] { "foo@example.com:hello", "bar@example.com:world" };

	/**
	 * The default email to populate the email field with.
	 */
	public static final String EXTRA_EMAIL = "com.example.android.authenticatordemo.extra.EMAIL";

	/**
	 * Keep track of the login task to ensure we can cancel it if requested.
	 */
	private UserLoginTask mAuthTask = null;

	// Values for email and password at the time of the login attempt.
	private String mEmail;
	private String mPassword;

	// UI references.
	private EditText mEmailView;
	private EditText mPasswordView;
	private View mLoginFormView;
	private View mLoginStatusView;
	private TextView mLoginStatusMessageView;
	private Button startSocketBtn;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
//		Log.i(Global.TAG, "LoginActivity: onCreate>>>...");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_login);
		//load config 
		Configer.init(getAssets());
		try {
			//for c
			String sdCardPath = SdCardUtils.getSdCardPath();
			String gpid = SdCardUtils.initIniFile(getAssets(), sdCardPath, getApplicationContext());
			//Log.i(Global.TAG, "macAddress:" + CommonUtils.getLocalMacAddressFromIp(context));
			//Configer.setValue("device_id", CommonUtils.getLocalMacAddressFromIp(getApplicationContext()));
			Configer.setValue("peer_id", gpid);
			Log.d(tag, "Curren gpid:"+gpid);
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		// Set up the login form.
		mEmail = getIntent().getStringExtra(EXTRA_EMAIL);
		mEmailView = (EditText) findViewById(R.id.email);
		if (mEmailView.getText() == null || "".equals(mEmailView.getText().toString())) {
			if(android.os.Build.VERSION.SDK_INT > 20)
				mEmail = "mm@aa.aa" ;
			else
				mEmail = "gg@aa.aa" ;
		}
		mEmailView.setText(mEmail);

		mPasswordView = (EditText) findViewById(R.id.password);
		if (mPasswordView.getText() == null || "".equals(mPasswordView.getText().toString())) {
			mPassword = "000000" ;
		}
		mPasswordView.setText(mPassword);
		mPasswordView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
				if (id == R.id.login || id == EditorInfo.IME_NULL) {
					attemptLogin();
					return true;
				}
				return false;
			}
		});

		mLoginFormView = findViewById(R.id.login_form);
		mLoginStatusView = findViewById(R.id.login_status);
		mLoginStatusMessageView = (TextView) findViewById(R.id.login_status_message);
		
		//P2PClientManager.getP2PClientInstance().startRTPSession("50ccf82fbd5d50ccf82fbd5d50ccf82fbd5dffff");
		findViewById(R.id.sign_in_button).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				attemptLogin();
			}
		});
		
//		findViewById(R.id.test_in_button).setOnClickListener(new View.OnClickListener() {
//			@Override
//			public void onClick(View view) {
//				try {
//					P2PClientManager.getP2PClientInstance().startRTPSession("bef5acac974fbef5acac974fbef5acac974fffff");
//					VideoUtil.startVideoRecording(SdCardUtils.getSdCardPath()+"/video/out.h264");
//				} catch (Exception e1) {
//					// TODO Auto-generated catch block
//		 			e1.printStackTrace();
//				}
//			}
//		});
//		startSocketBtn = (Button)findViewById(R.id.socket_test_button) ;
//		startSocketBtn.setText("start sending");
//		startSocketBtn.setOnClickListener(new View.OnClickListener() {
//			@Override
//			public void onClick(View view) {
//				try {
//					Log.d(Global.TAG, "socket_test_button --------------TestSocketClient.isSending=="
//							+TestSocketClient.isSending);
//					
//					if (TestSocketClient.isSending == true) {
//						String ip = ((EditText)findViewById(R.id.editText_socket_server_ip)).getText().toString();
//						//TestSocketClient.start(SdCardUtils.getSdCardPath()+"/video/out.h264",ip);
//						TestSocketClient.start(SdCardUtils.getSdCardPath()+"/video/out.h264",ip);
//						startSocketBtn.setText("stop sending");
//					} else {
//						TestSocketClient.stop();
//						startSocketBtn.setText("start sending");
//					}
//					
//					//VideoUtil.startVideoRecording(SdCardUtils.getSdCardPath()+"/video/out.h264");
//				} catch (Exception e1) {
//					// TODO Auto-generated catch block
//		 			e1.printStackTrace();
//				}
//			}
//		});
//
//		
//		TextView local_ip_TextView = ((TextView)findViewById(R.id.local_ip));
//		local_ip_TextView.setText(local_ip_TextView.getText() +":"+ NetworkUtils.getLocalIpAddress2(getApplicationContext()));
//		
		Log.i(Global.TAG, "LoginActivity: onCreate============finished!");
		Timer timer = new Timer();
		timer.schedule(new TimerTask() {
			
			@Override
			public void run() {
				autoLogin();
				
			}
		}, 500);
		
	}

	@Override
	protected void onDestroy() {
		Log.i(Global.TAG, "LoginActivity --------------onDestroy!");
//		try {
//			P2PClientManager.getP2PClientInstance().uninit();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
		super.onDestroy();
	}
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		getMenuInflater().inflate(R.menu.login, menu);
		return true;
	}

	/**
	 * Attempts to sign in or register the account specified by the login form.
	 * If there are form errors (invalid email, missing fields, etc.), the
	 * errors are presented and no actual login attempt is made.
	 */
	public void attemptLogin() {
		if (mAuthTask != null) {
			return;
		}

		// Reset errors.
		mEmailView.setError(null);
		mPasswordView.setError(null);

		// Store values at the time of the login attempt.
		mEmail = mEmailView.getText().toString();
		mPassword = mPasswordView.getText().toString();

		boolean cancel = false;
		View focusView = null;

		// Check for a valid password.
		if (TextUtils.isEmpty(mPassword)) {
			mPasswordView.setError(getString(R.string.error_field_required));
			focusView = mPasswordView;
			cancel = true;
		} else if (mPassword.length() < 4) {
			mPasswordView.setError(getString(R.string.error_invalid_password));
			focusView = mPasswordView;
			cancel = true;
		}

		// Check for a valid email address.
		if (TextUtils.isEmpty(mEmail)) {
			mEmailView.setError(getString(R.string.error_field_required));
			focusView = mEmailView;
			cancel = true;
		} else if (!mEmail.contains("@")) {
			mEmailView.setError(getString(R.string.error_invalid_email));
			focusView = mEmailView;
			cancel = true;
		}

		if (cancel) {
			// There was an error; don't attempt login and focus the first
			// form field with an error.
			focusView.requestFocus();
		} else {
			// Show a progress spinner, and kick off a background task to
			// perform the user login attempt.
			mLoginStatusMessageView.setText(R.string.login_progress_signing_in);
			showProgress(true);
			mAuthTask = new UserLoginTask();
			mAuthTask.execute((Void) null);
		}
	}

	
	private void autoLogin() {
		// Store values at the time of the login attempt.
		mEmail = mEmailView.getText().toString();
		mPassword = mPasswordView.getText().toString();
		mAuthTask = new UserLoginTask();
		mAuthTask.execute((Void) null);
	}
	
	/**
	 * Shows the progress UI and hides the login form.
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	private void showProgress(final boolean show) {
		// On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
		// for very easy animations. If available, use these APIs to fade-in
		// the progress spinner.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
			int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);

			mLoginStatusView.setVisibility(View.VISIBLE);
			mLoginStatusView.animate().setDuration(shortAnimTime).alpha(show ? 1 : 0).setListener(new AnimatorListenerAdapter() {
				@Override
				public void onAnimationEnd(Animator animation) {
					mLoginStatusView.setVisibility(show ? View.VISIBLE : View.GONE);
				}
			});

			mLoginFormView.setVisibility(View.VISIBLE);
			mLoginFormView.animate().setDuration(shortAnimTime).alpha(show ? 0 : 1).setListener(new AnimatorListenerAdapter() {
				@Override
				public void onAnimationEnd(Animator animation) {
					mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
				}
			});
		} else {
			// The ViewPropertyAnimator APIs are not available, so simply show
			// and hide the relevant UI components.
			mLoginStatusView.setVisibility(show ? View.VISIBLE : View.GONE);
			mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
		}
	}

	/**
	 * Represents an asynchronous login/registration task used to authenticate
	 * the user.
	 */
	public class UserLoginTask extends AsyncTask<Void, Void, Boolean> {
		@Override
		protected Boolean doInBackground(Void... params) {
			// TODO: attempt authentication against a network service.
			
			VCParam param = new VCParam();
			param.szVCServer = Configer.getValue("vc_server");
			param.szAppKey = Configer.getValue("app_key");
			param.szAppSecret = Configer.getValue("app_secret");
			param.szDeviceId = Configer.getValue("device_id");
			param.szDeviceModel = Configer.getValue("device_model");
			param.szPeerId = Configer.getValue("peer_id");
			Log.i(Global.TAG, "param.szPeerId=" + param.szPeerId);
			Log.i(Global.TAG, "Configer.getValue(peer_id)=" + Configer.getValue("peer_id"));
			CoreVCAPI.getInstance().init(param);
//			Log.i(Global.TAG, "szVCServer==" + CoreVCAPI.getInstance().getVCServer());
//			Log.i(Global.TAG, "szAppKey==" + CoreVCAPI.getInstance().getAppKey());
//			Log.i(Global.TAG, "szAppSecret==" + CoreVCAPI.getInstance().getAppSecret());
//			Log.i(Global.TAG, "szDeviceId==" + CoreVCAPI.getInstance().getDeviceId());
//			Log.i(Global.TAG, "szDeviceModel==" + CoreVCAPI.getInstance().getDeviceModel());
//			Log.i(Global.TAG, "szPeerId==" + CoreVCAPI.getInstance().getPeerId());
			
			LoginParam loginParam = new LoginParam();
			LoginResponse loginResponse = new LoginResponse();
//			loginParam.szUsername = Configer.getValue("user_name");
//			loginParam.szPassword = Configer.getValue("password");

			loginParam.szUsername = mEmail;
			loginParam.szPassword = mPassword;
			Log.i(Global.TAG, "mEmail =" + mEmail);
			Log.i(Global.TAG, "mPassword =" + mPassword);
			int ret = CoreVCAPI.getInstance().login(loginParam, loginResponse);
//			Log.i(Global.TAG, "login ret = " + ret);
			if(0 == ret) {
//				Log.i(Global.TAG, "user id = " + loginResponse.lUserId);
//				Log.i(Global.TAG, "token = " + loginResponse.szToken);
//				Log.i(Global.TAG, "Status = " + loginResponse.szStatus);
//				Log.i(Global.TAG, "nickname = " + loginResponse.szNickname);
//
				Configer.setValue("token", loginResponse.szToken);
				Configer.setValue("user_id", String.valueOf(loginResponse.lUserId));
				Configer.setValue("username", mEmail);
				Configer.setValue("nickname", loginResponse.szNickname);
			}else{
//				Log.i(Global.TAG, "loginResponse ret = " + ret);
				return false;
			}
//			try {
//				// Simulate network access.
//				Thread.sleep(2000);
//			} catch (InterruptedException e) {
//				return false;
//			}

			for (String credential : DUMMY_CREDENTIALS) {
				String[] pieces = credential.split(":");
				if (pieces[0].equals(mEmail)) {
					// Account exists, return true if the password matches.
					return pieces[1].equals(mPassword);
				}
			}

			// TODO: register the new account here.
			return true;
		}
		
		@Override
		protected void onPostExecute(final Boolean success) {
			mAuthTask = null;
			showProgress(false);

			if (success) {
				  /* 新建一个Intent对象 */
		        Intent intent = new Intent();
//		        intent.putExtra("userId","LeiPei");    
		        /* 指定intent要启动的类 */
		        intent.setClass(LoginActivity.this, FriendListActivity.class);
		        /* 启动一个新的Activity */
		        LoginActivity.this.startActivity(intent);
		        /* 关闭当前的Activity */
				finish();
			} else {
				mPasswordView.setError(getString(R.string.error_incorrect_password));
				mPasswordView.requestFocus();
			}
		}

		@Override
		protected void onCancelled() {
			mAuthTask = null;
			showProgress(false);
		}
	}
}
